#pragma once

namespace Config {
	namespace DATA {
		static inline constexpr std::string_view toml_path_default{ "Data/SKSE/Plugins/sperg.toml" };
		static inline constexpr std::string_view toml_path_custom{ "Data/SKSE/Plugins/sperg_custom.toml" };
		static inline constexpr std::string_view MAIN_SETTINGS { "Settings" };
	}
	using namespace DATA;
	struct Settings : public REX::Singleton<Settings> {
		
		static inline REX::TOML::F64 h2h_base_xp{ MAIN_SETTINGS, "fBaseH2Hxp", 3.0 };
		static inline REX::TOML::F64 h2h_bonus_xp{ MAIN_SETTINGS, "fBonusH2Hxp", 0.15 };
		static inline REX::TOML::Bool lockbash_no_xp{ MAIN_SETTINGS, "bLockbashNoXP", false };

		static inline REX::TOML::Str break_sound_success_name{ MAIN_SETTINGS,"sBreakSoundSuccessName", std::string("TRPTripwireSD") };
		static inline REX::TOML::Str break_sound_fail_name{ MAIN_SETTINGS, "sBreakSoundFailName", std::string("DRSLockedSD") };
		static inline REX::TOML::Str break_success_message{ MAIN_SETTINGS, "sBreakSuccessMessage", std::string(" broke the lock.") };
		static inline REX::TOML::Str break_fail_message{ MAIN_SETTINGS, "sBreakFailMessage", std::string(" can not break the lock!") };

		static inline REX::TOML::Bool play_sound_and_message{ MAIN_SETTINGS, "bPlaySoundAndMessage", true };

		void UpdateSettings(bool a_save) {
			const auto toml = REX::TOML::SettingStore::GetSingleton();
			toml->Init(toml_path_default.data(), toml_path_custom.data());
			if (!a_save)
				toml->Load();
			else
				toml->Save();
		}

	};
	struct Forms : REX::Singleton<Forms> {
		inline static RE::TESObjectWEAP* unarmed_weapon{};
		void LoadForms() {
			const auto dh = RE::TESDataHandler::GetSingleton();
			unarmed_weapon = dh->LookupForm<RE::TESObjectWEAP>(0x1f4, "Skyrim.esm");
		};
	};
}
