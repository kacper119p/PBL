#include "BookManager.h"
#include <algorithm>
#include <random>
#include "Engine/Engine.h"

namespace Engine
{
    void BookManager::Start()
    {
        Collider* collider = Component::GetOwner()->GetComponent<Collider>();
        if (collider)
        {
            collider->OnCollisionAddListener(BookCollision);
        }

        GetUnoccupiedPlace();
    }

    rapidjson::Value BookManager::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(bookWidth)
        SERIALIZE_FIELD(bookDepth)
        SERIALIZE_FIELD(bookMargin)
        SERIALIZE_FIELD(sidePadding)
        SERIALIZE_FIELD(backPadding)
        END_COMPONENT_SERIALIZATION
    }

    void BookManager::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(bookWidth)
        DESERIALIZE_VALUE(bookDepth)
        DESERIALIZE_VALUE(bookMargin)
        DESERIALIZE_VALUE(sidePadding)
        DESERIALIZE_VALUE(backPadding)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void BookManager::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

    void BookManager::OnDestroy()
    {

    }

    void BookManager::Update(float dt)
    {
        for (auto it = activeBookMoves.begin(); it != activeBookMoves.end();)
        {
            it->time += dt;
            float t = glm::clamp(it->time / it->duration, 0.0f, 1.0f);
            glm::vec3 interpolated = glm::mix(it->start, it->end, t);
            it->book->GetTransform()->SetPosition(interpolated);

            if (t >= 1.0f)
            {
                it = activeBookMoves.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void BookManager::GetUnoccupiedPlace()
    {
        unoccupiedPlaces.clear();

        Entity* shelfEntity = Component::GetOwner();

        for (Transform* shelf : shelfEntity->GetTransform()->GetChildren())
        {
            if (!shelf->GetOwner()->GetComponent<Collider>())
                continue;

            Collider* shelfCollider = shelf->GetOwner()->GetComponent<Collider>();
            glm::vec3 shelfCenter = shelf->GetOwner()->GetTransform()->GetPosition();
            glm::vec3 shelfSize = shelfCollider->GetBoundingBox();

            float availableWidth = shelfSize.x - 2.0f * sidePadding;
            int countX = static_cast<int>((availableWidth + bookMargin) / (bookWidth + bookMargin));

            for (int i = 0; i < countX; ++i)
            {
                glm::vec3 pos = shelfCenter;

                float startX = shelfCenter.x - (shelfSize.x * 0.5f) + sidePadding + (bookWidth * 0.5f);
                pos.x = startX + i * (bookWidth + bookMargin);

                pos.z = shelfCenter.z + (shelfSize.z * 0.5f) - backPadding - (bookDepth * 0.5f);
                pos.y = shelfCenter.y;

                bool occupied = false;
                for (Entity* book : currentBooksBeingPut)
                {
                    glm::vec3 bookPos = book->GetTransform()->GetPosition();
                    if (glm::distance(pos, bookPos) < (bookWidth + bookMargin) * 0.5f)
                    {
                        occupied = true;
                        break;
                    }
                }

                if (!occupied)
                {
                    unoccupiedPlaces.push_back(pos);
                }
            }
        }
    }

    void BookManager::PutBookOnShelf(Collider* bookCollider)
    {
        if (!bookCollider || unoccupiedPlaces.empty())
            return;

        Entity* bookEntity = bookCollider->GetOwner();

        if (std::find(currentBooksBeingPut.begin(), currentBooksBeingPut.end(), bookEntity) !=
            currentBooksBeingPut.end())
            return;

        if (bookCollider->collisionMask == 0)
            return;

        size_t index = rand() % unoccupiedPlaces.size();
        glm::vec3 targetPos = unoccupiedPlaces[index];
        unoccupiedPlaces.erase(unoccupiedPlaces.begin() + index);

        currentBooksBeingPut.push_back(bookEntity);

        bookCollider->collisionMask = 0;

        BookLerpData lerp;
        lerp.book = bookEntity;
        lerp.start = bookEntity->GetTransform()->GetPosition();
        lerp.end = targetPos;
        lerp.duration = 1.0f;

        activeBookMoves.push_back(lerp);
    }

#if EDITOR
    void BookManager::DrawImGui()
    {
        ImGui::Text("Book Placement Settings:");
        ImGui::DragFloat("Book Width", &bookWidth, 0.01f, 0.01f, 1.0f);
        ImGui::DragFloat("Book Depth", &bookDepth, 0.005f, 0.005f, 0.5f);
        ImGui::DragFloat("Book Margin", &bookMargin, 0.005f, 0.0f, 0.2f);
        ImGui::DragFloat("Side Padding", &sidePadding, 0.005f, 0.0f, 0.2f);
        ImGui::DragFloat("Back Padding", &backPadding, 0.005f, 0.0f, 0.2f);

        ImGui::Separator();
        ImGui::Text("Currently Assigned Books: %zu", currentBooksBeingPut.size());
        for (size_t i = 0; i < currentBooksBeingPut.size(); ++i)
        {
            Entity* book = currentBooksBeingPut[i];
            ImGui::Text("Book %zu: %s", i, book ? book->GetName().c_str() : "<null>");
        }

        // Debug info
        ImGui::Text("Available Spots: %zu", unoccupiedPlaces.size());
    }
#endif

} // namespace Engine
