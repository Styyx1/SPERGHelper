#include "lockbash.h"
#include "config.h"

namespace Lockbash {

	constexpr auto very_easy{ RE::LOCK_LEVEL::kVeryEasy };
	constexpr auto easy{ RE::LOCK_LEVEL::kEasy };
	constexpr auto average{ RE::LOCK_LEVEL::kAverage };
	constexpr auto hard{ RE::LOCK_LEVEL::kHard };
	constexpr auto very_hard{ RE::LOCK_LEVEL::kVeryHard };

	const std::unordered_map<RE::LOCK_LEVEL, float> lock_map{
		{ RE::LOCK_LEVEL::kVeryEasy, 0.f },
		{ RE::LOCK_LEVEL::kEasy, 25.f },
		{ RE::LOCK_LEVEL::kAverage, 50.f},
		{RE::LOCK_LEVEL::kHard, 75.f},
		{RE::LOCK_LEVEL::kVeryHard, 100.f },
		{RE::LOCK_LEVEL::kRequiresKey, 255.f }
    };

	void OnHitEventHandler::ProcessLockHit(const RE::TESHitEvent* a_event)
	{
		using HFlag = RE::TESHitEvent::Flag;
		if (!a_event) {
			return;
		}
		if (!a_event->cause) {
			return;
		}
		auto actor = a_event->cause.get()->As<RE::Actor>();
		if (!actor || !actor->IsPlayerRef()) {
			return;
		}
		RE::TESObjectREFR* locked_ref = a_event->target.get();
		if (!locked_ref) {
			return;
		}
		bool isLocked = locked_ref->IsLocked();
		if (!isLocked) {
			return;
		}

		bool attackCanBreakLock = ActorUtil::IsPowerAttacking(actor) || ActorUtil::IsBashing(actor);
		if (!attackCanBreakLock) {
			return;
		}

		auto weapon_used = RE::TESForm::LookupByID<RE::TESObjectWEAP>(a_event->source);
		if (!weapon_used) {
			return;
		}
		if (weapon_used) {
			REX::DEBUG("attacking weapon is: {}", weapon_used->IsHandToHandMelee() ? "nothing" : weapon_used->GetName());
		}

		if (weapon_used->IsRanged()) {
			return;
		}
		if (actor->HasPerk(Config::Forms::lockbash_perk) || (actor->IsPlayerTeammate() && RE::PlayerCharacter::GetSingleton()->HasPerk(Config::Forms::lockbash_perk))) {
			if (GenerateNotificationAndSound(locked_ref->GetLockLevel(), actor)) {
				ProcessHit(locked_ref->GetLock(), locked_ref, 50, 10);
				CheckCrime(locked_ref, 512, actor);
			}
		}
	}

	float OnHitEventHandler::GetBashLevel(RE::Actor* actor)
	{
		float bash_level = 1.f;
		bool isShield = actor->GetEquippedObject(true) && actor->GetEquippedObject(true)->IsArmor();
		if (isShield) {
			auto shield = actor->GetEquippedObject(true)->As<RE::TESObjectARMO>();
			if (shield) {
				bash_level = shield->GetArmorRating() * 4.0f;
			}
		}
		else {
			auto weap = actor->GetEquippedObject(false)->As<RE::TESObjectWEAP>();
			if (weap && !weap->IsRanged()) {
				if (!weap->IsHandToHandMelee()) {
					bash_level = static_cast<float>(weap->GetAttackDamage()) * 8.0f;
				}
				else {
					bash_level = actor->GetActorValue(RE::ActorValue::kUnarmedDamage) * 8.0f;
				}
			}
		}		
		REX::DEBUG("bash level is: {}", bash_level);
		return std::clamp(bash_level, 0.0f, 254.f);
	}

    void OnHitEventHandler::ProcessHit(RE::REFR_LOCK* lock, RE::TESObjectREFR* locked, int alarm_value, float xp_gain) noexcept
    {
        lock->SetLocked(false);
        REX::DEBUG("Unlocked {}", locked->GetName());

        FinalizeUnlock(locked->AsReference());
        REX::DEBUG("Finalized unlock for {}", locked->GetName());

        const auto player{ RE::PlayerCharacter::GetSingleton() };
        player->UpdateCrosshairs();
        RE::PlaySound(break_sound_to_use);

        if (!Config::Settings::lockbash_no_xp.GetValue()) {
            player->AddSkillExperience(RE::ActorValue::kLockpicking, xp_gain);
            REX::DEBUG("Added {} {} XP", xp_gain, "Lockpicking");
        }
    }
	float OnHitEventHandler::CalculateCrimeGold(RE::Actor* witness)
	{
		auto fac = witness->GetCrimeFaction();
		return static_cast<float>(fac->crimeData.crimevalues.trespassCrimeGold);
	}

    void OnHitEventHandler::CheckCrime(RE::TESObjectREFR* unlockedObj, float radius, RE::Actor* attacker)
    {
        if (!unlockedObj || !attacker)
            return;
        auto gold = RE::BGSDefaultObjectManager::GetSingleton()->GetObject(RE::DEFAULT_OBJECT::kGold);
        int detected_by_count = 0;
        // Capture 'this' in the lambda to allow member access
        RE::TES::GetSingleton()->ForEachReferenceInRange(unlockedObj, radius, [attacker, &unlockedObj, &detected_by_count, this](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult {
            if (!ref->IsPlayerRef() && ref->Is(RE::FormType::ActorCharacter) && ref->HasKeywordWithType(RE::DEFAULT_OBJECT::kKeywordNPC)) {
                RE::Actor* const actor = ref->As<RE::Actor>();
                if (actor && !actor->IsDead() && !actor->IsPlayerTeammate() && actor != attacker) {
                    RE::TESFaction* const crime_faction = actor->GetCrimeFaction();
                    int32_t detection_level = actor->RequestDetectionLevel(attacker);
                    float morality_level = actor->GetActorValue(RE::ActorValue::kMorality);
                    uint16_t crime_gold = crime_faction ? crime_faction->crimeData.crimevalues.trespassCrimeGold : 50;
                    auto owner = unlockedObj->GetOwner();
                    if (!owner) {
                        owner = GetOwnerFactionDoor(unlockedObj);
						if (!owner) {
							owner = crime_faction;
						}
                    }
					if (owner) {
						if (detection_level > 0 && detected_by_count == 0) {
							if (morality_level > 0) {
								if (owner == crime_faction) {
									if (!crime_faction->IgnoresTrespass()) {
										if (actor->IsGuard()) {
											REX::DEBUG("Report: {} [{}]", actor->GetName(), detection_level);
										}
										attacker->TrespassAlarm(ref, owner, RE::PackageNS::CRIME_TYPE::kTrespass);
										detected_by_count++;
										return RE::BSContainer::ForEachResult::kContinue;
									}
								}
								if (const auto own_fact = owner->As<RE::TESFaction>(); own_fact && owner->Is(RE::FormType::Faction)  && actor->IsInFaction(crime_faction)) {
									attacker->TrespassAlarm(ref, owner, RE::PackageNS::CRIME_TYPE::kTrespass);
									detected_by_count++;
									return RE::BSContainer::ForEachResult::kContinue;
								}
							}
						}
					}
                }
            }
            return RE::BSContainer::ForEachResult::kContinue;
        });
    }

	RE::TESForm* OnHitEventHandler::GetOwnerFactionDoor(RE::TESObjectREFR* object)
	{
		if (object->Is(RE::FormType::Door)) {
			if (const auto teleport = object->extraList.GetByType<RE::ExtraTeleport>(); teleport) {
				if (const auto teleport_data = teleport->teleportData; teleport_data) {
					if (const auto linked_door = teleport_data->linkedDoor.get().get(); linked_door) {
						if (const auto door_cell = linked_door->GetParentCell(); door_cell) {
							if (const auto door_cell_owner = door_cell->GetOwner(); door_cell_owner) {
								return door_cell_owner;
							}							
						}
					}
				}
			}
		}
		return nullptr;
	}

	bool OnHitEventHandler::GenerateNotificationAndSound(RE::LOCK_LEVEL lvl, RE::Actor* actor)
	{
		actor_name = actor->GetName();

		bool canBreak = CanBashLevelBreak(lvl, actor);
		const char* msg = "test";
		std::string_view test = "test";
		if (canBreak) {			
			full_message = std::format("{}{}", actor_name, Config::Settings::break_success_message.GetValue());
			static std::string break_sound_success_str;
			break_sound_success_str = Config::Settings::break_sound_success_name.GetValue();
			break_sound_to_use = break_sound_success_str.c_str();
			REX::DEBUG("break message: {}", full_message);
			msg = full_message.c_str();
			RE::SendHUDMessage::ShowHUDMessage(msg);
		}
		else {
			full_message = std::format("{}{}",actor_name, Config::Settings::break_fail_message.GetValue());
			static std::string break_sound_fail_str;
			break_sound_fail_str = Config::Settings::break_sound_fail_name.GetValue();
			break_sound_to_use = break_sound_fail_str.c_str();
			REX::DEBUG("break message fail: {}", full_message);
			msg = full_message.c_str();
			RE::SendHUDMessage::ShowHUDMessage(msg);
		}
		return canBreak;
		
		
	}
	bool OnHitEventHandler::CanBashLevelBreak(RE::LOCK_LEVEL lvl, RE::Actor* actor)
	{
		float bash_level = GetBashLevel(actor);
		float lock_level_float = lock_map.at(lvl);

		return lock_level_float <= bash_level;
	}
}

