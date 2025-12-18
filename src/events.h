#pragma once
namespace Events {
	using EventResult = RE::BSEventNotifyControl;
	struct HitEvent : public REX::Singleton<HitEvent>, public RE::BSTEventSink<RE::TESHitEvent> {

		void Register() {
			const auto manager{ RE::ScriptEventSourceHolder::GetSingleton() };
			manager->AddEventSink(this);
			REX::INFO("Registered for {}", typeid(RE::TESHitEvent).name());
		};
		void ProcessUnarmed(RE::TESHitEvent* event);
		EventResult ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) override;

	};
}
