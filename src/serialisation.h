#pragma once

#include "hooks.h"

namespace Serialisation
{
	static constexpr uint32_t SerializationVersion = 1;
	static constexpr uint32_t ID = 'SPEH';
	static constexpr uint32_t SerializationType = 'SPXP';

	inline void SaveCallback(SKSE::SerializationInterface* a_skse)
	{
		if (!a_skse->OpenRecord(SerializationType, SerializationVersion)) {
			REX::ERROR("Failed to open record");
			return;
		}

		auto data = Hooks::SpellLearnData::GetSingleton();

		uint32_t count = static_cast<uint32_t>(data->spell_xp_map.size());
		a_skse->WriteRecordData(count);

		for (auto& [spell, xp] : data->spell_xp_map) {
			RE::FormID id = spell->GetFormID();
			a_skse->WriteRecordData(id);
			a_skse->WriteRecordData(xp);

			REX::INFO("Saved {} ({:08X}) XP: {}", spell->GetName(), id, xp);
		}
	}

	inline void LoadCallback(SKSE::SerializationInterface* a_skse)
	{
		uint32_t type, version, length;
		if (!a_skse->GetNextRecordInfo(type, version, length)) {
			return;
		}

		if (type != SerializationType || version != SerializationVersion) {
			REX::ERROR("Unable to load data");
			return;
		}

		auto data = Hooks::SpellLearnData::GetSingleton();

		uint32_t count = 0;
		a_skse->ReadRecordData(count);

		for (uint32_t i = 0; i < count; i++) {
			RE::FormID id;
			float xp;

			a_skse->ReadRecordData(id);
			a_skse->ReadRecordData(xp);

			RE::FormID new_id = id;
			a_skse->ResolveFormID(id, new_id);
			REX::INFO("Old id was: {:08X}, new id is: {:08X}", id, new_id);
			if (auto* form = RE::TESForm::LookupByID(new_id)) {
				if (auto* spell = form->As<RE::SpellItem>()) {
					auto it = data->spell_xp_map.find(spell);
					if (it != data->spell_xp_map.end()) {
						it->second = xp;
					}
					REX::INFO("Loaded {} with ID: {:08X} and XP: {}", spell->GetName(), new_id, xp);
				}
			}
		}
	}

	inline void RevertCallback([[maybe_unused]] SKSE::SerializationInterface* a_skse)
	{
		auto data = Hooks::SpellLearnData::GetSingleton();
		data->spell_xp_map.clear();
		data->PopulateXPMapOnStart();
	};


}