#pragma once

#include "../Colliders/SphereCollider.h"
#include "../Colliders/BoxCollider.h"
#include "../Component.h"
#include "../Updateable.h"
#include "Serialization/SerializationUtility.h"
#if EDITOR
#include "imgui.h"
#endif


class Vacuum : public Engine::Updateable
{
private:
    int volume = 0;
    int maxVolume = 10;

    int size = 5;
    int centerSize = 1;
    std::vector<Engine::Entity*> items;
    Engine::SphereCollider* collider;

    bool isSuccing = false;
    bool isShooting = false;

    int shootForce = 100;

public:
    Vacuum() = default;
    ~Vacuum() override = default;
    int GetVolume() const { return volume; }
    void SetVolume(int newVolume) { volume = newVolume; }
    int GetMaxVolume() const { return maxVolume; }
    void SetMaxVolume(int newMaxVolume) { maxVolume = newMaxVolume; }
    void Start() override;
    void Update(float deltaTime) override;

    SERIALIZATION_EXPORT_CLASS(Vacuum);
#if EDITOR
    void DrawImGui() override;
#endif
};
