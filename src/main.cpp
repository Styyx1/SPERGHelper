#include "config.h"
#include "unarmed.h"
void Listener(SKSE::MessagingInterface::Message* a_msg) {
	using msg = SKSE::MessagingInterface;
	switch (a_msg->type)
	{
	case msg::kDataLoaded:
		Unarmed::Install();
	}
}

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	Config::Settings::GetSingleton()->LoadSettings();
	SKSE::GetMessagingInterface()->RegisterListener(Listener);

	return true;
}
