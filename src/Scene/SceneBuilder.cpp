#include "SceneBuilder.h"
#include "Shaders/Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>
#include "Utility/TextureUtilities.h"
#include "Models/Model.h"
#include "Models/ModelAnimated.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/Components/Renderers/AnimatedModelRenderer.h"
#include "Engine/Components/Lights/DirectionalLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"
#include "Materials/SkyboxMaterial.h"
#include "Materials/ReflectiveMaterial.h"
#include "Materials/RefractiveMaterial.h"
#include "Shaders/ComputeShader.h"
#include "Engine/Components/Renderers/ParticleEmitter.h"
#include "Materials/WaterMaterial.h"
#include "Engine/Components/Game/ShipRoller.h"
#include "Engine/Components/Game/Rotator.h"
#include "Engine/EngineObjects/Scene/SceneManager.h"
#include "Engine/Textures/Texture.h"
#include "Engine/Textures/TextureManager.h"
#include "Materials/AnimatedPbrMaterial.h"
#include "Materials/MaterialManager.h"
#include "Materials/PbrMaterial.h"
#include "Models/ModelManager.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"
#include "tracy/Tracy.hpp"

namespace Scene
{
    SceneBuilder::~SceneBuilder() = default;

    void SceneBuilder::Build(Engine::Scene*& Scene)
    {
        ZoneScoped;

        Scene = new class Engine::Scene();
        Engine::SceneManager::LoadScene("./res/scenes/SampleScene.lvl", Scene);
        Engine::Entity* entity = Scene->SpawnEntity(nullptr);
        Engine::AnimatedModelRenderer* renderer = entity->AddComponent<Engine::AnimatedModelRenderer>();
        Models::ModelAnimated* submarineModel = Models::ModelManager::GetAnimatedModel("./res/models/Submarine.fbx");
        Models::Animation* animation = new Models::Animation("./res/models/Submarine.fbx", submarineModel);
        renderer->SetModel(submarineModel);
        renderer->SetAnimation(animation);

        Engine::Texture submarineBaseMap = Engine::TextureManager::GetTexture("./res/textures/Submarine/Base.png");
        Engine::Texture submarineRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Submarine/RoughnessMetallicAmbientOcclusion.png");
        Engine::Texture submarineNormalMap = Engine::TextureManager::GetTexture("./res/textures/Submarine/Normal.png");
        Engine::Texture submarineEmissiveMap = Engine::TextureManager::GetTexture(
                "./res/textures/EmissiveMapDefault.png");
        Materials::Material* submarineMaterial = new Materials::AnimatedPbrMaterial(
                submarineBaseMap,
                submarineRmaoMap, submarineNormalMap,
                submarineEmissiveMap,
                glm::vec3(1.0f), 1.0f, 1.0f, glm::vec3(0.0f));
        Materials::MaterialManager::SaveMaterial("./res/materials/SampleScene/SubmarineAnimated.mat",
                                                 submarineMaterial);
        renderer->SetMaterial(submarineMaterial);
        renderer->SetAnimator();
        renderer->Start();

        Engine::SceneManager::SaveScene("./res/scenes/SampleScene.lvl", Scene);
    }

} // Scene
