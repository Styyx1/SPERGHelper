#include "config.h"
#include "unarmed.h"
#include "lockbash.h"

void Listener(SKSE::MessagingInterface::Message* a_msg) {
	using msg = SKSE::MessagingInterface;
	switch (a_msg->type)
	{
	case msg::kDataLoaded:
		Unarmed::Install();
		Lockbash::OnHitEventHandler::GetSingleton()->RegisterLockBash();
	}
}

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse,{.trampoline = true});

	Config::Settings::GetSingleton()->UpdateSettings(false);
	SKSE::GetMessagingInterface()->RegisterListener(Listener);

	return true;
}
