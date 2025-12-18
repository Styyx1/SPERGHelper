#pragma once

namespace Hooks {
	struct LearnSpellsFromStaff {
		RE::ActorMagicCaster* caster;
		static void SpellCast(RE::ActorMagicCaster* a_this, bool a_doCast, std::uint32_t a_arg2, RE::MagicItem* a_spell);
		
		//RE::ActorMagicCaster::VTABLE[0], 0xA, CheckCast
		static inline REL::HookVFT _SpellCastHook{ RE::ActorMagicCaster::VTABLE[0], 0x9, SpellCast };

		static RE::SpellItem* FindMatchingSpellWithEffectFromList(RE::EffectSetting* a_effect);

		static bool AddSpellActor(RE::Actor* actor, RE::SpellItem* a_spell) {
			return StyyxUtil::FuncCall<38716, bool>(actor, a_spell);
		}
	};

	struct SpellLearnData : REX::Singleton<SpellLearnData> {
		RE::SpellItem* a_spell;
		float a_spell_xp;
		std::unordered_map<RE::SpellItem*, float> spell_xp_map;
		void PopulateXPMapOnStart();
		void AddXPToSpell(RE::SpellItem* a_spell);
		void LearnSpell(RE::Actor* actor, RE::SpellItem* a_spell);

	};
}
