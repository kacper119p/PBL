#include "AiManager.h"
#include "Sequence.h"
#include "Selector.h"
#include "LeafNodes.h"
#include "NavMesh.h"
#include "Engine/Components/Game/Thrash.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Serialization/SerializationUtility.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    AiManager::AiManager()
    {
        UpdateManager::GetInstance()->RegisterComponent(this);
        InitializeAStar();
        InitializeBehaviorTree();
    }

    AiManager::~AiManager()
    {
        delete AStarComponent;
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

    void AiManager::InitializeAStar()
    {
        if (!AStarComponent)
        {
            AStarComponent = new AStar();
            AStarComponent->SetGraph(NavMesh::Get().GetGraph());
        }
    }

    void AiManager::InitPlayer()
    {
        auto root = GetOwner()->GetScene()->GetRoot();
        for (auto& child : root->GetTransform()->GetChildren())
        {
            if (child->GetOwner()->GetName() == SelectedPlayerName)
            {
                Player = child->GetOwner();
                return;
            }
        }

        Player = nullptr;
    }

    void AiManager::InitializeBehaviorTree()
    {
        using namespace std;

        auto left = make_shared<Sequence>();
        auto root = make_shared<Selector>();

        auto selector1 = make_shared<Selector>();
        auto sequence0 = make_shared<Sequence>();
        auto sequence1 = make_shared<Sequence>();
        auto sequence2 = make_shared<Sequence>();
        auto selectorX = make_shared<Selector>();
        auto isPlayerInRange = make_shared<IsPlayerInRangeNode>(this);
        auto updateChaseTimer = make_shared<UpdateChaseTimerNode>(this);
        auto restTimer = make_shared<UpdateRestTimerNode>(this);
        auto runFromPlayer = make_shared<RunFromPlayerNode>(this);
        auto stopSequence = make_shared<Sequence>();
        auto isChaseOver = make_shared<IsChaseTimerOverNode>(this);
        auto stopChasing = make_shared<EntityStopNode>(this);
        auto walkSlowly = make_shared<WalkSlowlyNode>(this);
        auto isTrashInRange = make_shared<IsTrashInRangeNode>(this);
        auto walkToTrash = make_shared<WalkToTrashNode>(this);
        auto absorbTrash = make_shared<AbsorbTrashNode>(this);

        selectorX->AddChild(left);
        selectorX->AddChild(walkSlowly);
        left->AddChild(isTrashInRange);
        left->AddChild(walkToTrash);
        left->AddChild(absorbTrash);

        sequence0->AddChild(isPlayerInRange);
        sequence0->AddChild(selector1);
        selector1->AddChild(sequence1);
        selector1->AddChild(sequence2);
        sequence1->AddChild(isChaseOver);
        sequence1->AddChild(restTimer);
        sequence1->AddChild(stopChasing);
        sequence2->AddChild(updateChaseTimer);
        sequence2->AddChild(runFromPlayer);

        root->AddChild(sequence0);
        root->AddChild(selectorX);

        RootBehavior = root;
    }

    bool AiManager::IsPlayerInRange() const
    {
        auto slimePos = GetOwner()->GetTransform()->GetPosition();
        auto playerPos = Player->GetTransform()->GetPosition();
        return glm::distance(slimePos, playerPos) < PlayerRange;
    }

    bool AiManager::IsChaseTimerOver() const
    {
        return GetChaseTimer() > GetChaseCooldown();
    }

    void AiManager::Update(const float DeltaTime)
    {
        if (!Player || !NavMesh::Get().GetGraph())
            return;

        if (RootBehavior)
            RootBehavior->Tick(DeltaTime);

        AStarComponent->SetObjectPosition(GetOwner()->GetTransform()->GetPosition());

        if (NavMesh::Get().IsOnNavMesh(GetOwner()->GetTransform()->GetPosition(), NavMesh::Get().GetSpacing()))
            AStarComponent->UpdateMovement(DeltaTime, GetOwner());
    }

    void AiManager::StartChase()
    {
        IsChasing = true;
        ChaseTimer = 0.0f;
    }

    void AiManager::StopChase()
    {
        IsChasing = false;
        ChaseTimer = 0.0f;
    }

    bool AiManager::IsTrashNearby() const
    {
        glm::vec3 slimePos = GetOwner()->GetTransform()->GetPosition();

        for (const auto& trash : TrashEntities)
        {
            if (!trash)
                continue;

            glm::vec3 trashPos = trash->GetTransform()->GetPosition();
            if (glm::distance(slimePos, trashPos) <= TrashRange)
            {
                return true;
            }
        }

        return false;
    }

    void AiManager::RecalculateCurrentTrash()
    {
        CurrentTrashValue = 0;

        auto children = GetOwner()->GetTransform()->GetChildren();
        for (Transform* child : children)
        {
            Entity* entity = child->GetOwner();
            Thrash* trash = entity->GetComponent<Thrash>();
            if (trash)
            {
                CurrentTrashValue += static_cast<int>(trash->GetSize());
            }
        }
    }

#if EDITOR
    void AiManager::AddEntityRecursivelyToTrash(Entity* entity)
    {
        if (!entity)
            return;

        const auto& children = entity->GetTransform()->GetChildren();

        if (!children.empty())
        {
            for (auto& child : children)
            {
                AddEntityRecursivelyToTrash(child->GetOwner());
            }
        }
        else
        {
            TrashEntities.push_back(entity);
        }
    }

    void AiManager::DrawImGui()
    {
        if (ImGui::CollapsingHeader("AI Manager", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Chasing: %s", IsChasing ? "Yes" : "No");
            ImGui::InputFloat("Chase Timer", &ChaseTimer);

            static std::vector<std::string> entityNames;
            static int selectedIndex = -1;

            entityNames.clear();
            auto root = GetOwner()->GetScene()->GetRoot();
            for (auto& child : root->GetTransform()->GetChildren())
            {
                entityNames.push_back(child->GetOwner()->GetName());
            }

            selectedIndex = -1;
            for (size_t i = 0; i < entityNames.size(); ++i)
            {
                if (entityNames[i] == SelectedPlayerName)
                {
                    selectedIndex = static_cast<int>(i);
                    break;
                }
            }

            if (ImGui::BeginCombo("Select Player", SelectedPlayerName.c_str()))
            {
                for (size_t i = 0; i < entityNames.size(); ++i)
                {
                    bool isSelected = (SelectedPlayerName == entityNames[i]);
                    if (ImGui::Selectable(entityNames[i].c_str(), isSelected))
                    {
                        SelectedPlayerName = entityNames[i];
                        InitPlayer();
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (Player)
            {
                auto pos = Player->GetTransform()->GetPosition();
                ImGui::Text("Player Pos: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
            }

            ImGui::Separator();
            ImGui::Text("Trash Entities");

            static int selectedTrashIndex = -1;
            static std::string selectedTrashName = "";

            for (int i = 0; i < (int) TrashEntities.size(); ++i)
            {
                Entity* entity = TrashEntities[i];
                ImGui::PushID(i);

                ImGui::Text("%d: %s", i, entity->GetName().c_str());
                ImGui::SameLine();
                if (ImGui::Button("Remove"))
                {
                    TrashEntities.erase(TrashEntities.begin() + i);
                    --i;
                }

                ImGui::PopID();
            }

            if (ImGui::BeginCombo("Add Entity", "Select..."))
            {
                for (auto& child : root->GetTransform()->GetChildren())
                {
                    Entity* entity = child->GetOwner();
                    bool alreadyInList = std::find(TrashEntities.begin(), TrashEntities.end(), entity) !=
                                         TrashEntities.end();

                    if (!alreadyInList)
                    {
                        if (ImGui::Selectable(entity->GetName().c_str()))
                        {
                            AddEntityRecursivelyToTrash(entity);
                        }
                    }
                }
                ImGui::EndCombo();
            }
        }

        if (ImGui::CollapsingHeader("A* Pathfinding", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float moveSpeed = AStarComponent->GetMoveSpeed();
            if (ImGui::InputFloat("Move Speed", &moveSpeed))
            {
                AStarComponent->SetMoveSpeed(moveSpeed);
            }

            if (ImGui::Button("Bake NavMesh"))
            {
                AStarComponent->ClearPath(GetOwner());
                NavMesh::Get().BakeNavMesh(GetOwner()->GetScene()->GetRoot());
                AStarComponent->SetGraph(NavMesh::Get().GetGraph());
            }

            if (ImGui::Button("Compute Path"))
            {
                AStarComponent->ComputePath(AStarComponent->GetGoalPosition(), GetOwner());
            }

            if (ImGui::Button("Stop agent"))
            {
                AStarComponent->SetGraph(nullptr);
            }

            if (!AStarComponent->GetPath().empty())
            {
                ImGui::Text("Path Nodes: %zu", AStarComponent->GetPath().size());
                ImGui::Text("Current Node Index: %d", AStarComponent->GetCurrentPathIndex());

                if (ImGui::TreeNode("Path List"))
                {
                    for (size_t i = 0; i < AStarComponent->GetPath().size(); ++i)
                    {
                        ImGui::BulletText("Node ID: %d", AStarComponent->GetPath()[i]);
                    }
                    ImGui::TreePop();
                }
            }
            else
            {
                ImGui::Text("No active path.");
            }
        }
    }
#endif

    rapidjson::Value AiManager::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION

        END_COMPONENT_SERIALIZATION
    }

    void AiManager::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS

        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void AiManager::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

}
