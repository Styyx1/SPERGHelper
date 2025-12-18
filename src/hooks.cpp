#include "hooks.h"
#include "config.h"

namespace Hooks {

	void LearnSpellsFromStaff::SpellCast(RE::ActorMagicCaster* a_this, bool a_doCast, std::uint32_t a_arg2, RE::MagicItem* a_spell)
	{
		_SpellCastHook(a_this, a_doCast, a_arg2, a_spell);

		if (!a_spell || !a_doCast)
			return;

		if (a_spell->GetSpellType() == RE::MagicSystem::SpellType::kStaffEnchantment) {
			auto actor = a_this->GetCasterAsActor();
			if (!actor)
				return;	

			if (actor->IsPlayerRef()) {

				if (!actor->HasPerk(Config::Forms::artificer_perk))
					return;

				if (a_spell) {					
					RE::SpellItem* spell{ nullptr };
					auto data = SpellLearnData::GetSingleton();
					for (auto& effect : a_spell->effects) {
						if (effect->baseEffect) {
							spell = FindMatchingSpellWithEffectFromList(effect->baseEffect);
							if (spell) {
								data->AddXPToSpell(spell);								
								break;
							}							
						}
					}
					if (spell) {
						data->LearnSpell(actor, spell);
					}
				}
			}
		}
	}

	RE::SpellItem* LearnSpellsFromStaff::FindMatchingSpellWithEffectFromList(RE::EffectSetting* a_effect)
	{
		if (Config::Forms::spell_set.empty()) {
			return nullptr;
		}
		for (auto& spell : Config::Forms::spell_set) {
			for (auto& effect : spell->effects) {
				if (effect->baseEffect == a_effect) {
					return spell;
				}
			}
		}
		return nullptr;
	}
	void SpellLearnData::PopulateXPMapOnStart() {
		for (auto& spell : Config::Forms::spell_set) {
			spell_xp_map[spell] = 0.f;
		}
	}
	void SpellLearnData::AddXPToSpell(RE::SpellItem* a_spell)
	{
		if (!a_spell) {
			return;
		}

		auto it = spell_xp_map.find(a_spell);
		if (it == spell_xp_map.end()) {
			return;
		}
		if (it->second > Config::Settings::base_xp_needed_for_spell_learning.GetValue())
			return;
		it->second += 1.f;
		if (Config::Settings::show_spell_learn_notif.GetValue()) {
			std::string msg = std::format("{} has now {} xp, use it {} more times to learn the spell", a_spell->GetName(), it->second, Config::Settings::base_xp_needed_for_spell_learning.GetValue() - it->second);
			RE::ConsoleLog::GetSingleton()->Print(msg.c_str());
		}		
	}
	void SpellLearnData::LearnSpell(RE::Actor* actor, RE::SpellItem* a_spell) {
		if (!actor || !a_spell)
			return;

		if (actor->HasSpell(a_spell)) {
			spell_xp_map.erase(a_spell);
			return;
		}

		auto it = spell_xp_map.find(a_spell);
		if (it == spell_xp_map.end()) {
			return;
		}
		if (it->second >= Config::Settings::base_xp_needed_for_spell_learning.GetValue()) {
			actor->AddSpell(a_spell);
			RE::SendHUDMessage::ShowHUDMessage("You learned {}", a_spell->GetName());
			spell_xp_map.erase(it);							
		}
	}
}