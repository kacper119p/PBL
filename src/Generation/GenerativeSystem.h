#pragma once

#include "Engine/EngineObjects/Scene/SceneManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Components/Updateable.h"
#include "Engine/Components/Renderers/ParticleEmitter.h"
#include "Engine/Components/Audio/AudioSource.h"
#include "Engine/EngineObjects/Scene/SceneManager.h"
#include <filesystem>
#include <iostream>
#include <random>
#include "Engine/Components/AI/NavMesh.h"
#include "Engine/Gui/SceneViewGUI.h"

namespace Generation
{
    class GenerativeSystem
    {
    public:
        void GenerateTrash(Engine::Scene* scene,
                           const std::vector<std::pair<Models::Model*, Materials::Material*>>& trash, int TrashCount,
                           float TrashDensity, std::vector<float> trashPercentages,
                           Engine::Entity*& LastGeneratedEntity);

        void GenerateBlood(Engine::Scene* scene,
                           const std::vector<std::pair<Models::Model*, Materials::Material*>>& stains, int BloodCount,
                           float BloodSize, float BloodSpacing,
                           Engine::Entity*& LastGeneratedEntity);

        void ClearGeneratedEntities(Engine::Entity*& LastGeneratedEntity);

    private:
        bool PrepareNavMesh(Engine::Scene* scene);

        glm::vec3 GetRandomNavMeshPosition(float density);

        void NormalizePercentages(std::vector<float>& percentages);

        std::vector<int> CalculateModelCounts(const std::vector<float>& percentages, int totalCount);

        Engine::Entity* CreateParentEntity(Engine::Scene* scene, const glm::vec3& position, const std::string name);

        void SpawnTrashEntities(Engine::Scene* scene, Engine::Entity* parent,
                                const std::vector<std::pair<Models::Model*, Materials::Material*>>& trash,
                                const std::vector<int>& counts, const glm::vec3& basePosition, float density);

        void SpawnBloodEntities(Engine::Scene* scene, Engine::Entity* parent,
                                const std::vector<std::pair<Models::Model*, Materials::Material*>>& stains,
                                int count, const glm::vec3& basePosition, float spacing, float size);

    };
}
