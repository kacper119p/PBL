#include "DefaultPlayer.h"
#include "Engine/Prefabs/PrefabLoader.h"
#include "spdlog/spdlog.h"


namespace Engine
{
    void DefaultPlayer::Start()
    {
        if (!PrefabPath.empty())
        {
            //loading model from prefab
            Entity* prefabEntity = PrefabLoader::LoadPrefab(PrefabPath, this->GetScene(), this->GetScene()->GetRoot()->GetTransform());
            if (prefabEntity)
            {
                this->GetTransform()->AddChild(prefabEntity->GetTransform());
            }
            rb = this->AddComponent<Engine::Rigidbody>();
            movementComponent = this->AddComponent<Engine::MovementComponent>();
            boxCollider = this->AddComponent<Engine::BoxCollider>();
            //collider settings
            boxCollider->SetHeight(2.0f);
            boxCollider->SetWidth(2.0f);
            boxCollider->SetDepth(2.0f);
            //rb settings
            rb->friction = 0.1f;
            rb->angularDamping = 0.01f;
            rb->linearDamping = 0.01f;
            rb->restitution = 0.3f;
            rb->SetMass(1.0f);
            rb->frictionEnabled = true;
            //player start position
            this->GetTransform()->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        }
        else
        {
            spdlog::warn("DefaultPlayer: Prefab path is empty, cannot load player model.");
        }
    }

    void DefaultPlayer::Update()
    {
    }
}
