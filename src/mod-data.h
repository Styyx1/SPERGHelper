#pragma once

struct MOD {
	static inline constexpr std::string_view TOML_PATH_DEFAULT = "Data/SKSE/Plugins/sperg.toml";
	static inline constexpr std::string_view TOML_PATH_CUSTOM = "Data/SKSE/Plugins/sperg_custom.toml";
	static inline constexpr std::string_view JSON_PATH_DEFAULT = "Data/SKSE/Plugins/sperg-spells.json";
	static inline constexpr std::string_view JSON_PATH_CUSTOM = "Data/SKSE/Plugins/sperg-spells_custom.json";

	static inline constexpr std::string_view MAIN_SETTINGS{ "General Settings" };
	static inline constexpr std::string_view HAND_TO_HAND{ "Hand to Hand" };
	static inline constexpr std::string_view LOCK_SETTINGS{ "Lock Bashing" };
	static inline constexpr std::string_view SPELL_LEARNING{ "Spell Learning" };

	static inline constexpr std::string_view SKYRIM_NAME = "Skyrim.esm";
	static inline constexpr RE::FormID UNARMED_ID = 0x1f4;

	static inline constexpr std::string_view SPERG_MOD = "SPERG-SSE.esp";
	static inline constexpr RE::FormID LOCK_BASH_PERK_ID = 0x51275;
	static inline constexpr RE::FormID STAFF_SPELL_LEARN_LIST_ID = 0x21B1A;
	static inline constexpr RE::FormID ARTIFICER_PERK_ID = 0x3AC5F;
	static inline constexpr RE::FormID QUIET_CASTING_PERK_ID = 0x0;

	// currently not needed cause after removing the float templates from REX::JSON, the json array works for me
	static inline constexpr std::string_view SPERG_HELPER_MOD = "Sperg-Helper.esp";
	static inline constexpr RE::FormID FORMLIST2_FOR_SPELL_LEARNING_ID = 0x1;
	static inline constexpr RE::FormID FORMLIST3_FOR_SPELL_LEARNING_ID = 0x2;

};
