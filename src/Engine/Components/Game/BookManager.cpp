#include "BookManager.h"
#include <algorithm>
#include <random>
#include "Engine/Components/Physics/Rigidbody.h"
#include "Engine/Components/Game/Thrash.h"
#include "Engine/Engine.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/Components/Game/ThrashManager.h"
#include <iostream>
#if EDITOR
#include "ImGuizmo.h"
#endif
namespace Engine
{
    float RandomRange(float min, float max)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    void BookManager::Start()
    {
        UpdateManager::GetInstance()->RegisterComponent(this);
        if (Collider* collider = Component::GetOwner()->GetComponent<Collider>())
        {
            collider->OnCollisionAddListener(BookCollision);
        }
        if (itemType == ItemType::Weapon)
        {
            auto children = Component::GetOwner()->GetTransform()->GetChildren();
            swordPlacementPoint = children[0];
            bowPlacementPoint = children[1];
            arrowPlacementPoint = children[2];
            shieldPlacementPoint = children[3];
        }
        GetUnoccupiedPlace();
    }

    rapidjson::Value BookManager::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(bookWidth)
        SERIALIZE_FIELD(bookDepth)
        SERIALIZE_FIELD(bookHeight)
        SERIALIZE_FIELD(bookMargin)
        SERIALIZE_FIELD(sidePadding)
        SERIALIZE_FIELD(backPadding)
        SERIALIZE_FIELD(itemType)
        SERIALIZE_FIELD(swordPlacementPoint)
        SERIALIZE_FIELD(bowPlacementPoint)
        SERIALIZE_FIELD(arrowPlacementPoint)
        SERIALIZE_FIELD(shieldPlacementPoint)
        END_COMPONENT_SERIALIZATION
    }

    void BookManager::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(bookWidth)
        DESERIALIZE_VALUE(bookDepth)
        DESERIALIZE_VALUE(bookHeight)
        DESERIALIZE_VALUE(bookMargin)
        DESERIALIZE_VALUE(sidePadding)
        DESERIALIZE_VALUE(backPadding)
        DESERIALIZE_VALUE(itemType)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void BookManager::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        DESERIALIZE_POINTER(swordPlacementPoint)
        DESERIALIZE_POINTER(bowPlacementPoint)
        DESERIALIZE_POINTER(arrowPlacementPoint)
        DESERIALIZE_POINTER(shieldPlacementPoint)
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

    void BookManager::OnDestroy() { UpdateManager::GetInstance()->UnregisterComponent(this); }

    void BookManager::Update(float dt)
    {
        for (auto it = activeBookMoves.begin(); it != activeBookMoves.end();)
        {
            it->time += dt;
            float t = glm::clamp(it->time / it->duration, 0.0f, 1.0f);

            glm::vec3 pos = glm::mix(it->start, it->end, t);
            glm::quat rot = glm::slerp(it->startRotation, it->endRotation, t);

            it->book->GetTransform()->SetPosition(pos);
            it->book->GetTransform()->SetRotation(glm::eulerAngles(rot));

            if (t >= 1.5f)
            {
                it = activeBookMoves.erase(it);
                if (itemType == ItemType::Book)
                    ThrashManager::GetInstance()->AddCleanedUpBook(it->book);
                else if (itemType == ItemType::Coin)
                    ThrashManager::GetInstance()->AddCleanedUpCoin(it->book);
                else if (itemType == ItemType::Weapon)
                    ThrashManager::GetInstance()->AddCleanedUpWeapon(it->book);
            }
            else
                ++it;
        }
    }

    void BookManager::GetUnoccupiedPlace()
    {
        unoccupiedPlaces.clear();
        Entity* shelfEntity = Component::GetOwner();

        for (Transform* shelf : shelfEntity->GetTransform()->GetChildren())
        {
            Collider* shelfCollider = shelf->GetOwner()->GetComponent<Collider>();
            if (!shelfCollider)
                continue;

            glm::vec3 shelfCenter = shelf->GetOwner()->GetTransform()->GetPosition();
            glm::vec3 shelfSize = shelfCollider->GetBoundingBox();

            float availableWidth = shelfSize.x;
            int countX = static_cast<int>((availableWidth + bookMargin) / (bookWidth + bookMargin));
            float startX = shelfCenter.x - (availableWidth * 0.5f) + (bookWidth * 0.5f);

            for (int i = 0; i < countX; ++i)
            {
                glm::vec3 pos(startX + i * (bookWidth + bookMargin),
                              shelfCenter.y + (shelfSize.y * 0.5f) + (bookHeight * 0.5f), shelfCenter.z);

                bool occupied = false;
                for (Entity* book : currentBooksBeingPut)
                {
                    if (glm::distance(pos, book->GetTransform()->GetPosition()) < (bookWidth + bookMargin) * 0.5f)
                    {
                        occupied = true;
                        break;
                    }
                }

                if (!occupied)
                    unoccupiedPlaces.push_back(pos);
            }
        }
    }

    void BookManager::PutItem(Collider* itemCollider)
    {
        if (!itemCollider)
            return;
        if (!itemCollider->GetOwner()->GetComponent<Thrash>())
        {
            return;
        }
        Entity* item = itemCollider->GetOwner();
        if (std::find(currentBooksBeingPut.begin(), currentBooksBeingPut.end(), item) != currentBooksBeingPut.end())
            return;

        if (itemCollider->collisionMask == 0)
            return;

        currentBooksBeingPut.push_back(item);
        std::string name = item->GetName();
        glm::vec3 start = item->GetTransform()->GetPosition();
        glm::quat startRot = item->GetTransform()->GetRotation();
        glm::vec3 end = glm::vec3(0.0f);
        glm::quat endRot = glm::quat(glm::vec3(0.0f));

        if (itemType == ItemType::Book && name.find("SmallBook") != std::string::npos)
        {
            if (unoccupiedPlaces.empty())
                return;

            if (item->GetComponent<Rigidbody>())
            {
                item->RemoveComponent<Rigidbody>();
            }

            size_t index = rand() % unoccupiedPlaces.size();
            end = unoccupiedPlaces[index];
            unoccupiedPlaces.erase(unoccupiedPlaces.begin() + index);

            endRot = glm::quat(glm::radians((rand() % 2 == 0) ? glm::vec3(0, -90, 0) : glm::vec3(0, 90, 0)));
        }
        else if (itemType == ItemType::Coin &&
                 (name.find("Coin") != std::string::npos || name.find("rubin") != std::string::npos ||
                  name.find("szafir") != std::string::npos || name.find("szmaragd") != std::string::npos))
        {
            if (item->GetComponent<Rigidbody>())
            {
                item->RemoveComponent<Rigidbody>();
            }
            end = this->GetOwner()->GetTransform()->GetPosition() +
                          glm::vec3(RandomRange(-0.5f, 0.5f), RandomRange(0.0f, 0.2f), RandomRange(-0.5f, 0.5f));
            
        }
        else if (itemType == ItemType::Weapon)
        {
            std::string name = item->GetName();
            if (name.find("Sword") != std::string::npos)
            {
                if (item->GetComponent<Rigidbody>())
                {
                    item->RemoveComponent<Rigidbody>();
                }
                endRot = swordPlacementPoint->GetRotation();
                end = swordPlacementPoint->GetPosition();
                std::cout << std::endl << "Sword found" << std::endl;    
            }
            else if (name.find("Bow") != std::string::npos)
            {
                if (item->GetComponent<Rigidbody>())
                {
                    item->RemoveComponent<Rigidbody>();
                }
                endRot = bowPlacementPoint->GetRotation();
                end = bowPlacementPoint->GetPosition();
            }
            else if (name.find("Arrow") != std::string::npos)
            {
                if (item->GetComponent<Rigidbody>())
                {
                    item->RemoveComponent<Rigidbody>();
                }
                endRot = arrowPlacementPoint->GetRotation();
                end = arrowPlacementPoint->GetPosition();
            }
            else if (name.find("Shield") != std::string::npos)
            {
                if (item->GetComponent<Rigidbody>())
                {
                    item->RemoveComponent<Rigidbody>();
                }
                endRot = shieldPlacementPoint->GetRotation();
                end = shieldPlacementPoint->GetPosition();
            }
            else
                return; // Unrecognized
        }

        BookLerpData lerp;
        lerp.book = item;
        lerp.start = start;
        lerp.end = end;
        lerp.startRotation = startRot;
        lerp.endRotation = endRot;
        lerp.duration = 1.0f;

        activeBookMoves.push_back(lerp);
    }

    #if EDITOR
    void BookManager::DrawImGui()
    {
        const char* itemTypeNames[] = {"Book", "Coin", "Weapon"};
        int selectedType = static_cast<int>(itemType);
        if (ImGui::Combo("Item Type", &selectedType, itemTypeNames, IM_ARRAYSIZE(itemTypeNames)))
        {
            itemType = static_cast<ItemType>(selectedType);
        }

        if (itemType == ItemType::Book)
        {
            ImGui::DragFloat("Book Width", &bookWidth, 0.01f, 0.01f, 1.0f);
            ImGui::DragFloat("Book Depth", &bookDepth, 0.005f, 0.005f, 0.5f);
            ImGui::DragFloat("Book Height", &bookHeight, 0.01f, 0.01f, 1.0f);
            ImGui::DragFloat("Book Margin", &bookMargin, 0.005f, 0.0f, 0.2f);
            ImGui::DragFloat("Side Padding", &sidePadding, 0.005f, 0.0f, 0.2f);
            ImGui::DragFloat("Back Padding", &backPadding, 0.005f, 0.0f, 0.2f);
        }
        if (itemType == ItemType::Weapon)
        {
            auto children = Component::GetOwner()->GetTransform()->GetChildren();
            swordPlacementPoint = children[0];
            bowPlacementPoint = children[1];
            arrowPlacementPoint = children[2];
            shieldPlacementPoint = children[3];

            auto drawTransformControls = [](Transform* entity, const char* label)
            {
                if (!entity)
                {
                    ImGui::Text("%s: <no transform>", label);
                    return;
                }

                glm::vec3 pos = entity->GetPosition();
                glm::vec3 rotEuler = glm::degrees(glm::eulerAngles(entity->GetRotation()));

                if (ImGui::DragFloat3((std::string(label) + " Position").c_str(), &pos.x, 0.01f))
                    entity->SetPosition(pos);

                if (ImGui::DragFloat3((std::string(label) + " Rotation (Euler)").c_str(), &rotEuler.x, 0.5f))
                    entity->SetRotation(glm::quat(glm::radians(rotEuler)));
            };
            ImGui::NewLine();
            drawTransformControls(swordPlacementPoint, "Sword Point");
            ImGui::NewLine();
            drawTransformControls(bowPlacementPoint, "Bow Point");
            ImGui::NewLine();
            drawTransformControls(arrowPlacementPoint, "Arrow Point");
            ImGui::NewLine();
            drawTransformControls(shieldPlacementPoint, "Shield Point");
        }

        ImGui::Separator();
        ImGui::Text("Assigned Items: %zu", currentBooksBeingPut.size());
        for (size_t i = 0; i < currentBooksBeingPut.size(); ++i)
        {
            Entity* book = currentBooksBeingPut[i];
            ImGui::Text("Item %zu: %s", i, book ? book->GetName().c_str() : "<null>");
        }

        ImGui::Text("Available Spots: %zu", unoccupiedPlaces.size());
    }
    #endif
} // namespace Engine
