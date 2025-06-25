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
        Book = 2,
        Coin = 3,
        Arrow = 4,
        Medium = 5,
        Bow = 6,
        Sword = 10,
        Shield = 19,
        Large = 20
    };

    class Thrash : public Component
    {
        ThrashSize size = ThrashSize::Small;
        Collider* collider = nullptr;
        Events::TAction<Collider*> ThrowOut = Events::TAction<Collider*>(this, &Thrash::DeleteThrash);

    public:
        Thrash() = default;
        ~Thrash() override = default;
        ThrashSize GetSize() const 
        { 
            return (size == ThrashSize::Coin) ? ThrashSize::Small : size; 
        }
        void SetSize(ThrashSize newSize) { size = newSize; }
        void Start() override;
        
        void OnDestroy() override 
        {
            if (size == ThrashSize::Coin)
            {
                ThrashManager::GetInstance()->RemoveCoin(GetOwner());
                ThrashManager::GetInstance()->RemoveCleanedUpCoin(GetOwner());
            }
            else if (size == ThrashSize::Book)
            {
                ThrashManager::GetInstance()->RemoveBook(GetOwner());
                ThrashManager::GetInstance()->RemoveCleanedUpBook(GetOwner());
            }
            else if (size == ThrashSize::Arrow || size == ThrashSize::Bow || size == ThrashSize::Shield || size == ThrashSize::Sword)
            {
                ThrashManager::GetInstance()->RemoveWeapon(GetOwner());
                ThrashManager::GetInstance()->RemoveCleanedUpWeapon(GetOwner());
            }
            else
            {
                ThrashManager::GetInstance()->RemoveThrash(this);
            }
        }

        void DeleteThrash(Collider* collider); 


        SERIALIZATION_EXPORT_CLASS(Thrash);

#if EDITOR
        void DrawImGui() override;
#endif
    };
} // namespace Engine
