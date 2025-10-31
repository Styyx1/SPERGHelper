#include "unarmed.h"
#include "config.h"

namespace Unarmed {
	void HitEvent::Register()
	{
		auto src = RE::ScriptEventSourceHolder::GetSingleton();
		src->AddEventSink(this);
		REX::INFO("Registered for {}", typeid(RE::TESHitEvent).name());
	}
	EventResult HitEvent::ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*)
	{
		using flag = RE::TESHitEvent::Flag;
		if(!event)
			return EventResult::kContinue;
		if(!event->target || event->cause)
			return EventResult::kContinue;

		auto defender = event->target.get()->As<RE::Actor>();
		auto attacker = event->cause.get()->As<RE::Actor>();

		if(!defender || !attacker || !attacker->IsPlayerRef())
			return EventResult::kContinue;

		auto defenderProcess = defender->currentProcess;
		auto attackerProcess = attacker->currentProcess;
		auto attackingWeapon = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);

		if (!attackerProcess || !attackingWeapon || !defenderProcess || !defenderProcess->high || !attackingWeapon->IsMelee() || !defender->Get3D())
			return EventResult::kContinue;
		auto& attackData = attackerProcess->high->attackData;
		if(!attackData)
			return EventResult::kContinue;
		if ((defender->GetLifeState() != RE::ACTOR_LIFE_STATE::kDead) && !Helper::IsBeastRace() && attackingWeapon->IsHandToHandMelee()) {
			Helper::GiveHandToHandXP();
		}
		return EventResult::kContinue;
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
	void Install()
	{
		HitEvent::GetSingleton()->Register();
	}
}


