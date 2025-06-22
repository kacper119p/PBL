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

#if EDITOR
#include "imgui.h"
#endif

namespace Engine
{
    struct BookLerpData
    {
        Entity* book = nullptr;
        glm::vec3 start;
        glm::vec3 end;
        float time = 0.0f;
        float duration = 1.0f;
    };

    class BookManager : public Component, public IUpdateable
    {
    private:
        float bookWidth = 0.2f;
        float bookDepth = 0.05f;
        float bookMargin = 0.02f;
        float sidePadding = 0.05f;
        float backPadding = 0.03f;
        std::vector<glm::vec3> unoccupiedPlaces = {};
        std::vector<Entity*> currentBooksBeingPut = {};
        std::vector<BookLerpData> activeBookMoves = {};

        Events::TAction<Collider*> BookCollision = Events::TAction<Collider*>(this, &BookManager::PutBookOnShelf);

    public:
        BookManager() = default;
        ~BookManager() = default;

        void Start() override;
        void OnDestroy() override;
        void Update(float dt) override;

        void GetUnoccupiedPlace();
        void PutBookOnShelf(Collider* bookCollider);

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_CLASS(BookManager);
    };
} // namespace Engine
