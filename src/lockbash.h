#pragma once

namespace Lockbash {
    static bool FinalizeUnlock(RE::TESObjectREFR* a_ref)
    {
        static REL::Relocation<decltype(&FinalizeUnlock)> func{ RELOCATION_ID(19110, 19512) };
        return func(a_ref);
    }

    struct OnHitEventHandler : REX::Singleton<OnHitEventHandler>
    {
        void ProcessLockHit(const RE::TESHitEvent* a_event);

    private:       
        float GetBashLevel(RE::Actor* actor);
        void ProcessHit(RE::REFR_LOCK* lock, RE::TESObjectREFR* locked, int alarm_value, float xp_gain) noexcept;
        float CalculateCrimeGold(RE::Actor* witness);
        void CheckCrime(RE::TESObjectREFR* unlockedObj, float radius, RE::Actor* attacker);
        RE::TESForm* GetOwnerFactionDoor(RE::TESObjectREFR* object);
        bool GenerateNotificationAndSound(RE::LOCK_LEVEL lvl, RE::Actor* actor);
        bool CanBashLevelBreak(RE::LOCK_LEVEL lvl, RE::Actor* actor);
        inline static const char* break_sound_to_use;
        inline static std::string break_message_success;
        inline static std::string break_message_fail;
        inline static std::string actor_name;
        inline static std::string full_message;

    };
}
