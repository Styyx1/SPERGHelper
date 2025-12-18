#pragma once
#include "mod-data.h"

namespace Config {

	struct Settings : public REX::Singleton<Settings>, public MOD {
		
		static inline REX::TOML::F64 h2h_base_xp{ HAND_TO_HAND, "fBaseH2Hxp", 3.0 };
		static inline REX::TOML::F64 h2h_bonus_xp{ HAND_TO_HAND, "fBonusH2Hxp", 0.15 };
		static inline REX::TOML::Bool lockbash_no_xp{ HAND_TO_HAND, "bLockbashNoXP", false };

		static inline REX::TOML::Str break_sound_success_name{ LOCK_SETTINGS,"sBreakSoundSuccessName", std::string("TRPTripwireSD") };
		static inline REX::TOML::Str break_sound_fail_name{ LOCK_SETTINGS, "sBreakSoundFailName", std::string("DRSLockedSD") };
		static inline REX::TOML::Str break_success_message{ LOCK_SETTINGS, "sBreakSuccessMessage", std::string(" broke the lock.") };
		static inline REX::TOML::Str break_fail_message{ LOCK_SETTINGS, "sBreakFailMessage", std::string(" can not break the lock!") };
		static inline REX::TOML::Bool play_sound_and_message{ LOCK_SETTINGS, "bPlaySoundAndMessage", true };

		static inline REX::TOML::Bool show_spell_learn_notif{ SPELL_LEARNING, "bShowXPNotification", true };
		static inline REX::TOML::F32 base_xp_needed_for_spell_learning{ SPELL_LEARNING, "fBaseXPForSpellLearning", 100.0f };


		void UpdateSettings(bool a_save) {
			const auto toml = REX::TOML::SettingStore::GetSingleton();
			toml->Init(TOML_PATH_DEFAULT.data(), TOML_PATH_CUSTOM.data());
			if (!a_save)
				toml->Load();
			else
				toml->Save();
		}
	};

	struct SpellLoader : REX::Singleton<SpellLoader>, MOD{

		static inline REX::JSON::StrA spell_allow_list{ "LearnSpells", {"Firebolt"} };

		void UpdateSpellLoader(bool a_save) {
			const auto json = REX::JSON::SettingStore::GetSingleton();
			json->Init(MOD::JSON_PATH_DEFAULT.data(), MOD::JSON_PATH_CUSTOM.data());
			if (!a_save)
				json->Load();
			else
				json->Save();
		}
	};

	struct Forms : REX::Singleton<Forms>, public MOD {
		inline static RE::TESObjectWEAP* unarmed_weapon{};
		inline static RE::BGSPerk* lockbash_perk{};
		inline static RE::BGSPerk* learn_from_staves_perk{};
		inline static RE::BGSPerk* artificer_perk{};
		inline static RE::BGSPerk* quiet_casting_perk{};
		inline static RE::BGSListForm* learn_from_staves_formlist{};

		inline static std::set<RE::SpellItem*> spell_set;

		void PopulateSpellSetFromFormlist() {
			if (learn_from_staves_formlist) {
				learn_from_staves_formlist->ForEachForm([&](RE::TESForm* a_formInList) {
					auto spell = a_formInList->As<RE::SpellItem>();
					if (spell)
					{
						spell_set.insert(spell);
						REX::DEBUG("insterted {}", spell->GetName());
						return RE::BSContainer::ForEachResult::kContinue;
					}
					return RE::BSContainer::ForEachResult::kContinue;
					});
			}
			if (!Config::SpellLoader::spell_allow_list.GetValue().empty()) {
				REX::INFO("start reading json");
				for (auto& spell : Config::SpellLoader::spell_allow_list.GetValue()) {
					auto* form = FormUtil::GetFormFromString(spell);
					if (!form) {
						REX::WARN("Invalid form string: {}", spell);
						continue;
					}
					auto* da_spell = form->As<RE::SpellItem>();
					if (!da_spell) {
						REX::WARN("Form is not a spell: {}", spell);
						continue;
					}
					auto res = spell_set.insert(da_spell);
					if(res.second)
						REX::DEBUG("inserted {} from json", da_spell->GetName());
				}
			}
		};

		void LoadForms() {
			const auto dh = RE::TESDataHandler::GetSingleton();
			unarmed_weapon = dh->LookupForm<RE::TESObjectWEAP>(UNARMED_ID, SKYRIM_NAME);

			if(!MiscUtil::IsModLoaded(SPERG_MOD))
				REX::FAIL("Can not find {}, please make sure to activate the mod", SPERG_MOD);

			lockbash_perk = dh->LookupForm<RE::BGSPerk>(LOCK_BASH_PERK_ID, SPERG_MOD);
			learn_from_staves_formlist = dh->LookupForm<RE::BGSListForm>(STAFF_SPELL_LEARN_LIST_ID, SPERG_MOD);
			artificer_perk = dh->LookupForm<RE::BGSPerk>(ARTIFICER_PERK_ID, SPERG_MOD);
			quiet_casting_perk = dh->LookupForm<RE::BGSPerk>(QUIET_CASTING_PERK_ID, SPERG_MOD);

			PopulateSpellSetFromFormlist();
		};
	};
}
