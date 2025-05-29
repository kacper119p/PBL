#pragma once
#include "Engine/Components/Component.h"
#include "Engine/EngineObjects/Entity.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/Components/Colliders/Collider.h"

class ThrashCan : public Engine::Component
{
    Engine::Collider* collider = nullptr;

public:
    ThrashCan() = default;
    ~ThrashCan() override = default;



    void Start() override 
    {
        collider = this->GetOwner()->GetComponent<Engine::Collider>();
        collider->OnCollisionAddListener()
    }
    void OnDestroy() override {}


};
