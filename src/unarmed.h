#pragma once

namespace Unarmed {

	// Hit Event to give Pickpocket xp to unarmed hits for SPERG
	// Credits colinswrath: https://github.com/colinswrath/handtohand/blob/main/src/Events.h
	struct HitEvent : public REX::Singleton<HitEvent>{
		void ProcessUnarmed(const RE::TESHitEvent* event);
	};
	struct Helper {
		static bool IsBeastRace();
		static void GiveHandToHandXP();
		static inline constexpr RE::ActorValue skill_to_use = RE::ActorValue::kOneHanded;
	};
}