#pragma once

#include "../Colliders/BoxCollider.h"
#include "../Component.h"
#include "../Updateable.h"
#include "Serialization/SerializationUtility.h"
#include "Audio/AudioManager.h"
#if EDITOR
#include "imgui.h"
#endif

namespace Engine
{

    class Vacuum : public Engine::Updateable
    {
    private:
        int volume = 0;
        int maxVolume = 10;

        int size = 2 * 3;
        float centerSize = 1.5f * 3;
        std::vector<Engine::Entity*> items;
        Engine::SphereCollider* collider;

        bool isSuccing = false;
        bool isShooting = false;

        int shootForce = 100;

        bool wasShootingKeyPressed = false;
        bool isShootingKeyPressed = false;
        float shootKeyHoldStartTime = 0.0f;
        float lastShootTime = 0.0f;
        const float shootCooldown = 0.3f; // 300 ms
        float lastSuckToggleTime = 0.0f;

        std::shared_ptr<ma_sound> blowingSound;
        std::shared_ptr<ma_sound> suckingSound;

        struct GravityDisableTask
        {
            Engine::Entity* entity;
            float disableTime;
        };

        struct PendingSlimeIntake
        {
            Engine::Entity* entity;
            int totalSize;
            int currentAbsorbed;
            float nextAbsorbTime;
        };

        std::vector<PendingSlimeIntake> activeSlimeAbsorptions;
        std::vector<GravityDisableTask> pendingGravityDisables;

    public:
        Vacuum() = default;

        ~Vacuum() override = default;

        int GetVolume() const { return volume; }
        void SetVolume(int newVolume) { volume = newVolume; }
        int GetMaxVolume() const { return maxVolume; }
        void SetMaxVolume(int newMaxVolume) { maxVolume = newMaxVolume; }

        void Start() override;

        void Update(float deltaTime) override;

        void Shoot();

        void UpdateFillIndicator();

        SERIALIZATION_EXPORT_CLASS(Vacuum);
#if EDITOR
        void DrawImGui() override;
#endif
    };
} // namespace Engine
