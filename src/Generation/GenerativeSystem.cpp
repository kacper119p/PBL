#include "GenerativeSystem.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/Gui/EditorGUI.h"
#include "Serialization/SerializationUtility.h"
#include "spdlog/spdlog.h"
#include "Engine/Components/AI/NavMesh.h"

namespace Generation
{
    void GenerativeSystem::GenerateItems(Engine::Scene* Scene,
                                         const std::vector<std::string>& Items,
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
            float YOffset, float YHeight)
    {
        const int horizontalAttempts = 10;
        const int maxStackLevels = 10;

        for (int id : GetShuffledNodeIds())
        {
            glm::vec3 basePos = Engine::NavMesh::Get().GetGraph()->GetAllNodes().at(id).GetPosition();

            if (glm::distance(basePos, Center) > BaseSpacing)
                continue;

            std::vector<glm::vec3> failedNoisyPositions;
            auto horizontalPos = TryFindHorizontalPlacement(basePos, BaseSpacing, UsedAabbs,
                                                            Perlin, PerlinScale, PerlinMagnitude,
                                                            AabbMin, AabbMax, YOffset,
                                                            horizontalAttempts, failedNoisyPositions);

            if (horizontalPos.has_value())
                return horizontalPos;

            auto stackedPos = TryStackingPlacement(failedNoisyPositions, UsedAabbs,
                                                   AabbMin, AabbMax, YOffset,
                                                   YHeight, maxStackLevels);

            if (stackedPos.has_value())
                return stackedPos;
        }

        return std::nullopt;
    }

    std::vector<int> GenerativeSystem::GetShuffledNodeIds()
    {
        const auto* graph = Engine::NavMesh::Get().GetGraph();
        const auto& nodes = graph->GetAllNodes();

        std::vector<int> nodeIds;
        for (const auto& pair : nodes)
            nodeIds.push_back(pair.first);

        std::default_random_engine rng(std::random_device{}());
        std::shuffle(nodeIds.begin(), nodeIds.end(), rng);
        return nodeIds;
    }

    std::optional<glm::vec3> GenerativeSystem::TryFindHorizontalPlacement(
            const glm::vec3& BasePos, float BaseSpacing,
            const std::vector<std::pair<glm::vec3, glm::vec3>>& UsedAabbs,
            Perlin& Perlin, float PerlinScale, float PerlinMagnitude,
            const glm::vec3& AabbMin, const glm::vec3& AabbMax,
            float YOffset, int Attempts, std::vector<glm::vec3>& FailedNoisyPositions)
    {
        for (int attempt = 0; attempt < Attempts; ++attempt)
        {
            float noiseX = Perlin.Noise(BasePos.x * PerlinScale, BasePos.z * PerlinScale);
            float noiseZ = Perlin.Noise(BasePos.z * PerlinScale, BasePos.x * PerlinScale);

            glm::vec3 noisyPos = BasePos;
            noisyPos.x += (noiseX - 0.5f) * 2.0f * PerlinMagnitude;
            noisyPos.z += (noiseZ - 0.5f) * 2.0f * PerlinMagnitude;

            if (!Engine::NavMesh::Get().IsOnNavMesh(noisyPos, Engine::NavMesh::Get().GetSpacing()))
                continue;

            glm::vec3 baseYOffset(0.0f, YOffset, 0.0f);
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

            FailedNoisyPositions.push_back(noisyPos);
        }

        return std::nullopt;
    }

    std::optional<glm::vec3> GenerativeSystem::TryStackingPlacement(
            const std::vector<glm::vec3>& FailedPositions,
            const std::vector<std::pair<glm::vec3, glm::vec3>>& UsedAabbs,
            const glm::vec3& AabbMin, const glm::vec3& AabbMax,
            float YOffset, float YStep, int MaxStackLevels)
    {
        for (const auto& noisyPos : FailedPositions)
        {
            float baseY = 0.0f;

            for (int stackLevel = 0; stackLevel < MaxStackLevels; ++stackLevel)
            {
                glm::vec3 candidateMin = noisyPos + glm::vec3(AabbMin.x, baseY + YOffset, AabbMin.z);
                glm::vec3 candidateMax = noisyPos + glm::vec3(AabbMax.x, baseY + YOffset, AabbMax.z);

                bool intersects = false;
                float topY = baseY;

                for (const auto& [otherMin, otherMax] : UsedAabbs)
                {
                    bool overlapsXZ =
                            candidateMax.x > otherMin.x && candidateMin.x < otherMax.x &&
                            candidateMax.z > otherMin.z && candidateMin.z < otherMax.z;

                    if (overlapsXZ && candidateMin.y < otherMax.y && candidateMax.y > otherMin.y)
                    {
                        intersects = true;
                        topY = std::max(topY, otherMax.y - noisyPos.y);
                    }
                }

                if (!intersects)
                {
                    return noisyPos + glm::vec3(0.0f, baseY + YOffset, 0.0f);
                }

                baseY = topY + 0.01f;
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
                                              const std::vector<std::string>& Items,
                                              const std::vector<int>& Counts, const glm::vec3& BasePosition,
                                              float Spacing, float ItemsSizeMin, float ItemsSizeMax)
    {
        if (Items.empty())
            return;

        std::vector<std::pair<glm::vec3, glm::vec3>> usedAabBs;
        std::default_random_engine rng(std::random_device{}());

        Perlin perlin(std::random_device{}());
        float perlinScale = 0.2f;
        float perlinMagnitude = 1.0f;

        std::uniform_real_distribution<float> sizeDist(ItemsSizeMin, ItemsSizeMax);
        std::uniform_real_distribution<float> rotationDist(0.0f, 360.0f);

        std::vector<size_t> modelIndices = CreateShuffledModelIndices(Counts, rng);

        std::unordered_map<size_t, int> modelInstanceCounts;

        for (size_t idx : modelIndices)
        {
            Engine::Entity* prefab = Engine::PrefabLoader::LoadPrefab(Items[idx], Scene, Parent->GetTransform());
            auto modelRenderer = prefab->GetComponent<Engine::ModelRenderer>();
            if (!modelRenderer)
                continue;

            Models::Model* model = modelRenderer->GetModel();

            float scaleFactor = sizeDist(rng);
            float randomYRotation = rotationDist(rng);

            glm::vec3 rotMin, rotMax;
            float rotYOffset, rotHeight;
            std::tie(rotMin, rotMax, rotYOffset, rotHeight) = CalculateRotatedBoundingBox(
                    model, scaleFactor, randomYRotation);

            auto spawnPosOpt = GetNonOverlappingNavMeshPosition(BasePosition, Spacing, usedAabBs,
                                                                perlin, perlinScale, perlinMagnitude,
                                                                rotMin, rotMax, rotYOffset, rotHeight);

            if (!spawnPosOpt.has_value())
            {
                spdlog::warn("Failed to generate all items with the given parameters.");
                Scene->DeleteEntity(Parent);
                return;
            }

            glm::vec3 finalPos = *spawnPosOpt;
            Engine::Entity* newEntity = CreateAndPlaceEntity(Scene, prefab, Parent, finalPos, randomYRotation,
                                                             scaleFactor, ++modelInstanceCounts[idx]);
            usedAabBs.emplace_back(finalPos + rotMin, finalPos + rotMax);
        }
    }

    std::vector<size_t> GenerativeSystem::CreateShuffledModelIndices(const std::vector<int>& Counts,
                                                                     std::default_random_engine& Rng)
    {
        std::vector<size_t> indices;
        for (size_t i = 0; i < Counts.size(); ++i)
            indices.insert(indices.end(), Counts[i], i);

        std::shuffle(indices.begin(), indices.end(), Rng);
        return indices;
    }

    std::tuple<glm::vec3, glm::vec3, float, float> GenerativeSystem::CalculateRotatedBoundingBox(
            Models::Model* Model, float ScaleFactor, float YRotationDeg)
    {
        glm::vec3 minAabb(FLT_MAX), maxAabb(-FLT_MAX);

        for (size_t i = 0; i < Model->GetMeshCount(); ++i)
        {
            const auto& mesh = Model->GetMesh(i);
            const auto& aabb = mesh->GetAabBox();
            minAabb = glm::min(minAabb, aabb.min);
            maxAabb = glm::max(maxAabb, aabb.max);
        }

        glm::vec3 scaledMin = minAabb * ScaleFactor;
        glm::vec3 scaledMax = maxAabb * ScaleFactor;

        std::vector<glm::vec3> corners = {
                scaledMin, scaledMax,
                {scaledMin.x, scaledMin.y, scaledMax.z},
                {scaledMin.x, scaledMax.y, scaledMin.z},
                {scaledMax.x, scaledMin.y, scaledMin.z},
                {scaledMax.x, scaledMax.y, scaledMin.z},
                {scaledMin.x, scaledMax.y, scaledMax.z},
                {scaledMax.x, scaledMin.y, scaledMax.z}
        };

        glm::vec3 rotMin(FLT_MAX), rotMax(-FLT_MAX);
        for (const auto& c : corners)
        {
            glm::vec3 rotated = RotateY(c, glm::radians(YRotationDeg));
            rotMin = glm::min(rotMin, rotated);
            rotMax = glm::max(rotMax, rotated);
        }

        float offsetY = -rotMin.y;
        float height = rotMax.y - rotMin.y;

        return {rotMin, rotMax, offsetY, height};
    }

    Engine::Entity* GenerativeSystem::CreateAndPlaceEntity(Engine::Scene* Scene, Engine::Entity* Prefab,
                                                           Engine::Entity* Parent, const glm::vec3& Position,
                                                           float YRotationDeg, float Scale, int InstanceIndex)
    {
        Engine::Entity* entity = Prefab;
        entity->SetScene(Scene);
        entity->GetTransform()->SetParent(Parent->GetTransform());
        entity->GetTransform()->SetPosition(Position);
        entity->GetTransform()->SetRotation(glm::vec3(0.0f, glm::radians(YRotationDeg), 0.0f));
        entity->GetTransform()->SetScale(glm::vec3(Scale));

        std::string baseName = Prefab->GetName();
        entity->SetName(baseName + " " + std::to_string(InstanceIndex));

        return entity;
    }
}
