#include "config.h"

namespace Config {
	void Settings::LoadSettings()
	{
		const auto toml = REX::TOML::SettingStore::GetSingleton();
		toml->Init(toml_path_default.data(), toml_path_custom.data());
		toml->Load();
		h2h_bonus_xp.SetValue(std::clamp(h2h_bonus_xp.GetValue(), 0.0, 100.0));
		h2h_base_xp.SetValue(std::max(0.0, h2h_base_xp.GetValue()));
	}
	void Settings::SaveSettings()
	{
		const auto toml = REX::TOML::SettingStore::GetSingleton();
		toml->Init(toml_path_default.data(), toml_path_custom.data());
		toml->Save();
	}
}

