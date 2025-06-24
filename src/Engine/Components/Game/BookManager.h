#pragma once
#include <vector>
#include "../Component.h"
#include "Engine/Components/Colliders/BoxCollider.h"
#include "Engine/Components/Colliders/CapsuleCollider.h"
#include "Engine/Components/Colliders/Collider.h"
#include "Engine/Components/Colliders/SphereCollider.h"
#include "Engine/Components/Game/ThrashManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Events/TAction.h"
#include "Serialization/SerializationUtility.h"
#include "glm/glm.hpp"
#include "Engine\Components\Interfaces\IUpdateable.h"
#include "../Updateable.h"
#if EDITOR
#include "imgui.h"
#endif

namespace Engine
{
    enum class ItemType
    {
        Book = 0,
        Coin = 1,
        Weapon = 2
    };

    struct BookLerpData
    {
        Entity* book = nullptr;
        glm::vec3 start;
        glm::vec3 end;
        glm::quat startRotation;
        glm::quat endRotation;
        float time = 0.0f;
        float duration = .3f;
    };

    class BookManager : public Updateable
    {
    private:
        float bookWidth = 0.2f;
        float bookDepth = 0.05f;
        float bookHeight = 0.3f;
        float bookMargin = 0.02f;
        float sidePadding = 0.05f;
        float backPadding = 0.03f;
        std::vector<glm::vec3> unoccupiedPlaces = {};
        std::vector<Entity*> currentBooksBeingPut = {};
        std::vector<BookLerpData> activeBookMoves = {};
        Transform* swordPlacementPoint = nullptr;
        Transform* bowPlacementPoint = nullptr;
        Transform* arrowPlacementPoint = nullptr;
        Transform* shieldPlacementPoint = nullptr;
        ItemType itemType = ItemType::Book;
        Events::TAction<Collider*> BookCollision = Events::TAction<Collider*>(this, &BookManager::PutItem);

    public:
        BookManager() = default;
        ~BookManager() = default;

        void Start() override;
        void OnDestroy() override;
        void Update(float dt) override;

        void GetUnoccupiedPlace();
        void PutItem(Collider* bookCollider);
        

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_CLASS(BookManager);
    };
} // namespace Engine
