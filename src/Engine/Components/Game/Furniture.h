#pragma once

#include "../Component.h"
#include "Engine/EngineObjects/Entity.h"
#include "Serialization/SerializationUtility.h"
#include "Events/TAction.h"
#include "Engine/Components/Colliders/Collider.h"
#include "Engine/Components/Colliders/BoxCollider.h"
#include "Engine/Components/Colliders/SphereCollider.h"
#include "Engine/Components/Colliders/CapsuleCollider.h"

namespace Engine
{
    class Furniture : public Component
    {
        Collider* collider = nullptr;
        Events::TAction<Collider*> ThrowOut = Events::TAction<Collider*>(this, &Furniture::DeleteFurniture);

        public:
        Furniture() = default;
        ~Furniture() override = default;
        void Start() override;
        void OnDestroy() override;

        void DeleteFurniture(Collider* collider);

        SERIALIZATION_EXPORT_CLASS(Furniture);

        #if EDITOR
        void DrawImGui() override;
        #endif
    };
} // namespace Engine
