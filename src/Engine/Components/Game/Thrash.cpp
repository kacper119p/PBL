#include "Thrash.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include <iostream>

namespace Engine
{
    
    void Thrash::Start() { 
        collider = GetOwner()->GetComponent<Collider>();
        collider->OnCollisionAddListener(ThrowOut);
        if (size == ThrashSize::Coin)
        {
            ThrashManager::GetInstance()->AddCoin(GetOwner());
        }
        else if (size == ThrashSize::Book)
        {
            ThrashManager::GetInstance()->AddBook(GetOwner());
        }
        else if (size == ThrashSize::Arrow || size == ThrashSize::Bow || size == ThrashSize::Sword || size == ThrashSize::Shield)
        {
            ThrashManager::GetInstance()->AddWeapon(GetOwner());
        }
        else
        {
            collider->OnTriggerAddListener(ThrowOut);
            ThrashManager::GetInstance()->AddThrash(this);
        }
    }

    rapidjson::Value Thrash::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(size)
        END_COMPONENT_SERIALIZATION
    }

    void Thrash::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(size)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void Thrash::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

    void Thrash::DeleteThrash(Engine::Collider* collider)
    {
        if (collider->GetOwner()->GetName() == "ThrashCan")
        {
            if (this->collider)
            {
                std::cout << "Thrash: Removing ThrowOut listener from collider." << std::endl;
                this->collider->OnTriggerRemoveListener(ThrowOut);
                if (size == ThrashSize::Coin)
                {
                    ThrashManager::GetInstance()->SetCoinTaskFailed(true);
                }
                else if (size == ThrashSize::Book)
                {
                    ThrashManager::GetInstance()->SetBookTaskFailed(true);
                }
                else if (size == ThrashSize::Arrow || size == ThrashSize::Bow || size == ThrashSize::Shield ||
                         size == ThrashSize::Sword)
                {
                    ThrashManager::GetInstance()->SetWeaponTaskFailed(true);
                }
            }
            GetOwner()->Destroy();
        }
    }

#if EDITOR
    void Thrash::DrawImGui()
    {
        static const char* sizeLabels[] = {"Small", "Medium", "Large", "Coin", "Book", "Arrow", "Bow", "Sword", "Shield"};
        int currentSizeIndex = 0;

        switch (size)
        {
            case ThrashSize::Small:
                currentSizeIndex = 0;
                break;
            case ThrashSize::Medium:
                currentSizeIndex = 1;
                break;
            case ThrashSize::Large:
                currentSizeIndex = 2;
                break;
            case ThrashSize::Coin:
                currentSizeIndex = 3;
                break;
            case ThrashSize::Book:
                currentSizeIndex = 4;
                break;
            case ThrashSize::Arrow:
                currentSizeIndex = 5;
                break;
            case ThrashSize::Bow:
                currentSizeIndex = 6;
                break;
            case ThrashSize::Sword:
                currentSizeIndex = 7;
                break;
            case ThrashSize::Shield:
                currentSizeIndex = 8;
                break;
        }

        if (ImGui::Combo("Size", &currentSizeIndex, sizeLabels, IM_ARRAYSIZE(sizeLabels)))
        {
            switch (currentSizeIndex)
            {
                case 0:
                    size = ThrashSize::Small;
                    break;
                case 1:
                    size = ThrashSize::Medium;
                    break;
                case 2:
                    size = ThrashSize::Large;
                    break;
                case 3:
                    size = ThrashSize::Coin;
                    break;
                case 4:
                    size = ThrashSize::Book;
                    break;
                case 5:
                    size = ThrashSize::Arrow;
                    break;
                case 6:
                    size = ThrashSize::Bow;
                    break;
                case 7:
                    size = ThrashSize::Sword;
                    break;
                case 8:
                    size = ThrashSize::Shield;
                    break;
            }
        }
    }
#endif
} // namespace Engine
