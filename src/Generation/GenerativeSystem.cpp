#include "GenerativeSystem.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/Gui/EditorGUI.h"
#include "Serialization/SerializationUtility.h"
#include "spdlog/spdlog.h"
#include "Engine/Components/AI/NavMesh.h"
#include "Engine/Components/Colliders/SphereCollider.h"
#include "Engine/Components/Game/Thrash.h"
#include "Engine/Components/Physics/Rigidbody.h"

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
            const glm::vec3& Center, float BaseSpacing,
            const std::vector<std::pair<glm::vec3, glm::vec3>>& UsedAabbs,
            Perlin& Perlin, float PerlinScale, float PerlinMagnitude,
            const glm::vec3& AabbMin, const glm::vec3& AabbMax,
            float yOffset, float yHeight)
    {
        const auto* graph = Engine::NavMesh::Get().GetGraph();
        const auto& nodes = graph->GetAllNodes();

        std::vector<int> nodeIds;
        for (const auto& pair : nodes)
            nodeIds.push_back(pair.first);

        std::default_random_engine rng(std::random_device{}());
        std::shuffle(nodeIds.begin(), nodeIds.end(), rng);

        const int horizontalAttempts = 10;
        const int maxStackLevels = 10;
        const float yStackStep = yHeight;

        for (int id : nodeIds)
        {
            glm::vec3 basePos = nodes.at(id).GetPosition();

            if (glm::distance(basePos, Center) > BaseSpacing)
                continue;

            std::vector<glm::vec3> failedNoisyPositions;

            for (int attempt = 0; attempt < horizontalAttempts; ++attempt)
            {
                float noiseX = Perlin.Noise(basePos.x * PerlinScale, basePos.z * PerlinScale);
                float noiseZ = Perlin.Noise(basePos.z * PerlinScale, basePos.x * PerlinScale);

                glm::vec3 noisyPos = basePos;
                noisyPos.x += (noiseX - 0.5f) * 2.0f * PerlinMagnitude;
                noisyPos.z += (noiseZ - 0.5f) * 2.0f * PerlinMagnitude;

                if (!Engine::NavMesh::Get().IsOnNavMesh(noisyPos, Engine::NavMesh::Get().GetSpacing()))
                    continue;

                glm::vec3 baseYOffset(0.0f, yOffset, 0.0f);
                glm::vec3 testMin = noisyPos + baseYOffset + AabbMin;
                glm::vec3 testMax = noisyPos + baseYOffset + AabbMax;

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
                    return noisyPos + baseYOffset;

                failedNoisyPositions.push_back(noisyPos);
            }

            for (const auto& noisyPos : failedNoisyPositions)
            {
                for (int stackLevel = 1; stackLevel < maxStackLevels; ++stackLevel)
                {
                    glm::vec3 stackYOffset(0.0f, stackLevel * yStackStep, 0.0f);
                    glm::vec3 baseYOffset(0.0f, yOffset, 0.0f);

                    glm::vec3 testMin = noisyPos + glm::vec3(AabbMin.x, 0.0f, AabbMin.z) + glm::vec3(
                                                0.0f, yOffset, 0.0f) + stackYOffset;
                    glm::vec3 testMax = noisyPos + glm::vec3(AabbMax.x, 0.0f, AabbMax.z) + glm::vec3(
                                                0.0f, yOffset, 0.0f) + stackYOffset;

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
                        return noisyPos + baseYOffset + stackYOffset;
                }
            }
        }

        return std::nullopt;
    }

    glm::vec3 RotateY(const glm::vec3& Point, float AngleRadians)
    {
        float cosA = std::cos(AngleRadians);
        float sinA = std::sin(AngleRadians);
        return glm::vec3(
                Point.x * cosA - Point.z * sinA,
                Point.y,
                Point.x * sinA + Point.z * cosA
                );
    }

    void GenerativeSystem::SpawnItemsEntities(Engine::Scene* Scene, Engine::Entity* Parent,
                                              const std::vector<std::pair<Models::Model*, Materials::Material*>>& Items,
                                              const std::vector<int>& Counts, const glm::vec3& BasePosition,
                                              float Spacing, float ItemsSizeMin, float ItemsSizeMax)
    {
        std::vector<std::pair<glm::vec3, glm::vec3>> usedAabBs;

        std::random_device rd;
        Perlin perlin(rd());
        float perlinScale = 0.2f;
        float perlinMagnitude = 1.0f;

        std::default_random_engine rng(std::random_device{}());
        std::uniform_real_distribution<float> sizeDist(ItemsSizeMin, ItemsSizeMax);

        std::vector<size_t> modelIndices;
        for (size_t i = 0; i < Counts.size(); ++i)
        {
            modelIndices.insert(modelIndices.end(), Counts[i], i);
        }

        std::shuffle(modelIndices.begin(), modelIndices.end(), rng);

        std::unordered_map<size_t, int> modelInstanceCounts;

        for (size_t idx = 0; idx < modelIndices.size(); ++idx)
        {
            size_t i = modelIndices[idx];
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

            float scaleFactor = sizeDist(rng);
            glm::vec3 scaledMin = minAabb * scaleFactor;
            glm::vec3 scaledMax = maxAabb * scaleFactor;
            float yOffset = scaledMin.y;

            std::uniform_real_distribution<float> rotationDist(0.0f, 360.0f);
            float randomYRotation = rotationDist(rng);

            std::vector<glm::vec3> corners = {
                    scaledMin,
                    scaledMax,
                    glm::vec3(scaledMin.x, scaledMin.y, scaledMax.z),
                    glm::vec3(scaledMin.x, scaledMax.y, scaledMin.z),
                    glm::vec3(scaledMax.x, scaledMin.y, scaledMin.z),
                    glm::vec3(scaledMax.x, scaledMax.y, scaledMin.z),
                    glm::vec3(scaledMin.x, scaledMax.y, scaledMax.z),
                    glm::vec3(scaledMax.x, scaledMin.y, scaledMax.z)
            };

            glm::vec3 rotMin(std::numeric_limits<float>::max());
            glm::vec3 rotMax(-std::numeric_limits<float>::max());

            for (const auto& corner : corners)
            {
                glm::vec3 rotated = RotateY(corner, glm::radians(randomYRotation));
                rotMin = glm::min(rotMin, rotated);
                rotMax = glm::max(rotMax, rotated);
            }

            float rotYOffset = -rotMin.y;
            float rotHeight = rotMax.y - rotMin.y;

            auto spawnPosOpt = GetNonOverlappingNavMeshPosition(BasePosition, Spacing, usedAabBs,
                                                                perlin, perlinScale, perlinMagnitude,
                                                                rotMin, rotMax, rotYOffset, rotHeight);

            if (!spawnPosOpt.has_value())
            {
                spdlog::warn("Failed to generate all items with the given parameters.");
                Scene->DeleteEntity(Parent);
                return;
            }

            glm::vec3 spawnPos = *spawnPosOpt;
            glm::vec3 finalPos = spawnPos;

            Engine::Entity* newEntity = Scene->SpawnEntity(Parent);
            newEntity->GetTransform()->SetPosition(finalPos);
            newEntity->GetTransform()->SetRotation(glm::vec3(0.0f, glm::radians(randomYRotation), 0.0f));

            auto modelRenderer = newEntity->AddComponent<Engine::ModelRenderer>();
            modelRenderer->SetModel(modelMaterialPair.first);
            modelRenderer->SetMaterial(modelMaterialPair.second);

            std::filesystem::path modelPath(modelMaterialPair.first->GetPath());
            std::filesystem::path materialPath(
                    Materials::MaterialManager::GetMaterialPath(modelMaterialPair.second));
            std::string modelName = modelPath.filename().string();
            std::string materialName = materialPath.filename().filename().string();
            std::string baseName = modelName + " " + materialName;
            modelInstanceCounts[i]++;
            int instanceNumber = modelInstanceCounts[i];

            newEntity->SetName(baseName + " " + std::to_string(instanceNumber));
            newEntity->SetScene(Scene);
            newEntity->GetTransform()->SetScale(glm::vec3(scaleFactor));

            usedAabBs.emplace_back(
                    finalPos + rotMin,
                    finalPos + rotMax);
        }
    }
}
