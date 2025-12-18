#include "config.h"
#include "events.h"
#include "hooks.h"
#include "serialisation.h"

void Listener(SKSE::MessagingInterface::Message* a_msg) {
	using msg = SKSE::MessagingInterface;
	switch (a_msg->type)
	{
	case msg::kDataLoaded:
		Config::Forms::GetSingleton()->LoadForms();
		Events::HitEvent::GetSingleton()->Register();
		Hooks::SpellLearnData::GetSingleton()->PopulateXPMapOnStart();		
	}
}

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse,{.trampoline = true});

	Config::Settings::GetSingleton()->UpdateSettings(false);
	Config::SpellLoader::GetSingleton()->UpdateSpellLoader(false);
	SKSE::GetMessagingInterface()->RegisterListener(Listener);
	if (auto serialization = SKSE::GetSerializationInterface())
	{
		serialization->SetUniqueID(Serialisation::ID);
		serialization->SetSaveCallback(&Serialisation::SaveCallback);
		serialization->SetLoadCallback(&Serialisation::LoadCallback);
		serialization->SetRevertCallback(&Serialisation::RevertCallback);
	}
	return true;
}
