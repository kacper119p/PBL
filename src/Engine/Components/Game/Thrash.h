#pragma once

#include "../Component.h"
#include "Engine/Components/Colliders/BoxCollider.h"
#include "Engine/Components/Colliders/CapsuleCollider.h"
#include "Engine/Components/Colliders/Collider.h"
#include "Engine/Components/Colliders/SphereCollider.h"
#include "Engine/Components/Game/ThrashManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Events/TAction.h"
#include "Serialization/SerializationUtility.h"
#if EDITOR
#include "imgui.h"
#endif

namespace Engine
{

    enum ThrashSize
    {
        Small = 1,
        Medium = 5,
        Large = 10
    };

    class Thrash : public Component
    {
        ThrashSize size = ThrashSize::Small;
        Collider* collider = nullptr;
        Events::TAction<Collider*> ThrowOut = Events::TAction<Collider*>(this, &Thrash::DeleteThrash);

    public:
        Thrash() = default;
        ~Thrash() override = default;
        ThrashSize GetSize() const { return size; }
        void SetSize(ThrashSize newSize) { size = newSize; }
        void Start() override;
        
        void OnDestroy() override 
        {
            ThrashManager::GetInstance()->RemoveThrash(this);
        }

        void DeleteThrash(Collider* collider);

        SERIALIZATION_EXPORT_CLASS(Thrash);

#if EDITOR
        void DrawImGui() override;
#endif
    };
} // namespace Engine
