#include "unarmed.h"
#include "config.h"

namespace Unarmed {

	void HitEvent::ProcessUnarmed(const RE::TESHitEvent* event)
	{
		using flag = RE::TESHitEvent::Flag;
		if (!event)
			return;
		if (!event || !event->cause || !event->cause->IsPlayerRef() || event->target->IsNot(RE::FormType::ActorCharacter) || !event->source) {
			return;
		}

		auto defender = event->target->As<RE::Actor>();
		auto defenderProcess = defender->currentProcess;
		auto attackingWeapon = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);

		if (!defender || !attackingWeapon || !defenderProcess || !defenderProcess->high || !attackingWeapon->IsMelee() || !defender->Get3D()) {
			return;
		}
		auto attacker = event->cause->As<RE::Actor>();
		auto& attackData = attacker->currentProcess->high->attackData;
		if (!attackData)
			return;

		if ((defender->GetLifeState() != RE::ACTOR_LIFE_STATE::kDead) && !Helper::IsBeastRace() && attackingWeapon->IsHandToHandMelee()) {
			Helper::GiveHandToHandXP();
			return;
		}
		return;
	}	
	bool Helper::IsBeastRace()
	{
		RE::MenuControls* MenuControls = RE::MenuControls::GetSingleton();
		return MenuControls->InBeastForm();
	}
	void Helper::GiveHandToHandXP() 
	{
		auto player = RE::PlayerCharacter::GetSingleton();
		float H2Hlvl = player->GetActorValue(skill_to_use);
		float baseXP = (Config::Settings::h2h_bonus_xp.GetValue() * H2Hlvl) + Config::Settings::h2h_base_xp.GetValue();
		player->AddSkillExperience(skill_to_use, baseXP);
	}
}


