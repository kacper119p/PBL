#pragma once

#include "../Component.h"
#include "Serialization/SerializationUtility.h"
#include "../Updateable.h"
#include "../Colliders/SphereCollider.h"
#if EDITOR
#include "imgui.h"
#endif


class Vacuum : public Engine::Updateable
{
private:
    int volume = 0;
    int maxVolume = 10;

    int size = 5;

public:
    Vacuum() = default;
    ~Vacuum() override = default;
    int GetVolume() const { return volume; }
    void SetVolume(int newVolume) { volume = newVolume; }
    int GetMaxVolume() const { return maxVolume; }
    void SetMaxVolume(int newMaxVolume) { maxVolume = newMaxVolume; }
    void Update(float deltaTime) override;
    
    Engine::SphereCollider* collider = ;

    #if EDITOR
    void DrawImGui() override;
    #endif
};
