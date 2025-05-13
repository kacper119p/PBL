#pragma once

#include "Engine/EngineObjects/Scene/SceneManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Components/Updateable.h"
#include "Engine/Components/Renderers/ParticleEmitter.h"
#include "Engine/Components/Audio/AudioSource.h"
#include <filesystem>
#include <random>
#include "Perlin.h"
#include <optional>

namespace Generation
{
    /**
     * @brief System responsible for procedural generation of trash entities within a scene.
     */
    class GenerativeSystem
    {
    public:
        /**
         * @brief Generates items entities in the given scene using procedural rules.
         * @param Scene Pointer to the scene where items will be generated.
         * @param Items Vector of model-material pairs representing different items types.
         * @param ItemsCount Total number of items entities to spawn.
         * @param ItemsDensity Minimum spacing between items.
         * @param ItemsPercentages Percentage distribution of each item type.
         * @param ItemsSizeMin Minimum scale factor for items size.
         * @param ItemsSizeMax Maximum scale factor for items size.
         */
        void GenerateItems(Engine::Scene* Scene,
                           const std::vector<std::pair<Models::Model*, Materials::Material*>>& Items, int ItemsCount,
                           float ItemsDensity, std::vector<float> ItemsPercentages, float ItemsSizeMin,
                           float ItemsSizeMax);

    private:
        /**
         * @brief Ensures that the navigation mesh is valid and usable for placement.
         * @param Scene Pointer to the current scene.
         * @return True if the nav mesh is ready, false otherwise.
         */
        bool PrepareNavMesh(Engine::Scene* Scene);

        /**
         * @brief Selects a random valid position on the navigation mesh.
         * @return A position vector from the nav mesh.
         */
        glm::vec3 GetRandomNavMeshPosition();

        /**
         * @brief Normalizes a vector of percentages so they sum to 1.0.
         * @param Percentages Reference to a vector of percentages to normalize.
         */
        void NormalizePercentages(std::vector<float>& Percentages);

        /**
         * @brief Calculates how many entities to spawn for each items type based on percentages.
         * @param Percentages Normalized percentages.
         * @param TotalCount Total number of items entities to distribute.
         * @return A vector with spawn counts for each items type.
         */
        std::vector<int> CalculateModelCounts(const std::vector<float>& Percentages, int TotalCount);

        /**
         * @brief Creates a parent entity to group all spawned items.
         * @param Scene Target scene for spawning.
         * @param Position Center position for the items group.
         * @param Name Name of the parent entity.
         * @return Pointer to the created parent entity.
         */
        Engine::Entity* CreateParentEntity(Engine::Scene* Scene, const glm::vec3& Position, const std::string Name);

        /**
         * @brief Finds a position on the nav mesh where an AABB does not overlap with existing ones.
         * @param Center Center point to search around.
         * @param BaseSpacing Minimum allowed spacing.
         * @param UsedAabbs List of AABBs already in use.
         * @param Perlin Reference to a Perlin noise generator.
         * @param PerlinScale Scale of the noise input.
         * @param PerlinMagnitude Magnitude of noise perturbation.
         * @param AabbMin Minimum extent of the AABB to place.
         * @param AabbMax Maximum extent of the AABB to place.
         * @return A valid non-overlapping position, or std::nullopt if not found.
         */
        std::optional<glm::vec3> GetNonOverlappingNavMeshPosition(
                const glm::vec3& Center, float BaseSpacing,
                const std::vector<std::pair<glm::vec3, glm::vec3>>& UsedAabbs,
                Perlin& Perlin, float PerlinScale, float PerlinMagnitude, const glm::vec3& AabbMin,
                const glm::vec3& AabbMax);

        /**
         * @brief Spawns all items entities in the scene based on given parameters.
         * @param Scene Target scene for spawning.
         * @param Parent Entity under which all items will be grouped.
         * @param Items Vector of model-material pairs representing items types.
         * @param Counts Number of instances per items type.
         * @param BasePosition Starting position for placement.
         * @param Density Minimum spacing between items entities.
         * @param ItemsSizeMin Minimum size for scaling.
         * @param ItemsSizeMax Maximum size for scaling.
         */
        void SpawnItemsEntities(Engine::Scene* Scene, Engine::Entity* Parent,
                                const std::vector<std::pair<Models::Model*, Materials::Material*>>& Items,
                                const std::vector<int>& Counts, const glm::vec3& BasePosition, float Density,
                                float ItemsSizeMin, float ItemsSizeMax);
    };
}
