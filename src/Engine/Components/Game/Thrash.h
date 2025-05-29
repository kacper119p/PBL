#pragma once

#include "../Component.h"
#include "Engine/EngineObjects/Entity.h"
#include "Serialization/SerializationUtility.h"
#include "Events/TAction.h"
#include "Engine/Components/Colliders/Collider.h"
#if EDITOR
#include "imgui.h"
#endif

enum ThrashSize
{
    Small = 1,
    Medium = 5,
    Large = 10
};

class Thrash : public Engine::Component
{
    ThrashSize size;
    Engine::Collider* collider = nullptr;
    Events::TAction<Engine::Collider*> ThrowOut;

public:
    Thrash() = default;
    ~Thrash() override = default;
    ThrashSize GetSize() const { return size; }
    void SetSize(ThrashSize newSize) { size = newSize; }
    void Start() override
    {
        collider = this->GetOwner()->GetComponent<Engine::Collider>();
        ThrowOut = Events::TAction<Engine::Collider*>(this, &Thrash::DeleteThrash);
    }
    void OnDestroy() override {}

    void DeleteThrash(Engine::Collider* collider);

    SERIALIZATION_EXPORT_CLASS(Thrash);

    #if EDITOR
    void DrawImGui() override;
#endif
};
