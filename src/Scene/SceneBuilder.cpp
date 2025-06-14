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
#include "Engine/Components/Camera/CameraFollow.h"
#include "Engine/Components/BloodSystem/BloodEraser.h"
#include "Engine/Components/Game/Vacuum.h"


namespace Scene
{
    bool shouldContinue = true;
    void SceneBuilder::Build(Engine::Scene*& Scene)
    {
        ZoneScoped;

        Scene = new class Engine::Scene();
        Engine::SceneManager::LoadScene("./res/scenes/Gameplay.lvl", Scene);
        // TODO: remove when no longer needed
        

        
       

        // BOX BOX SCENARIO /////////////////////////////////////////////////////////////////////////////////////////////////////////
        #if !EDITOR
        
        /*Engine::Entity* playerVacuum = Scene->SpawnEntity(nullptr);
        playerVacuum->SetName("PlayerVacuum");
        Scene->GetPlayer()->GetTransform()->AddChild(playerVacuum->GetTransform());
        playerVacuum->AddComponent<Engine::Vacuum>();
        playerVacuum->GetTransform()->SetPosition(glm::vec3(0.0f,1.0f,-3.0f));*/

        /*Engine::Entity* playerBroom = Scene->SpawnEntity(nullptr);
        playerBroom->SetName("PlayerBroom");
        Scene->GetPlayer()->GetTransform()->AddChild(playerBroom->GetTransform());
        playerBroom->AddComponent<Engine::BloodEraser>();
        playerBroom->GetTransform()->SetPosition(glm::vec3(0.0f, 1.0f, -2.5f));*/

        /*Engine::Entity* box = Scene->SpawnEntity(nullptr);
        box->SetName("Box");
        Engine::BoxCollider* boxCollider = box->AddComponent<Engine::BoxCollider>();
        boxCollider->SetWidth(.25f);
        boxCollider->SetHeight(.25f);
        boxCollider->SetDepth(.25f);
        box->GetTransform()->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
        Engine::Rigidbody* rb1 = box->AddComponent<Engine::Rigidbody>();
        rb->restitution = 0.89f;
        box->GetTransform()->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        box->AddComponent<Engine::ModelRenderer>()->SetModel(Models::ModelManager::GetModel("./res/models/Box.fbx"));*/

        //secondBoxCollider->SetWidth(2.0f);
        //secondBoxCollider->SetHeight(2.0f);
        //secondBoxCollider->SetDepth(2.0f);

        /* Engine::Entity* secondBoxEntity2 = Scene->SpawnEntity(nullptr);
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

        Engine::Entity* triggerBoxEntity = Scene->SpawnEntity(nullptr);
        Engine::SphereCollider* triggerBoxCollider = triggerBoxEntity->AddComponent<Engine::SphereCollider>();
        triggerBoxCollider->SetRadius(1.0f);
        triggerBoxEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 2.1f, 0.0f));
        triggerBoxCollider->SetTrigger(true);
        Engine::ModelRenderer* modelTrigger = triggerBoxEntity->AddComponent<Engine::ModelRenderer>();
        modelTrigger->SetModel(Models::ModelManager::GetModel("./res/models/SphereLowPoly.fbx"));
        modelTrigger->SetMaterial(Materials::MaterialManager::GetMaterial("./res/materials/SampleScene/Default.mat"));

        CameraFollow::GetInstance().SetTarget(secondBoxEntity);
        */
        #endif
        
        //secondBoxCollider->shouldMove = true;
        
        //Engine::RigidBody* rb = secondBoxEntity->AddComponent<Engine::RigidBody>();


        //Engine::SceneManager::SaveScene("./res/scenes/Gameplay.lvl", Scene);
    }
} // namespace Scene
