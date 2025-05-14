#include "GenerativeSystem.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/Gui/EditorGUI.h"
#include "Serialization/SerializationUtility.h"
#include "spdlog/spdlog.h"
#include "Engine/Components/AI/NavMesh.h"

namespace Generation
{
    void GenerativeSystem::GenerateItems(Engine::Scene* Scene,
                                         const std::vector<std::pair<Models::Model*, Materials::Material*>>& Items,
                                         int ItemsCount, float ItemsDensity, std::vector<float> ItemsPercentages,
                                         float ItemsSizeMin, float ItemsSizeMax)
    {
        if (!PrepareNavMesh(Scene))
            return;

        if (Items.empty())
            return;

        NormalizePercentages(ItemsPercentages);

        std::vector<int> counts = CalculateModelCounts(ItemsPercentages, ItemsCount);

        glm::vec3 basePosition = GetRandomNavMeshPosition();

        Engine::Entity* parent = CreateParentEntity(Scene, basePosition, "Generated Items");

        SpawnItemsEntities(Scene, parent, Items, counts, basePosition, ItemsDensity, ItemsSizeMin, ItemsSizeMax);
    }

    bool GenerativeSystem::PrepareNavMesh(Engine::Scene* Scene)
    {
        Engine::NavMesh::Get().BakeNavMesh(Scene->GetRoot());
        const auto* graph = Engine::NavMesh::Get().GetGraph();
        return graph && !graph->GetAllNodes().empty();
    }

    glm::vec3 GenerativeSystem::GetRandomNavMeshPosition()
    {
        const auto* graph = Engine::NavMesh::Get().GetGraph();
        const auto& nodes = graph->GetAllNodes();

        std::vector<int> nodeIds;
        for (const auto& pair : nodes)
            nodeIds.push_back(pair.first);

        std::default_random_engine rng(std::random_device{}());
        std::uniform_int_distribution<int> nodeDist(0, nodeIds.size() - 1);

        int randomNodeId = nodeIds[nodeDist(rng)];
        const glm::vec3& pos = graph->GetNode(randomNodeId).GetPosition();
        return pos;
    }

    void GenerativeSystem::NormalizePercentages(std::vector<float>& Percentages)
    {
        float sum = 0.0f;
        for (float p : Percentages)
            sum += p;

        if (sum == 0.0f)
            return;

        for (float& p : Percentages)
            p /= sum;
    }

    std::vector<int> GenerativeSystem::CalculateModelCounts(const std::vector<float>& Percentages, int TotalCount)
    {
        std::vector<int> counts(Percentages.size(), 0);
        int accumulated = 0;

        for (size_t i = 0; i < Percentages.size(); ++i)
        {
            counts[i] = static_cast<int>(TotalCount * Percentages[i]);
            accumulated += counts[i];
        }

        while (accumulated < TotalCount)
        {
            for (size_t i = 0; i < counts.size() && accumulated < TotalCount; ++i)
            {
                counts[i]++;
                accumulated++;
            }
        }

        return counts;
    }

    Engine::Entity* GenerativeSystem::CreateParentEntity(Engine::Scene* Scene, const glm::vec3& Position,
                                                         const std::string Name)
    {
        Engine::Entity* parent = Scene->SpawnEntity(Scene->GetRoot());
        parent->SetName(Name);
        parent->SetScene(Scene);
        parent->GetTransform()->SetPosition(Position);
        return parent;
    }

    bool AabBsIntersect(const glm::vec3& MinA, const glm::vec3& MaxA,
                        const glm::vec3& MinB, const glm::vec3& MaxB)
    {
        return (MinA.x <= MaxB.x && MaxA.x >= MinB.x) &&
               (MinA.y <= MaxB.y && MaxA.y >= MinB.y) &&
               (MinA.z <= MaxB.z && MaxA.z >= MinB.z);
    }

    std::optional<glm::vec3> GenerativeSystem::GetNonOverlappingNavMeshPosition(
            const glm::vec3& Center, float BaseSpacing, const std::vector<std::pair<glm::vec3, glm::vec3>>& UsedAabbs,
            Perlin& Perlin, float PerlinScale, float PerlinMagnitude, const glm::vec3& AabbMin,
            const glm::vec3& AabbMax)
    {
        const auto* graph = Engine::NavMesh::Get().GetGraph();
        const auto& nodes = graph->GetAllNodes();

        std::vector<int> nodeIds;
        for (const auto& pair : nodes)
            nodeIds.push_back(pair.first);

        std::default_random_engine rng(std::random_device{}());
        std::shuffle(nodeIds.begin(), nodeIds.end(), rng);

        float spacing = BaseSpacing;
        const float spacingStep = 0.5f;
        const int maxAttempts = 10;

        for (int attempt = 0; attempt < maxAttempts; ++attempt)
        {
            for (int id : nodeIds)
            {
                glm::vec3 pos = nodes.at(id).GetPosition();

                if (glm::distance(pos, Center) > spacing)
                    continue;

                float noiseX = Perlin.Noise(pos.x * PerlinScale, pos.z * PerlinScale);
                float noiseZ = Perlin.Noise(pos.z * PerlinScale, pos.x * PerlinScale);
                glm::vec3 noisyPos = pos;
                noisyPos.x += (noiseX - 0.5f) * 2.0f * PerlinMagnitude;
                noisyPos.z += (noiseZ - 0.5f) * 2.0f * PerlinMagnitude;

                glm::vec3 testMin = noisyPos + AabbMin;
                glm::vec3 testMax = noisyPos + AabbMax;

                bool valid = true;
                for (const auto& [otherMin, otherMax] : UsedAabbs)
                {
                    if (AabBsIntersect(testMin, testMax, otherMin, otherMax))
                    {
                        valid = false;
                        break;
                    }
                }

                if (valid)
                    return noisyPos;
            }

            spacing += spacingStep;
        }

        return std::nullopt;
    }

    void GenerativeSystem::SpawnItemsEntities(Engine::Scene* Scene, Engine::Entity* Parent,
                                              const std::vector<std::pair<Models::Model*, Materials::Material*>>& Items,
                                              const std::vector<int>& Counts, const glm::vec3& BasePosition,
                                              float Spacing, float ItemsSizeMin, float ItemsSizeMax)
    {
        std::vector<std::pair<glm::vec3, glm::vec3>> usedAabBs;

        Perlin perlin(1337);
        float perlinScale = 0.2f;
        float perlinMagnitude = 1.0f;

        std::default_random_engine rng(std::random_device{}());
        std::uniform_real_distribution<float> sizeDist(ItemsSizeMin, ItemsSizeMax);

        for (size_t i = 0; i < Items.size(); ++i)
        {
            const auto& modelMaterialPair = Items[i];
            if (!modelMaterialPair.first || !modelMaterialPair.second)
                continue;

            glm::vec3 minAabb = glm::vec3(std::numeric_limits<float>::max());
            glm::vec3 maxAabb = glm::vec3(-std::numeric_limits<float>::max());

            for (size_t meshIndex = 0; meshIndex < modelMaterialPair.first->GetMeshCount(); ++meshIndex)
            {
                const auto& mesh = modelMaterialPair.first->GetMesh(meshIndex);
                const auto& aabb = mesh->GetAabBox();

                minAabb = glm::min(minAabb, aabb.min);
                maxAabb = glm::max(maxAabb, aabb.max);
            }

            for (int j = 0; j < Counts[i]; ++j)
            {
                float scaleFactor = sizeDist(rng);
                glm::vec3 scaledMin = minAabb * scaleFactor;
                glm::vec3 scaledMax = maxAabb * scaleFactor;
                float yOffset = scaledMin.y;

                auto spawnPosOpt = GetNonOverlappingNavMeshPosition(BasePosition, Spacing, usedAabBs,
                                                                    perlin, perlinScale, perlinMagnitude,
                                                                    scaledMin, scaledMax);

                if (!spawnPosOpt.has_value())
                {
                    spdlog::warn("Could not place item entity {} due to overlap. Skipping.", j + 1);
                    continue;
                }

                glm::vec3 spawnPos = *spawnPosOpt;
                glm::vec3 finalPos = glm::vec3(spawnPos.x, spawnPos.y - yOffset, spawnPos.z);

                Engine::Entity* newEntity = Scene->SpawnEntity(Parent);
                newEntity->GetTransform()->SetPosition(finalPos);

                auto modelRenderer = newEntity->AddComponent<Engine::ModelRenderer>();
                modelRenderer->SetModel(modelMaterialPair.first);
                modelRenderer->SetMaterial(modelMaterialPair.second);

                std::filesystem::path modelPath(modelMaterialPair.first->GetPath());
                std::string baseName = modelPath.stem().string();
                newEntity->SetName(baseName + " " + std::to_string(j + 1));
                newEntity->SetScene(Scene);
                newEntity->GetTransform()->SetScale(glm::vec3(scaleFactor));

                usedAabBs.emplace_back(spawnPos + scaledMin, spawnPos + scaledMax);
            }
        }
    }
}
