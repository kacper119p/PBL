#include "NavMesh.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "spdlog/spdlog.h"
#include "Engine/EngineObjects/RayCast.h"

namespace Engine
{
    NavMesh& NavMesh::Get()
    {
        static NavMesh instance;
        return instance;
    }

    Graph* NavMesh::GetGraph()
    {
        return NavGraph.get();
    }

    bool NavMesh::IsOnNavMesh(const glm::vec3& Position, float MaxDistance)
    {
        const auto* graph = Engine::NavMesh::Get().GetGraph();
        if (!graph)
            return false;

        const auto& nodes = graph->GetAllNodes();
        for (const auto& [id, node] : nodes)
        {
            glm::vec3 nodePos = node.GetPosition();
            glm::vec2 nodeXz = glm::vec2(nodePos.x, nodePos.z);
            glm::vec2 posXz = glm::vec2(Position.x, Position.z);

            if (glm::distance(posXz, nodeXz) <= MaxDistance)
                return true;
        }

        return false;
    }

    void NavMesh::ClearGraph()
    {
        NavGraph = nullptr;
    }

    void NavMesh::BakeNavMesh(Entity* Root)
    {
        if (GetGraph())
        {
            ClearGraph();
            NavGraph = std::make_unique<Graph>();
        }
        BuildNavMesh(Root, Spacing, Padding);

        if (GetGraph()->GetAllNodes().empty())
        {
            spdlog::warn("NavGraph has 0 nodes!");
        }
    }

    bool IsPointInside(const glm::vec2& Point, const glm::vec4& Rect)
    {
        return Point.x >= Rect.x && Point.x <= Rect.z &&
               Point.y >= Rect.y && Point.y <= Rect.w;
    }

    void NavMesh::BuildNavMesh(Entity* Root, float Spacing, float Padding)
    {
        if (!Root)
            return;

        std::vector<std::pair<Models::Model*, glm::mat4>> modelTransforms;
        std::vector<std::pair<glm::vec2, glm::vec2>> blockedAreas;
        glm::vec2 sceneMin(FLT_MAX), sceneMax(-FLT_MAX);
        float largestModelSize = 1.0f;

        CollectModelData(Root, modelTransforms, blockedAreas, sceneMin, sceneMax, largestModelSize);
        ModelTransforms = modelTransforms;

        GenerateNavigationGrid(modelTransforms, blockedAreas, sceneMin, sceneMax, Spacing, Padding);
    }

    void NavMesh::CollectModelData(Entity* Root,
                                   std::vector<std::pair<Models::Model*, glm::mat4>>& ModelTransforms,
                                   std::vector<std::pair<glm::vec2, glm::vec2>>& BlockedAreas,
                                   glm::vec2& SceneMin,
                                   glm::vec2& SceneMax,
                                   float& LargestModelSize)
    {
        const auto& entities = Root->GetTransform()->GetChildren();

        for (const auto& entity : entities)
        {
            auto* owner = entity->GetOwner();
            auto* navArea = owner->GetComponent<NavArea>();
            auto* modelRenderer = owner->GetComponent<ModelRenderer>();
            if (!modelRenderer)
                continue;

            Models::Model* model = modelRenderer->GetModel();
            glm::mat4 transform = entity->GetLocalToWorldMatrix();

            if (navArea)
            {
                if (!navArea->GetWalkable())
                {
                    AddBlockedAreaFromModel(model, transform, BlockedAreas);
                }
                else
                {
                    ModelTransforms.emplace_back(model, transform);
                    UpdateSceneBoundsFromModel(model, transform, SceneMin, SceneMax, LargestModelSize);
                }
            }
        }
    }

    void NavMesh::AddBlockedAreaFromModel(Models::Model* Model, const glm::mat4& Transform,
                                          std::vector<std::pair<glm::vec2, glm::vec2>>& BlockedAreas)
    {
        for (int i = 0; i < Model->GetMeshCount(); ++i)
        {
            const auto& bounds = Model->GetMesh(i)->GetAabBox();

            glm::vec2 min(FLT_MAX), max(-FLT_MAX);
            for (const glm::vec3& corner : bounds.GetCorners())
            {
                glm::vec3 world = glm::vec3(Transform * glm::vec4(corner, 1.0f));
                min.x = std::min(min.x, world.x);
                min.y = std::min(min.y, world.z);
                max.x = std::max(max.x, world.x);
                max.y = std::max(max.y, world.z);
            }

            BlockedAreas.emplace_back(min, max);
        }
    }

    void NavMesh::UpdateSceneBoundsFromModel(Models::Model* Model, const glm::mat4& Transform,
                                             glm::vec2& SceneMin, glm::vec2& SceneMax,
                                             float& LargestModelSize)
    {
        for (int i = 0; i < Model->GetMeshCount(); ++i)
        {
            const auto& bounds = Model->GetMesh(i)->GetAabBox();

            for (const glm::vec3& corner : bounds.GetCorners())
            {
                glm::vec3 world = glm::vec3(Transform * glm::vec4(corner, 1.0f));
                SceneMin.x = std::min(SceneMin.x, world.x);
                SceneMin.y = std::min(SceneMin.y, world.z);
                SceneMax.x = std::max(SceneMax.x, world.x);
                SceneMax.y = std::max(SceneMax.y, world.z);
            }

            float sizeX = bounds.max.x - bounds.min.x;
            float sizeZ = bounds.max.z - bounds.min.z;
            LargestModelSize = std::max(LargestModelSize, std::max(sizeX, sizeZ));
        }
    }

    bool NavMesh::IsPointBlocked(const glm::vec2& Point,
                                 const std::vector<std::pair<glm::vec2, glm::vec2>>& BlockedAreas) const
    {
        for (const auto& area : BlockedAreas)
        {
            if (Point.x >= area.first.x && Point.x <= area.second.x &&
                Point.y >= area.first.y && Point.y <= area.second.y)
                return true;
        }
        return false;
    }

    bool NavMesh::FindClosestSurfaceHit(const glm::vec3& Origin,
                                        const std::vector<std::pair<Models::Model*, glm::mat4>>& ModelTransforms,
                                        glm::vec3& OutHitPoint) const
    {
        float closestDist = std::numeric_limits<float>::max();
        bool hit = false;
        glm::vec3 rayDir = glm::vec3(0, -1, 0);

        for (const auto& [model, transform] : ModelTransforms)
        {
            for (int i = 0; i < model->GetMeshCount(); ++i)
            {
                const auto& mesh = *model->GetMesh(i);
                const auto& verts = mesh.VerticesData;
                const auto& indices = mesh.VertexIndices;

                for (size_t j = 0; j + 2 < indices.size(); j += 3)
                {
                    glm::vec3 v0 = glm::vec3(transform * glm::vec4(verts[indices[j]].Position, 1.0f));
                    glm::vec3 v1 = glm::vec3(transform * glm::vec4(verts[indices[j + 1]].Position, 1.0f));
                    glm::vec3 v2 = glm::vec3(transform * glm::vec4(verts[indices[j + 2]].Position, 1.0f));
                    glm::vec3 hitPoint;

                    if (RayCast::RayIntersectsTriangle(Origin, rayDir, v0, v1, v2, &hitPoint))
                    {
                        float dist = glm::length(hitPoint - Origin);
                        if (dist < closestDist)
                        {
                            closestDist = dist;
                            OutHitPoint = hitPoint;
                            hit = true;
                        }
                    }
                }
            }
        }

        return hit;
    }

    void NavMesh::GenerateNavigationGrid(const std::vector<std::pair<Models::Model*, glm::mat4>>& ModelTransforms,
                                         const std::vector<std::pair<glm::vec2, glm::vec2>>& BlockedAreas,
                                         const glm::vec2& SceneMin,
                                         const glm::vec2& SceneMax,
                                         float Spacing,
                                         float Padding)
    {
        glm::vec2 paddedSceneMin = SceneMin + glm::vec2(Padding, Padding);
        glm::vec2 paddedSceneMax = SceneMax - glm::vec2(Padding, Padding);

        int width = static_cast<int>((paddedSceneMax.x - paddedSceneMin.x) / Spacing) + 1;
        int depth = static_cast<int>((paddedSceneMax.y - paddedSceneMin.y) / Spacing) + 1;

        auto nodeId = [width](int x, int z) { return z * width + x; };

        std::vector<std::pair<glm::vec2, glm::vec2>> expandedBlockedAreas;
        expandedBlockedAreas.reserve(BlockedAreas.size());
        for (const auto& area : BlockedAreas)
        {
            glm::vec2 expandedMin = area.first - glm::vec2(Padding, Padding);
            glm::vec2 expandedMax = area.second + glm::vec2(Padding, Padding);
            expandedBlockedAreas.emplace_back(expandedMin, expandedMax);
        }

        for (int z = 0; z < depth; ++z)
        {
            for (int x = 0; x < width; ++x)
            {
                glm::vec3 position = {
                        paddedSceneMin.x + x * Spacing,
                        1000.0f,
                        paddedSceneMin.y + z * Spacing
                };

                glm::vec2 flatPos(position.x, position.z);

                if (flatPos.x < paddedSceneMin.x || flatPos.x > paddedSceneMax.x ||
                    flatPos.y < paddedSceneMin.y || flatPos.y > paddedSceneMax.y)
                    continue;

                if (IsPointBlocked(flatPos, expandedBlockedAreas))
                    continue;

                glm::vec3 hit;
                if (FindClosestSurfaceHit(position, ModelTransforms, hit))
                {
                    int id = nodeId(x, z);
                    GetGraph()->AddNode(id, hit);

                    if (x > 0)
                        GetGraph()->AddConnection(id, nodeId(x - 1, z));
                    if (z > 0)
                        GetGraph()->AddConnection(id, nodeId(x, z - 1));
                    if (x > 0 && z > 0)
                        GetGraph()->AddConnection(id, nodeId(x - 1, z - 1));
                    if (x < width - 1 && z > 0)
                        GetGraph()->AddConnection(id, nodeId(x + 1, z - 1));
                }
            }
        }
    }

    int NavMesh::GetNodeIdFromPosition(const glm::vec3& Position) const
    {
        if (!NavGraph)
            return -1;

        const auto& nodes = NavGraph->GetAllNodes();
        float closestDistance = FLT_MAX;
        int closestNodeId = -1;

        for (const auto& nodePair : nodes)
        {
            const glm::vec3& nodePosition = nodePair.second.GetPosition();

            float distance = glm::length(glm::vec2(Position.x, Position.z) - glm::vec2(nodePosition.x, nodePosition.z));

            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestNodeId = nodePair.first;
            }
        }

        if (closestDistance > 2 * NavMesh::Get().GetSpacing())
        {
            return -1;
        }

        return closestNodeId;
    }
}
