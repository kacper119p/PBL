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
#include "Engine/Components/Colliders/Collider.h"
#include "Engine/Components/Colliders/BoxCollider.h"
#include "Engine/Components/Colliders/SphereCollider.h"
#include "Engine/Components/Colliders/CapsuleCollider.h"
#include "Engine/Components/Colliders/SpatialPartitioning.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Components/BaseMovement/MovementComponent.h"
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
#include "Engine/Components/Colliders/SpatialPartitioning.h"
#include "Engine/Components/Camera/CameraFollow.h"


namespace Scene
{
    bool shouldContinue = true;
    void SceneBuilder::Build(Engine::Scene*& Scene)
    {
        ZoneScoped;

        Scene = new class Engine::Scene();
        Engine::SceneManager::LoadScene("./res/scenes/BlockLayout.lvl", Scene);
        // TODO: remove when no longer needed



       

        // BOX BOX SCENARIO /////////////////////////////////////////////////////////////////////////////////////////////////////////
        #if !EDITOR
        Engine::Entity* secondBoxEntity = Scene->SpawnEntity(nullptr);
        Engine::ModelRenderer* secondBoxModelRenderer = secondBoxEntity->AddComponent<Engine::ModelRenderer>();
        secondBoxModelRenderer->SetModel(Models::ModelManager::GetModel("./res/models/Box.fbx"));
        secondBoxModelRenderer->SetMaterial(
                Materials::MaterialManager::GetMaterial("./res/materials/SampleScene/Box.mat"));

        Engine::BoxCollider* secondBoxCollider = secondBoxEntity->AddComponent<Engine::BoxCollider>();
        secondBoxEntity->GetTransform()->SetPosition(glm::vec3(2.0f, 7.0f, 0.0f));
        secondBoxCollider->SetWidth(2.0f);
        secondBoxCollider->SetHeight(2.0f);
        secondBoxCollider->SetDepth(2.0f);

        Engine::Entity* secondBoxEntity2 = Scene->SpawnEntity(nullptr);
        Engine::BoxCollider* secondBoxCollider2 = secondBoxEntity2->AddComponent<Engine::BoxCollider>();

        secondBoxCollider2->SetWidth(2.0f);
        secondBoxCollider2->SetHeight(2.0f);
        secondBoxCollider2->SetDepth(2.0f);
        secondBoxEntity2->GetTransform()->SetPosition(glm::vec3(0.0f, 7.0f, 4.0f));
        secondBoxCollider2->SetStatic(false);

        Engine::ModelRenderer* secondBoxModelRenderer2 = secondBoxEntity2->AddComponent<Engine::ModelRenderer>();
        secondBoxModelRenderer2->SetModel(Models::ModelManager::GetModel("./res/models/Box.fbx"));
        secondBoxModelRenderer2->SetMaterial(
                Materials::MaterialManager::GetMaterial("./res/materials/SampleScene/Box.mat"));
        secondBoxModelRenderer2->GetMaterial()->GetMainPass().SetUniform("Tint", glm::vec3(0.0f, 25.0f, 0.0f));

        secondBoxEntity->AddComponent<Engine::MovementComponent>();
        
        Engine::Rigidbody* rb2 = secondBoxEntity2->AddComponent<Engine::Rigidbody>();
        Engine::Rigidbody* rb = secondBoxEntity->AddComponent<Engine::Rigidbody>();


        CameraFollow::GetInstance().SetTarget(secondBoxEntity);
        #endif

        //secondBoxCollider->shouldMove = true;
        
        //Engine::RigidBody* rb = secondBoxEntity->AddComponent<Engine::RigidBody>();


        // Engine::SceneManager::SaveScene("./res/scenes/SampleScene.lvl", Scene);
    }
} // namespace Scene
