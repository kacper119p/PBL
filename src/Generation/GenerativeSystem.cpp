#include "GenerativeSystem.h"

#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/Gui/EditorGUI.h"
#include "Serialization/SerializationUtility.h"
#include "spdlog/spdlog.h"
#include "Engine/Components/AI/NavMesh.h"

namespace Generation
{
    void GenerativeSystem::GenerateTrash(Engine::Scene* scene,
                                         const std::vector<std::pair<Models::Model*, Materials::Material*>>& trash,
                                         int TrashCount, float TrashDensity, std::vector<float> trashPercentages,
                                         Engine::Entity*& LastGeneratedEntity)
    {
        if (!PrepareNavMesh(scene))
            return;

        if (trash.empty())
            return;

        NormalizePercentages(trashPercentages);

        std::vector<int> counts = CalculateModelCounts(trashPercentages, TrashCount);

        glm::vec3 basePosition = GetRandomNavMeshPosition(TrashDensity);

        Engine::Entity* parent = CreateParentEntity(scene, basePosition, "Generated Trash");

        SpawnTrashEntities(scene, parent, trash, counts, basePosition, TrashDensity);

        LastGeneratedEntity = parent;
    }

    bool GenerativeSystem::PrepareNavMesh(Engine::Scene* scene)
    {
        Engine::NavMesh::Get().BakeNavMesh(scene->GetRoot());
        const auto* graph = Engine::NavMesh::Get().GetGraph();
        return graph && !graph->GetAllNodes().empty();
    }

    glm::vec3 GenerativeSystem::GetRandomNavMeshPosition(float density)
    {
        const auto* graph = Engine::NavMesh::Get().GetGraph();
        const auto& nodes = graph->GetAllNodes();

        std::vector<int> nodeIds;
        for (const auto& pair : nodes)
            nodeIds.push_back(pair.first);

        std::default_random_engine rng(std::random_device{}());
        std::uniform_int_distribution<int> nodeDist(0, nodeIds.size() - 1);

        int randomNodeId = nodeIds[nodeDist(rng)];
        const glm::vec2& pos2D = graph->GetNode(randomNodeId).GetPosition();
        return glm::vec3(pos2D.x, 3.0f, pos2D.y);
    }

    void GenerativeSystem::NormalizePercentages(std::vector<float>& percentages)
    {
        float sum = 0.0f;
        for (float p : percentages)
            sum += p;

        if (sum == 0.0f)
            return;

        for (float& p : percentages)
            p /= sum;
    }

    std::vector<int> GenerativeSystem::CalculateModelCounts(const std::vector<float>& percentages, int totalCount)
    {
        std::vector<int> counts(percentages.size(), 0);
        int accumulated = 0;

        for (size_t i = 0; i < percentages.size(); ++i)
        {
            counts[i] = static_cast<int>(totalCount * percentages[i]);
            accumulated += counts[i];
        }

        while (accumulated < totalCount)
        {
            for (size_t i = 0; i < counts.size() && accumulated < totalCount; ++i)
            {
                counts[i]++;
                accumulated++;
            }
        }

        return counts;
    }

    Engine::Entity* GenerativeSystem::CreateParentEntity(Engine::Scene* scene, const glm::vec3& position,
                                                         const std::string name)
    {
        Engine::Entity* parent = scene->SpawnEntity(scene->GetRoot());
        parent->SetName(name);
        parent->SetScene(scene);
        parent->GetTransform()->SetPosition(position);
        return parent;
    }

    void GenerativeSystem::SpawnTrashEntities(Engine::Scene* scene, Engine::Entity* parent,
                                              const std::vector<std::pair<Models::Model*, Materials::Material*>>& trash,
                                              const std::vector<int>& counts, const glm::vec3& basePosition,
                                              float density)
    {
        std::default_random_engine rng(std::random_device{}());
        std::uniform_real_distribution<float> offsetDist(-density, density);

        for (size_t i = 0; i < trash.size(); ++i)
        {
            const auto& modelMaterialPair = trash[i];
            if (!modelMaterialPair.first || !modelMaterialPair.second)
                continue;

            for (int j = 0; j < counts[i]; ++j)
            {
                glm::vec3 spawnPos = basePosition + glm::vec3(offsetDist(rng), 0.0f, offsetDist(rng));

                Engine::Entity* newEntity = scene->SpawnEntity(parent);
                newEntity->GetTransform()->SetPosition(spawnPos);

                auto modelRenderer = newEntity->AddComponent<Engine::ModelRenderer>();
                modelRenderer->SetModel(modelMaterialPair.first);
                modelRenderer->SetMaterial(modelMaterialPair.second);

                std::filesystem::path modelPath(modelMaterialPair.first->GetPath());
                std::string baseName = modelPath.stem().string();
                newEntity->SetName(baseName + " " + std::to_string(j + 1));
                newEntity->SetScene(scene);

                spdlog::info("Generated Trash at position: ({}, {}, {})", spawnPos.x, spawnPos.y, spawnPos.z);
            }
        }
    }

    void GenerativeSystem::GenerateBlood(Engine::Scene* scene,
                                         const std::vector<std::pair<Models::Model*, Materials::Material*>>& stains,
                                         int BloodCount, float BloodSize, float BloodSpacing,
                                         Engine::Entity*& LastGeneratedEntity)
    {
        if (!PrepareNavMesh(scene) || stains.empty())
            return;

        glm::vec3 basePosition = GetRandomNavMeshPosition(BloodSpacing);
        Engine::Entity* parent = CreateParentEntity(scene, basePosition, "Generated Stains");

        SpawnBloodEntities(scene, parent, stains, BloodCount, basePosition, BloodSpacing, BloodSize);

        LastGeneratedEntity = parent;
    }

    void GenerativeSystem::SpawnBloodEntities(Engine::Scene* scene, Engine::Entity* parent,
                                              const std::vector<std::pair<Models::Model*, Materials::Material*>>&
                                              stains,
                                              int count, const glm::vec3& basePosition, float spacing, float size)
    {
        std::default_random_engine rng(std::random_device{}());
        std::uniform_real_distribution<float> offsetDist(-spacing, spacing);

        for (int i = 0; i < count; ++i)
        {
            glm::vec3 spawnPos(basePosition.x + offsetDist(rng), 0.0f, basePosition.z + offsetDist(rng));

            auto& modelMaterialPair = stains[i % stains.size()];
            if (!modelMaterialPair.first || !modelMaterialPair.second)
                continue;

            Engine::Entity* newEntity = scene->SpawnEntity(parent);
            newEntity->GetTransform()->SetPosition(spawnPos);
            newEntity->GetTransform()->SetScale(glm::vec3(size));

            auto modelRenderer = newEntity->AddComponent<Engine::ModelRenderer>();
            modelRenderer->SetModel(modelMaterialPair.first);
            modelRenderer->SetMaterial(modelMaterialPair.second);

            std::filesystem::path modelPath(modelMaterialPair.first->GetPath());
            std::string baseName = modelPath.stem().string();
            newEntity->SetName(baseName + " " + std::to_string(i + 1));
            newEntity->SetScene(scene);

            spdlog::info("Generated Blood at position: ({}, {}, {})", spawnPos.x, spawnPos.y, spawnPos.z);
        }
    }

    void GenerativeSystem::ClearGeneratedEntities(Engine::Entity*& GeneratedEntity)
    {
        if (GeneratedEntity && GeneratedEntity->GetScene())
        {
            GeneratedEntity->GetScene()->DeleteEntity(GeneratedEntity);
            GeneratedEntity = nullptr;
        }

        spdlog::info("Cleared all generated entities.");
    }

}
