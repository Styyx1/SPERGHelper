#include "events.h"
#include "unarmed.h"
#include "lockbash.h"

namespace Events {
	EventResult HitEvent::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>*) {
		if (!a_event)
			return EventResult::kContinue;

		Unarmed::HitEvent::GetSingleton()->ProcessUnarmed(a_event);
		Lockbash::OnHitEventHandler::GetSingleton()->ProcessLockHit(a_event);


		return EventResult::kContinue;
	}
}