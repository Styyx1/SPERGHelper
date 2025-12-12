#include "lockbash.h"
#include "config.h"

namespace Lockbash {
    /*
    
EndFunction

Event OnCrosshairRefChange(ObjectReference ref)
	If ref != NONE && ref.IsLocked()
		LockLevel = ref.GetLockLevel()
		If LockLevel <= 100
			LockedItem = ref
			GoToState("WatchingForBash")
			RegisterForSingleUpdate(0.1)
		EndIf
	Else
		GoToState("")
	EndIf
EndEvent

State WatchingForBash
	Event OnUpdate()
		If PlayerRef.GetAnimationVariableBool("IsBashing") == 1
			GoToState("ListeningForCrime")
			If LockLevel <= BashLevel
				LockedItem.CreateDetectionEvent(PlayerRef, 100)
				LockedItem.Lock(false)
				SPEPerkBashLocksDetection01.Cast(PlayerRef)
				Utility.Wait(0.2) ;let the animation play a bit
				int soundID = SPEWPNImpactBlunt2HVsWood.Play(LockedItem)
				Sound.SetInstanceVolume(soundID, 1.0)
			Else
				LockedItem.CreateDetectionEvent(PlayerRef, 10)
				SPEPerkBashLocksDetection02.Cast(PlayerRef)
				Utility.Wait(0.2) ;let the animation play a bit
				int soundID = WPNImpactBluntVsWood.Play(LockedItem)
				Sound.SetInstanceVolume(soundID, 1.0)
			EndIf
		Else
			RegisterForSingleUpdate(0.1)
		EndIf
	EndEvent
EndState

Function OnBashWitnessed(string eventName, string strArg, float numArg, Form sender)
	;Debug.Trace("Crime already reported.")
EndFunction

State ListeningForCrime
	Function OnBashWitnessed(string eventName, string strArg, float numArg, Form sender)
		GoToState("")
		;Debug.Trace("Bash witnessed!")
		(sender as Actor).SendAssaultAlarm()
	EndFunction
EndState
    
    */
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

    RES OnHitEventHandler::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) noexcept
    {
		using HFlag = RE::TESHitEvent::Flag;
		if (!a_event) {
			return RES::kContinue;
		}
		if (!a_event->cause) {
			return RES::kContinue;
		}
		auto actor = a_event->cause.get()->As<RE::Actor>();
		if (!actor || !actor->IsPlayerRef()) {
			return RES::kContinue;
		}
		RE::TESObjectREFR* locked_ref = a_event->target.get();
		if (!locked_ref) {
			return RES::kContinue;
		}
		bool isLocked = locked_ref->IsLocked();
		if (!isLocked) {
			return RES::kContinue;
		}
		
		bool attackCanBreakLock = ActorUtil::IsPowerAttacking(actor) || ActorUtil::IsBashing(actor);
		if (!attackCanBreakLock) {
			return RES::kContinue;
		}

		auto weapon_used = RE::TESForm::LookupByID<RE::TESObjectWEAP>(a_event->source);
		if (!weapon_used) {
			return RES::kContinue;
		}
		if (weapon_used) {
			REX::DEBUG("attacking weapon is: {}", weapon_used->IsHandToHandMelee() ? "nothing" : weapon_used->GetName());
		}

		if (weapon_used->IsRanged()) {
			return RES::kContinue;
		}
		if (GenerateNotificationAndSound(locked_ref->GetLockLevel(), actor)) {
			ProcessHit(locked_ref->GetLock(), locked_ref, 50, 10);
		}
			

        return RES::kContinue;
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
        RE::PlaySound("UILockpickingUnlock");
        if (locked->IsCrimeToActivate()) {
            player->StealAlarm(locked->AsReference(), locked->As<RE::TESForm>(), 0, alarm_value, locked->GetOwner(), false);
            REX::DEBUG("Sent steal alarm ({} gold)", alarm_value);
        }
        if (!Config::Settings::lockbash_no_xp.GetValue()) {
            player->AddSkillExperience(RE::ActorValue::kLockpicking, xp_gain);
            REX::DEBUG("Added {} {} XP", xp_gain, "Lockpicking");
        }
    }
	void OnHitEventHandler::CalculateCrimeGold(RE::Actor* witness)
	{
		auto fac = witness->GetCrimeFaction();
	}

	bool OnHitEventHandler::GenerateNotificationAndSound(RE::LOCK_LEVEL lvl,RE::Actor* actor)
	{
		float bash_level = GetBashLevel(actor);
		float lock_level_float = lock_map.at(lvl);
		actor_name = actor->GetName();

		
		const char* msg = "test";
		std::string_view test = "test";
		if (lock_level_float <= bash_level) {			
			full_message = std::format("{}{}", actor_name, Config::Settings::break_success_message.GetValue());
			static std::string break_sound_success_str;
			break_sound_success_str = Config::Settings::break_sound_success_name.GetValue();
			break_sound_to_use = break_sound_success_str.c_str();
			REX::DEBUG("break message: {}, lock level is: {} and bash level is: {}", full_message, lock_level_float, bash_level);
			msg = full_message.c_str();
			RE::SendHUDMessage::ShowHUDMessage(msg);
			
			return true;
		}
		else {
			full_message = std::format("{}{}",actor_name, Config::Settings::break_fail_message.GetValue());
			static std::string break_sound_fail_str;
			break_sound_fail_str = Config::Settings::break_sound_fail_name.GetValue();
			break_sound_to_use = break_sound_fail_str.c_str();
			REX::DEBUG("break message fail: {}, lock level is: {} and bash level is: {}", full_message, lock_level_float, bash_level);
			msg = full_message.c_str();
			RE::SendHUDMessage::ShowHUDMessage(msg);
			return false;
		}
		
		
	}
}

