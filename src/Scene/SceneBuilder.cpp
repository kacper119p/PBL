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
        Models::ModelAnimated* Model = &Models::ModelAnimated("./res/models/Submarine.fbx");
        Models::Animation* Animation = &Models::Animation("./res/models/Submarine.fbx", Model);
        renderer->SetModel(Model);
        renderer->SetAnimation(Animation);
        // Engine::SceneManager::SaveScene("./res/scenes/SampleScene.lvl", Scene);
    }

} // Scene
