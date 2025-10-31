#pragma once

namespace Unarmed {
	void Install();
	using EventResult = RE::BSEventNotifyControl;

	// Hit Event to give Pickpocket xp to unarmed hits for SPERG
	// Credits colinswrath: https://github.com/colinswrath/handtohand/blob/main/src/Events.h
	struct HitEvent : public REX::Singleton<HitEvent>, public RE::BSTEventSink<RE::TESHitEvent> {

		void Register();
		EventResult ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) override;		

	};
	struct Helper {
		static bool IsBeastRace();
		static void GiveHandToHandXP();
		static inline constexpr RE::ActorValue skill_to_use = RE::ActorValue::kOneHanded;
	};
}