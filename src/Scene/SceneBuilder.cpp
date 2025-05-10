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
        Engine::Entity* boxEntity = Scene->SpawnEntity(nullptr);
        Engine::CapsuleCollider* boxCollider = boxEntity->AddComponent<Engine::CapsuleCollider>();
        boxCollider->SetHeight(2.0f);
        boxCollider->SetRadius(.5f);
        boxEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 7.0f, 0.0f));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(.0f, 30.0f, 0.0f));

        Engine::Entity* secondBoxEntity = Scene->SpawnEntity(nullptr);
        Engine::BoxCollider* secondBoxCollider = secondBoxEntity->AddComponent<Engine::BoxCollider>();
        secondBoxEntity->GetTransform()->SetPosition(glm::vec3(2.0f, 7.0f, 0.0f));

        Engine::Entity* sphereEntity = Scene->SpawnEntity(nullptr);
        Engine::SphereCollider* sphereCollider = sphereEntity->AddComponent<Engine::SphereCollider>();
        sphereEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 14.0f, 0.0f));

        secondBoxEntity->AddComponent<Engine::MovementComponent>();

        CameraFollow::GetInstance().SetTarget(secondBoxEntity);
        #endif

        //secondBoxCollider->shouldMove = true;
        
        //Engine::RigidBody* rb = secondBoxEntity->AddComponent<Engine::RigidBody>();





        // BOX SPHERE SCENARIO /////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Engine::Entity* boxEntity = Scene->SpawnEntity(nullptr);
        //Engine::BoxCollider* boxCollider = boxEntity->AddComponent<Engine::BoxCollider>();
        //boxEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 20.0f));
        //boxEntity->GetTransform()->SetEulerAngles(glm::vec3(.0f, 30.0f, 0.0f));

        //Engine::Entity* sphereEntity = Scene->SpawnEntity(nullptr);
        //Engine::SphereCollider* sphereCollider = sphereEntity->AddComponent<Engine::SphereCollider>();
        //sphereEntity->GetTransform()->SetPosition(glm::vec3(6.0f, 0.0f, 20.0f));

        //boxCollider->AddColliderToSpatial(sphereCollider);
        //sphereCollider->AddColliderToSpatial(boxCollider);

        //boxCollider->SetStatic(false);
        //sphereCollider->SetStatic(false);

        //boxCollider->AddColliderToSpatial(sphereCollider);
        //sphereCollider->AddColliderToSpatial(boxCollider);

        //sphereCollider->shouldMove = true;





        // CAPSULE BOX SCENARIO /////////////////////////////////////////////////////////////////////////////////////////////////////////

       /* Engine::Entity* boxEntity = Scene->SpawnEntity(nullptr);
        Engine::BoxCollider* boxCollider = boxEntity->AddComponent<Engine::BoxCollider>();
        boxEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 20.0f));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(.0f, 30.0f, 0.0f));

        Engine::Entity* capsuleEntity = Scene->SpawnEntity(nullptr);
        Engine::CapsuleCollider* capsuleCollider = capsuleEntity->AddComponent<Engine::CapsuleCollider>();
        capsuleEntity->GetTransform()->SetPosition(glm::vec3(6.0f, 0.0f, 20.0f));
        capsuleEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 30.0f, 90.0f));

        boxCollider->AddColliderToSpatial(capsuleCollider);
        capsuleCollider->AddColliderToSpatial(boxCollider);

        boxCollider->SetStatic(false);
        capsuleCollider->SetStatic(false);
        boxCollider->AddColliderToSpatial(capsuleCollider);
        capsuleCollider->AddColliderToSpatial(boxCollider);

        capsuleCollider->shouldMove = true;*/
        





        // CAPSULE SPHERE SCENARIO /////////////////////////////////////////////////////////////////////////////////////////////////////////

       /* Engine::Entity* capsuleEntity = Scene->SpawnEntity(nullptr);
        Engine::CapsuleCollider* capsuleCollider = capsuleEntity->AddComponent<Engine::CapsuleCollider>();
        capsuleEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 20.0f));
        capsuleEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 0.0f, 90.0f));
        capsuleEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 30.0f, 90.0f));

        Engine::Entity* sphereEntity = Scene->SpawnEntity(nullptr);
        Engine::SphereCollider* sphereCollider = sphereEntity->AddComponent<Engine::SphereCollider>();
        sphereEntity->GetTransform()->SetPosition(glm::vec3(6.0f, 0.0f, 20.0f));

        capsuleCollider->AddColliderToSpatial(sphereCollider);
        sphereCollider->AddColliderToSpatial(capsuleCollider);

        capsuleCollider->SetStatic(false);
        sphereCollider->SetStatic(false);

        sphereCollider->shouldMove = true;*/
        





        // CAPSULE CAPSULE SCENARIO /////////////////////////////////////////////////////////////////////////////////////////////////////////
       /* Engine::Entity* capsuleEntity = Scene->SpawnEntity(nullptr);
        Engine::CapsuleCollider* capsuleCollider = capsuleEntity->AddComponent<Engine::CapsuleCollider>();
        capsuleEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 20.0f));
        capsuleEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 0.0f, 90.0f));
        capsuleEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 30.0f, 90.0f));

        Engine::Entity* capsuleEntity2 = Scene->SpawnEntity(nullptr);
        Engine::CapsuleCollider* capsuleCollider2 = capsuleEntity2->AddComponent<Engine::CapsuleCollider>();
        capsuleEntity2->GetTransform()->SetPosition(glm::vec3(8.0f, 0.0f, 20.0f));
        capsuleEntity2->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 0.0f, 90.0f));
        capsuleEntity2->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 60.0f, 90.0f));

        capsuleCollider->AddColliderToSpatial(capsuleCollider2);
        capsuleCollider2->AddColliderToSpatial(capsuleCollider);

        capsuleCollider->SetStatic(false);
        capsuleCollider2->SetStatic(false);


        capsuleCollider2->shouldMove = true;*/
        



        // TODO END

        // Engine::SceneManager::SaveScene("./res/scenes/SampleScene.lvl", Scene);
    }
} // namespace Scene
