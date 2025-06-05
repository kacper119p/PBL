#pragma once

#include "Engine/EngineObjects/Scene/SceneManager.h"
#include "Engine/EngineObjects/Entity.h"
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
                           const std::vector<std::string>& Items, int ItemsCount,
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
         * @param YOffset Vertical offset applied before stacking (e.g. object base height).
         * @param YHeight Vertical stacking step size.
         * @return A valid non-overlapping position, or std::nullopt if not found.
         */
        std::optional<glm::vec3> GetNonOverlappingNavMeshPosition(
                const glm::vec3& Center, float BaseSpacing,
                const std::vector<std::pair<glm::vec3, glm::vec3>>& UsedAabbs,
                Perlin& Perlin, float PerlinScale, float PerlinMagnitude,
                const glm::vec3& AabbMin, const glm::vec3& AabbMax,
                float YOffset, float YHeight);

        /**
         * @brief Returns a shuffled list of NavMesh node IDs.
         * @return Vector of shuffled node IDs.
         */
        std::vector<int> GetShuffledNodeIds();

        /**
         * @brief Tries to find a valid horizontal placement using Perlin noise.
         * @param BasePos Base position from a NavMesh node.
         * @param BaseSpacing Max allowed distance from center.
         * @param UsedAabbs List of bounding boxes to avoid.
         * @param Perlin Perlin noise generator.
         * @param PerlinScale Perlin noise scale.
         * @param PerlinMagnitude Perlin noise offset magnitude.
         * @param AabbMin Minimum AABB corner of the item.
         * @param AabbMax Maximum AABB corner of the item.
         * @param YOffset Vertical offset for placement.
         * @param Attempts Number of noise-based attempts.
         * @param FailedNoisyPositions Output vector for failed positions to try stacking.
         * @return Valid non-overlapping position or std::nullopt.
         */
        std::optional<glm::vec3> TryFindHorizontalPlacement(const glm::vec3& BasePos, float BaseSpacing,
                                                            const std::vector<std::pair<glm::vec3, glm::vec3>>&
                                                            UsedAabbs, Perlin& Perlin, float PerlinScale,
                                                            float PerlinMagnitude, const glm::vec3& AabbMin,
                                                            const glm::vec3& AabbMax, float YOffset, int Attempts,
                                                            std::vector<glm::vec3>& FailedNoisyPositions);

        /**
         * @brief Attempts vertical stacking placement from failed horizontal attempts.
         * @param FailedPositions List of positions that failed horizontal placement.
         * @param UsedAabbs List of bounding boxes to avoid.
         * @param AabbMin Minimum AABB corner of the item.
         * @param AabbMax Maximum AABB corner of the item.
         * @param YOffset Vertical offset for placement.
         * @param YStep Vertical step per stack level.
         * @param MaxStackLevels Maximum number of vertical levels to try.
         * @return Valid stacked position or std::nullopt.
         */
        std::optional<glm::vec3> TryStackingPlacement(const std::vector<glm::vec3>& FailedPositions,
                                                      const std::vector<std::pair<glm::vec3, glm::vec3>>& UsedAabbs,
                                                      const glm::vec3& AabbMin, const glm::vec3& AabbMax,
                                                      float YOffset, float YStep, int MaxStackLevels);

        /**
         * @brief Spawns all item entities in the scene based on given parameters.
         * @param Scene Target scene for spawning.
         * @param Parent Entity under which all items will be grouped.
         * @param Items Vector of pointers to item prefabs.
         * @param Counts Number of instances to spawn per item type.
         * @param BasePosition Starting position for item placement.
         * @param Density Minimum spacing between spawned items.
         * @param ItemsSizeMin Minimum scale factor for items.
         * @param ItemsSizeMax Maximum scale factor for items.
         */
        void SpawnItemsEntities(Engine::Scene* Scene, Engine::Entity* Parent,
                                const std::vector<std::string>& Items,
                                const std::vector<int>& Counts, const glm::vec3& BasePosition,
                                float Density, float ItemsSizeMin, float ItemsSizeMax);

        /**
         * @brief Creates and shuffles a list of model indices based on counts.
         * @param Counts Number of instances per model.
         * @param Rng Random engine used for shuffling.
         * @return Vector of shuffled model indices.
         */
        std::vector<size_t> CreateShuffledModelIndices(const std::vector<int>& Counts,
                                                       std::default_random_engine& Rng);

        /**
         * @brief Calculates the rotated bounding box for a model at given scale and rotation.
         * @param Model Pointer to the model.
         * @param ScaleFactor Scale factor applied to the model.
         * @param YRotationDeg Rotation around Y-axis in degrees.
         * @return Tuple containing min and max points of rotated bounding box, Y offset, and height.
         */
        std::tuple<glm::vec3, glm::vec3, float, float> CalculateRotatedBoundingBox(Models::Model* Model,
            float ScaleFactor, float YRotationDeg);

        /**
         * @brief Creates a new entity instance from a prefab and places it in the scene hierarchy.
         * @param Scene Target scene.
         * @param Prefab Entity prefab to clone.
         * @param Parent Parent entity to attach to.
         * @param Position Position to place the new entity.
         * @param YRotationDeg Rotation around Y-axis in degrees.
         * @param Scale Scale factor for the entity.
         * @param InstanceIndex Index used to uniquely name the instance.
         * @return Pointer to the newly created entity.
         */
        Engine::Entity* CreateAndPlaceEntity(Engine::Scene* Scene, Engine::Entity* Prefab, Engine::Entity* Parent,
                                             const glm::vec3& Position, float YRotationDeg, float Scale,
                                             int InstanceIndex);

    };
}
