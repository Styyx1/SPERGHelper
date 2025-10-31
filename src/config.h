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
		void LoadSettings();
		void SaveSettings();
	};
}
