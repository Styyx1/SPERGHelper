#pragma once

namespace Lockbash {
    static bool FinalizeUnlock(RE::TESObjectREFR* a_ref)
    {
        static REL::Relocation<decltype(&FinalizeUnlock)> func{ RELOCATION_ID(19110, 19512) };
        return func(a_ref);
    }

    using RES = RE::BSEventNotifyControl;
    struct OnHitEventHandler : REX::Singleton<OnHitEventHandler>, public RE::BSTEventSink<RE::TESHitEvent>
    {
        void RegisterLockBash() {
            const auto manager{ RE::ScriptEventSourceHolder::GetSingleton() };
            manager->AddEventSink(this);
            REX::INFO("Registered {} handler", typeid(RE::TESHitEvent).name());
        }
    private:

        RES ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) noexcept override;
        float GetBashLevel(RE::Actor* actor);
        static void ProcessHit(RE::REFR_LOCK* lock, RE::TESObjectREFR* locked, int alarm_value, float xp_gain) noexcept;
        static void CalculateCrimeGold(RE::Actor* witness);
        bool GenerateNotificationAndSound(RE::LOCK_LEVEL lvl, RE::Actor* actor);
        inline static const char* break_sound_to_use;
        inline static std::string break_message_success;
        inline static std::string break_message_fail;
        inline static std::string actor_name;
        inline static std::string full_message;

    };
}
