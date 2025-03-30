#include "SceneBuilder.h"
#include "Shaders/Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>
#include "Utility/TextureUtilities.h"
#include "Models/Model.h"
#include "Engine/EngineObjects/Scene.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
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
#include "Engine/Textures/Texture.h"
#include "Engine/Textures/TextureManager.h"
#include "Materials/PbrMaterial.h"
#include "Models/ModelManager.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"
#include "tracy/Tracy.hpp"

namespace Scene
{
    SceneBuilder::~SceneBuilder() = default;

    void
    SceneBuilder::Build(Engine::Scene*& Scene)
    {
        ZoneScoped;
        Scene = new class Engine::Scene();
        std::vector<Engine::Texture>& Textures = Scene->Textures;
        std::vector<Models::Model*>& Models = Scene->Models;
        std::vector<Shaders::Shader>& Shaders = Scene->Shaders;
        std::vector<Materials::Material*>& Materials = Scene->Materials;

        Shaders::ShaderSourceFiles depthSource("./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                                               "./res/shaders/DefaultDepth/DefaultDepth.frag");

        Shaders::ShaderSourceFiles shaderSource("./res/shaders/PBR/PBR.vert", nullptr,
                                                "./res/shaders/PBR/PBR.frag");

        Shaders::ShaderSourceFiles directionalShadowPass(
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.vert",
                nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag");

        Shaders::ShaderSourceFiles pointSpotShadowPass(
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag");

        Shaders::ShaderSourceFiles skyboxShaderSource(
                "./res/shaders/Skybox/Skybox.vert",
                nullptr,
                "./res/shaders/Skybox/Skybox.frag");

        Shaders::ShaderSourceFiles skyboxDirectionalLightShadowShaderSource(
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.vert",
                nullptr,
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.frag");

        Shaders::ShaderSourceFiles reflectiveShaderSource(
                "./res/shaders/Reflective/Reflective.vert",
                nullptr,
                "./res/shaders/Reflective/Reflective.frag");

        Shaders::ShaderSourceFiles refractiveShaderSource(
                "./res/shaders/Refractive/Refractive.vert",
                nullptr,
                "./res/shaders/Refractive/Refractive.frag");

        Shaders::ShaderSourceFiles waterShaderSource(
                "./res/shaders/Water/Water.vert",
                nullptr,
                "./res/shaders/Water/Water.frag");

        Shaders::ShaderSourceFiles particleRenderSource(
                "./res/shaders/Particles/Particles.vert",
                nullptr,
                "./res/shaders/Particles/Particle.frag");

        Shaders::Shader depth = Shaders::ShaderManager::GetShader(depthSource);
        Shaders::Shader shader = Shaders::ShaderManager::GetShader(shaderSource);
        Shaders::Shader directionalPass = Shaders::ShaderManager::GetShader(directionalShadowPass);
        Shaders::Shader pointPass = Shaders::ShaderManager::GetShader(pointSpotShadowPass);
        Shaders::Shader skyboxShader = Shaders::ShaderManager::GetShader(skyboxShaderSource);
        Shaders::Shader skyboxDirectionalLightShader = Shaders::ShaderManager::GetShader(
                skyboxDirectionalLightShadowShaderSource);
        Shaders::Shader reflectiveShader = Shaders::ShaderManager::GetShader(reflectiveShaderSource);
        Shaders::Shader refractiveShader = Shaders::ShaderManager::GetShader(refractiveShaderSource);
        Shaders::Shader waterShader = Shaders::ShaderManager::GetShader(waterShaderSource);

        Shaders::Shader particleRenderShader = Shaders::ShaderManager::GetShader(particleRenderSource);
        Shaders::ComputeShader particleSpawnShader = Shaders::ShaderManager::GetComputeShader(
                "./res/shaders/Particles/ParticlesSpawn.comp");
        Shaders::ComputeShader particleUpdateShader = Shaders::ShaderManager::GetComputeShader(
                "./res/shaders/Particles/ParticlesUpdate.comp");

        Shaders.push_back(depth);
        Shaders.push_back(shader);
        Shaders.push_back(directionalPass);
        Shaders.push_back(pointPass);
        Shaders.push_back(skyboxShader);
        Shaders.push_back(skyboxShader);
        Shaders.push_back(skyboxDirectionalLightShader);
        Shaders.push_back(reflectiveShader);
        Shaders.push_back(refractiveShader);
        Shaders.push_back(waterShader);

        Shaders.push_back(particleRenderShader);


        unsigned int environmentMap = Utility::LoadHdrCubeMapFromFile("./res/textures/Skyboxes/Skybox.hdr");

        Engine::LightManager::GetInstance()->SetEnvironmentMap(environmentMap);

        Textures.push_back(Engine::Texture(environmentMap));

        /*
         * Assembling Submarine
         */

        unsigned int submarineBaseMap = Engine::TextureManager::GetTexture("./res/textures/Submarine/Base.png").GetId();
        unsigned int submarineRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Submarine/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int submarineNormalMap = Engine::TextureManager::GetTexture("./res/textures/Submarine/Normal.png").
                GetId();

        unsigned int submarineEmissiveMap = Engine::TextureManager::GetTexture("./res/textures/EmissiveMapDefault.png").
                GetId();

        Models::Model* submarineModel = Models::ModelManager::GetModel("./res/models/Submarine.fbx");
        Materials::PbrMaterial* submarineMaterial = new Materials::PbrMaterial(
                submarineBaseMap,
                submarineRmaoMap, submarineNormalMap,
                submarineEmissiveMap,
                glm::vec3(1.0f), 1.0f, 1.0f, glm::vec3(0.0f));

        Engine::Entity* submarineEntity = new Engine::Entity();
        Engine::ModelRenderer* submarineRenderer = new Engine::ModelRenderer(submarineMaterial, submarineModel);
        submarineEntity->AddComponent(submarineRenderer);
        Engine::ShipRoller* submarineRoller = new Engine::ShipRoller(glm::vec3(1.0f, 1.0f, 5.0f),
                                                                     glm::vec3(0.2f, 0.1f, 0.4f));
        submarineEntity->AddComponent(submarineRoller);
        submarineEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());

        Textures.push_back(Engine::Texture(submarineBaseMap));
        Textures.push_back(Engine::Texture(submarineRmaoMap));
        Textures.push_back(Engine::Texture(submarineNormalMap));
        Textures.push_back(Engine::Texture(submarineEmissiveMap));

        Models.push_back(submarineModel);
        Materials.push_back(submarineMaterial);

        /*
         * Assembling Water
         */

        unsigned int waterNormal0 = Engine::TextureManager::GetTexture("./res/textures/Water/Normal0.png").GetId();
        unsigned int waterNormal1 = Engine::TextureManager::GetTexture(
                "./res/textures/Water/Normal1.png").GetId();


        Models::Model* waterModel = Models::ModelManager::GetModel("./res/models/Water.fbx");
        Materials::WaterMaterial* waterMaterial = new Materials::WaterMaterial(waterNormal0,
                                                                               waterNormal1,
                                                                               glm::vec3(0.02745f, 0.2156f, 0.3843f),
                                                                               glm::vec2(0.32f), glm::vec2(0.112f),
                                                                               glm::vec2(0.07f), glm::vec2(-0.03f),
                                                                               0.02f, 0.04f);

        Engine::Entity* waterEntity = new Engine::Entity();
        Engine::ModelRenderer* waterRenderer = new Engine::ModelRenderer(waterMaterial, waterModel);
        waterEntity->AddComponent(waterRenderer);
        waterEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());

        Textures.push_back(Engine::Texture(waterNormal0));
        Textures.push_back(Engine::Texture(waterNormal1));

        Models.push_back(waterModel);
        Materials.push_back(waterMaterial);

        /*
         * Assembling Floor
         */

        unsigned int floorBaseMap = Engine::TextureManager::GetTexture("./res/textures/Floor/Base.png").GetId();
        unsigned int floorRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Floor/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int floorNormalMap = Engine::TextureManager::GetTexture("./res/textures/Floor/Normal.png").GetId();

        unsigned int floorEmissiveMap = Engine::TextureManager::GetTexture("./res/textures/EmissiveMapDefault.png").
                GetId();

        Models::Model* floorModel = Models::ModelManager::GetModel("./res/models/Floor.fbx");
        Materials::PbrMaterial* floorMaterial = new Materials::PbrMaterial(floorBaseMap,
                                                                           floorRmaoMap, floorNormalMap,
                                                                           floorEmissiveMap,
                                                                           glm::vec3(1.0f), 1.0f, 1.0f,
                                                                           glm::vec3(0.0f));

        Engine::Entity* floorEntity = new Engine::Entity();
        Engine::ModelRenderer* floorRenderer = new Engine::ModelRenderer(floorMaterial, floorModel);
        floorEntity->AddComponent(floorRenderer);
        floorEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());

        Textures.push_back(Engine::Texture(floorBaseMap));
        Textures.push_back(Engine::Texture(floorRmaoMap));
        Textures.push_back(Engine::Texture(floorNormalMap));
        Textures.push_back(Engine::Texture(floorEmissiveMap));

        Models.push_back(floorModel);
        Materials.push_back(floorMaterial);

        /*
         * Assembling Walls
         */

        unsigned int wallsBaseMap = Engine::TextureManager::GetTexture("./res/textures/Walls/Base.png").GetId();
        unsigned int wallsRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Walls/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int wallsNormalMap = Engine::TextureManager::GetTexture("./res/textures/Walls/Normal.png").GetId();

        unsigned int wallsEmissiveMap = Engine::TextureManager::GetTexture("./res/textures/EmissiveMapDefault.png").
                GetId();

        Models::Model* wallsModel = Models::ModelManager::GetModel("./res/models/Walls.fbx");
        Materials::PbrMaterial* wallsMaterial = new Materials::PbrMaterial(wallsBaseMap,
                                                                           wallsRmaoMap, wallsNormalMap,
                                                                           wallsEmissiveMap,
                                                                           glm::vec3(1.0f), 1.0f, 1.0f,
                                                                           glm::vec3(0.0f));

        Engine::Entity* wallsEntity = new Engine::Entity();
        Engine::ModelRenderer* wallsRenderer = new Engine::ModelRenderer(wallsMaterial, wallsModel);
        wallsEntity->AddComponent(wallsRenderer);
        wallsEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());

        Textures.push_back(Engine::Texture(wallsBaseMap));
        Textures.push_back(Engine::Texture(wallsRmaoMap));
        Textures.push_back(Engine::Texture(wallsNormalMap));
        Textures.push_back(Engine::Texture(wallsEmissiveMap));

        Models.push_back(wallsModel);
        Materials.push_back(wallsMaterial);

        /*
         * Assembling Boxes
         */

        unsigned int boxBaseMap = Engine::TextureManager::GetTexture("./res/textures/Box/Base.png").GetId();
        unsigned int boxRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Box/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int boxNormalMap = Engine::TextureManager::GetTexture("./res/textures/Box/Normal.png").GetId();

        unsigned int boxEmissiveMap = Engine::TextureManager::GetTexture("./res/textures/EmissiveMapDefault.png").
                GetId();

        Models::Model* boxModel = Models::ModelManager::GetModel("./res/models/Box.fbx");
        Materials::PbrMaterial* boxMaterial = new Materials::PbrMaterial(boxBaseMap,
                                                                         boxRmaoMap, boxNormalMap,
                                                                         boxEmissiveMap,
                                                                         glm::vec3(1.0f), 1.0f, 1.0f, glm::vec3(0.0f));

        Engine::Entity* boxEntity = new Engine::Entity();
        Engine::ModelRenderer* boxRenderer = new Engine::ModelRenderer(boxMaterial, boxModel);
        boxEntity->AddComponent(boxRenderer);
        boxEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        boxEntity->GetTransform()->SetPosition(glm::vec3(-13.57f, 2.2008f, -5.9495f));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, -32.9f, 0.0f));

        boxEntity = new Engine::Entity();
        boxRenderer = new Engine::ModelRenderer(boxMaterial, boxModel);
        boxEntity->AddComponent(boxRenderer);
        boxEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        boxEntity->GetTransform()->SetPosition(glm::vec3(-13.6264f, 2.18668f, -2.95381));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 38.8818f, 0.0f));

        boxEntity = new Engine::Entity();
        boxRenderer = new Engine::ModelRenderer(boxMaterial, boxModel);
        boxEntity->AddComponent(boxRenderer);
        boxEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        boxEntity->GetTransform()->SetPosition(glm::vec3(13.2187, 2.20102f, 8.33215f));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, -32.9f, 0.0f));

        boxEntity = new Engine::Entity();
        boxRenderer = new Engine::ModelRenderer(boxMaterial, boxModel);
        boxEntity->AddComponent(boxRenderer);
        boxEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        boxEntity->GetTransform()->SetPosition(glm::vec3(13.6425, 2.18046f, 5.98336f));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, -32.9f, 0.0f));

        boxEntity = new Engine::Entity();
        boxRenderer = new Engine::ModelRenderer(boxMaterial, boxModel);
        boxEntity->AddComponent(boxRenderer);
        boxEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        boxEntity->GetTransform()->SetPosition(glm::vec3(11.3158, 2.20102f, 7.18511f));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, -32.9f, 0.0f));

        boxEntity = new Engine::Entity();
        boxRenderer = new Engine::ModelRenderer(boxMaterial, boxModel);
        boxEntity->AddComponent(boxRenderer);
        boxEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        boxEntity->GetTransform()->SetPosition(glm::vec3(12.626, 4.20103f, 7.20247f));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, -32.9f, 0.0f));

        boxEntity = new Engine::Entity();
        boxRenderer = new Engine::ModelRenderer(boxMaterial, boxModel);
        boxEntity->AddComponent(boxRenderer);
        boxEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        boxEntity->GetTransform()->SetPosition(glm::vec3(18.6972, 2.18039f, -11.6444f));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, -32.9f, 0.0f));

        boxEntity = new Engine::Entity();
        boxRenderer = new Engine::ModelRenderer(boxMaterial, boxModel);
        boxEntity->AddComponent(boxRenderer);
        boxEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        boxEntity->GetTransform()->SetPosition(glm::vec3(16.6849, 2.18039f, -9.5844f));
        boxEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, -77.4803f, 0.0f));


        Textures.push_back(Engine::Texture(boxBaseMap));
        Textures.push_back(Engine::Texture(boxRmaoMap));
        Textures.push_back(Engine::Texture(boxNormalMap));
        Textures.push_back(Engine::Texture(boxEmissiveMap));

        Models.push_back(boxModel);
        Materials.push_back(boxMaterial);

        /*
         * Assemble Fan
         */

        unsigned int fanBaseBaseMap = Engine::TextureManager::GetTexture("./res/textures/Fan/Base/Base.png").GetId();
        unsigned int fanBaseRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Fan/Base/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int fanBaseNormalMap = Engine::TextureManager::GetTexture("./res/textures/Fan/Base/Normal.png").
                GetId();

        unsigned int fanBaseEmissiveMap = Engine::TextureManager::GetTexture("./res/textures/EmissiveMapDefault.png").
                GetId();

        Models::Model* fanBaseModel = Models::ModelManager::GetModel("./res/models/Fan/Base.fbx");
        Materials::PbrMaterial* fanBaseMaterial = new Materials::PbrMaterial(fanBaseBaseMap,
                                                                             fanBaseRmaoMap, fanBaseNormalMap,
                                                                             fanBaseEmissiveMap,
                                                                             glm::vec3(1.0f), 1.0f, 1.0f,
                                                                             glm::vec3(0.0f));

        Engine::Entity* fanBaseEntity = new Engine::Entity();
        Engine::ModelRenderer* fanBaseRenderer = new Engine::ModelRenderer(fanBaseMaterial, fanBaseModel);
        fanBaseEntity->AddComponent(fanBaseRenderer);
        fanBaseEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        fanBaseEntity->GetTransform()->SetPosition(glm::vec3(-5.39749f, 1.2595f, -18.3669));
        fanBaseEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 31.2321f, 0.0f));

        Textures.push_back(Engine::Texture(fanBaseBaseMap));
        Textures.push_back(Engine::Texture(fanBaseRmaoMap));
        Textures.push_back(Engine::Texture(fanBaseNormalMap));
        Textures.push_back(Engine::Texture(fanBaseEmissiveMap));

        Models.push_back(fanBaseModel);
        Materials.push_back(fanBaseMaterial);

        unsigned int fanMotorBaseMap = Engine::TextureManager::GetTexture("./res/textures/Fan/Motor/Base.png").GetId();
        unsigned int fanMotorRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Fan/Motor/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int fanMotorNormalMap = Engine::TextureManager::GetTexture("./res/textures/Fan/Motor/Normal.png").
                GetId();

        unsigned int fanMotorEmissiveMap = Engine::TextureManager::GetTexture("./res/textures/EmissiveMapDefault.png").
                GetId();

        Models::Model* fanMotorModel = Models::ModelManager::GetModel("./res/models/Fan/Motor.fbx");
        Materials::PbrMaterial* fanMotorMaterial = new Materials::PbrMaterial(fanMotorBaseMap,
                                                                              fanMotorRmaoMap, fanMotorNormalMap,
                                                                              fanMotorEmissiveMap,
                                                                              glm::vec3(1.0f), 1.0f, 1.0f,
                                                                              glm::vec3(0.0f));

        Engine::Entity* fanMotorEntity = new Engine::Entity();
        Engine::ModelRenderer* fanMotorRenderer = new Engine::ModelRenderer(fanMotorMaterial, fanMotorModel);
        fanMotorEntity->AddComponent(fanMotorRenderer);
        fanMotorEntity->GetTransform()->SetParent(fanBaseEntity->GetTransform());
        fanMotorEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 1.58632f, 0.026547f));
        fanMotorEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, -31.2321, 0.0f));
        Engine::ShipRoller* fanMotorRoller = new Engine::ShipRoller(glm::vec3(0.0f, 45.0f, 0.0f),
                                                                    glm::vec3(0.0f, 0.625f, 0.0f));
        fanMotorRoller->SetInitialRotation(fanMotorEntity->GetTransform()->GetEulerAngles());
        fanMotorEntity->AddComponent(fanMotorRoller);

        Textures.push_back(Engine::Texture(fanMotorBaseMap));
        Textures.push_back(Engine::Texture(fanMotorRmaoMap));
        Textures.push_back(Engine::Texture(fanMotorNormalMap));
        Textures.push_back(Engine::Texture(fanMotorEmissiveMap));

        Models.push_back(fanMotorModel);
        Materials.push_back(fanMotorMaterial);

        unsigned int fanBladesBaseMap = Engine::TextureManager::GetTexture("./res/textures/Fan/Blades/Base.png").
                GetId();
        unsigned int fanBladesRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Fan/Blades/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int fanBladesNormalMap = Engine::TextureManager::GetTexture("./res/textures/Fan/Blades/Normal.png").
                GetId();

        unsigned int fanBladesEmissiveMap = Engine::TextureManager::GetTexture("./res/textures/EmissiveMapDefault.png").
                GetId();

        Models::Model* fanBladesModel = Models::ModelManager::GetModel("./res/models/Fan/Blades.fbx");
        Materials::PbrMaterial* fanBladesMaterial = new Materials::PbrMaterial(fanBladesBaseMap,
                                                                               fanBladesRmaoMap, fanBladesNormalMap,
                                                                               fanBladesEmissiveMap,
                                                                               glm::vec3(1.0f), 1.0f, 1.0f,
                                                                               glm::vec3(0.0f));

        Engine::Entity* fanBladesEntity = new Engine::Entity();
        Engine::ModelRenderer* fanBladesRenderer = new Engine::ModelRenderer(fanBladesMaterial, fanBladesModel);
        fanBladesEntity->AddComponent(fanBladesRenderer);
        fanBladesEntity->GetTransform()->SetParent(fanMotorEntity->GetTransform());
        fanBladesEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 0.058523f));
        fanBladesEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, 180.0f, 0.0f));
        Engine::Rotator* fanBladesRotator = new Engine::Rotator(glm::vec3(0.0f, 0.0f, 720.0f));
        fanBladesEntity->AddComponent(fanBladesRotator);

        Textures.push_back(Engine::Texture(fanBladesBaseMap));
        Textures.push_back(Engine::Texture(fanBladesRmaoMap));
        Textures.push_back(Engine::Texture(fanBladesNormalMap));
        Textures.push_back(Engine::Texture(fanBladesEmissiveMap));

        Models.push_back(fanBladesModel);
        Materials.push_back(fanBladesMaterial);

        /*
         * Assembling Table
         */

        unsigned int tableBaseMap = Engine::TextureManager::GetTexture("./res/textures/Table/Base.png").GetId();
        unsigned int tableRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Table/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int tableNormalMap = Engine::TextureManager::GetTexture("./res/textures/Table/Normal.png").GetId();

        unsigned int tableEmissiveMap = Engine::TextureManager::GetTexture("./res/textures/EmissiveMapDefault.png").
                GetId();

        Models::Model* tableModel = Models::ModelManager::GetModel("./res/models/Table.fbx");
        Materials::PbrMaterial* tableMaterial = new Materials::PbrMaterial(tableBaseMap,
                                                                           tableRmaoMap, tableNormalMap,
                                                                           tableEmissiveMap,
                                                                           glm::vec3(1.0f), 1.0f, 1.0f,
                                                                           glm::vec3(0.0f));

        Engine::Entity* tableEntity = new Engine::Entity();
        Engine::ModelRenderer* tableRenderer = new Engine::ModelRenderer(tableMaterial, tableModel);
        tableEntity->AddComponent(tableRenderer);
        tableEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        tableEntity->GetTransform()->SetPosition(glm::vec3(-8.19799f, 1.65611f, -18.3789f));

        tableEntity = new Engine::Entity();
        tableRenderer = new Engine::ModelRenderer(tableMaterial, tableModel);
        tableEntity->AddComponent(tableRenderer);
        tableEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        tableEntity->GetTransform()->SetPosition(glm::vec3(30.0707f, 1.65611f, 25.2889f));
        tableEntity->GetTransform()->SetEulerAngles(glm::vec3(0.0f, -90.0f, 0.0f));

        Textures.push_back(Engine::Texture(tableBaseMap));
        Textures.push_back(Engine::Texture(tableRmaoMap));
        Textures.push_back(Engine::Texture(tableNormalMap));
        Textures.push_back(Engine::Texture(tableEmissiveMap));

        Models.push_back(tableModel);
        Materials.push_back(tableMaterial);

        /*
         * Assembling Bottles
         */

        Models::Model* bottleModel = Models::ModelManager::GetModel("./res/models/Bottle.fbx");
        Materials::ReflectiveMaterial* bottleReflectiveMaterial = new Materials::ReflectiveMaterial(environmentMap);

        Engine::Entity* bottleEntity = new Engine::Entity();
        Engine::ModelRenderer* bottleRenderer = new Engine::ModelRenderer(bottleReflectiveMaterial, bottleModel);
        bottleEntity->AddComponent(bottleRenderer);
        bottleEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        bottleEntity->GetTransform()->SetPosition(glm::vec3(29.9154f, 2.20356f, 25.0271f));

        Materials::RefractiveMaterial* bottleRefractiveMaterial = new Materials::RefractiveMaterial(
                environmentMap, 1.52f);

        bottleEntity = new Engine::Entity();
        bottleRenderer = new Engine::ModelRenderer(bottleRefractiveMaterial, bottleModel);
        bottleEntity->AddComponent(bottleRenderer);
        bottleEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        bottleEntity->GetTransform()->SetPosition(glm::vec3(30.1237f, 2.20356f, 25.6987f));

        Models.push_back(bottleModel);
        Materials.push_back(bottleRefractiveMaterial);
        Materials.push_back(bottleReflectiveMaterial);

        /*
         * Assembling Monitor
         */

        unsigned int monitorBaseMap = Engine::TextureManager::GetTexture("./res/textures/Monitor/Base.png").GetId();
        unsigned int monitorRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/Monitor/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int monitorNormalMap = Engine::TextureManager::GetTexture("./res/textures/Monitor/Normal.png").GetId();

        unsigned int monitorEmissiveMap = Engine::TextureManager::GetTexture("./res/textures/Monitor/Emissive.png").
                GetId();

        Models::Model* monitorModel = Models::ModelManager::GetModel("./res/models/Monitor.fbx");
        Materials::PbrMaterial* monitorMaterial = new Materials::PbrMaterial(monitorBaseMap,
                                                                             monitorRmaoMap, monitorNormalMap,
                                                                             monitorEmissiveMap,
                                                                             glm::vec3(1.0f), 1.0f, 1.0f,
                                                                             glm::vec3(0.0f, 3.155f, 5.0f));

        Engine::Entity* monitorEntity = new Engine::Entity();
        Engine::ModelRenderer* monitorRenderer = new Engine::ModelRenderer(monitorMaterial, monitorModel);
        monitorEntity->AddComponent(monitorRenderer);
        monitorEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        monitorEntity->GetTransform()->SetPosition(glm::vec3(-8.11747, 2.28526, -18.5948));

        Textures.push_back(Engine::Texture(monitorBaseMap));
        Textures.push_back(Engine::Texture(monitorRmaoMap));
        Textures.push_back(Engine::Texture(monitorNormalMap));
        Textures.push_back(Engine::Texture(monitorEmissiveMap));

        Models.push_back(monitorModel);
        Materials.push_back(monitorMaterial);

        /*
         * Wall Monitor
         */

        unsigned int wallMonitorBaseMap = Engine::TextureManager::GetTexture("./res/textures/WallMonitor/Base.png").
                GetId();
        unsigned int wallMonitorRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/WallMonitor/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int wallMonitorNormalMap = Engine::TextureManager::GetTexture("./res/textures/WallMonitor/Normal.png").
                GetId();

        unsigned int wallMonitorEmissiveMap = Engine::TextureManager::GetTexture(
                "./res/textures/WallMonitor/Emissive.png").GetId();

        Models::Model* wallMonitorModel = Models::ModelManager::GetModel("./res/models/WallMonitor.fbx");
        Materials::PbrMaterial* wallMonitorMaterial = new Materials::PbrMaterial(
                wallMonitorBaseMap,
                wallMonitorRmaoMap, wallMonitorNormalMap,
                wallMonitorEmissiveMap,
                glm::vec3(1.0f), 1.0f, 1.0f,
                glm::vec3(0.0f, 3.155f, 5.0f));

        Engine::Entity* wallMonitorEntity = new Engine::Entity();
        Engine::ModelRenderer* wallMonitorRenderer = new Engine::ModelRenderer(wallMonitorMaterial, wallMonitorModel);
        wallMonitorEntity->AddComponent(wallMonitorRenderer);
        wallMonitorEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        wallMonitorEntity->GetTransform()->SetPosition(glm::vec3(-6.88389f, 5.599f, -19.0169f));

        Textures.push_back(Engine::Texture(wallMonitorBaseMap));
        Textures.push_back(Engine::Texture(wallMonitorRmaoMap));
        Textures.push_back(Engine::Texture(wallMonitorNormalMap));
        Textures.push_back(Engine::Texture(wallMonitorEmissiveMap));

        Models.push_back(wallMonitorModel);
        Materials.push_back(wallMonitorMaterial);

        /*
         * Ceiling Lamps
         */

        unsigned int ceilingLampBaseMap = Engine::TextureManager::GetTexture("./res/textures/CeilingLamp/Base.png").
                GetId();
        unsigned int ceilingLampRmaoMap = Engine::TextureManager::GetTexture(
                "./res/textures/CeilingLamp/RoughnessMetallicAmbientOcclusion.png").GetId();

        unsigned int ceilingLampNormalMap = Engine::TextureManager::GetTexture("./res/textures/CeilingLamp/Normal.png").
                GetId();

        unsigned int ceilingLampEmissiveMap = Engine::TextureManager::GetTexture(
                "./res/textures/CeilingLamp/Emissive.png").GetId();

        Models::Model* ceilingLampModel = Models::ModelManager::GetModel("./res/models/CeilingLamp.fbx");
        Materials::PbrMaterial* ceilingLampMaterial = new Materials::PbrMaterial(
                ceilingLampBaseMap,
                ceilingLampRmaoMap, ceilingLampNormalMap,
                ceilingLampEmissiveMap,
                glm::vec3(1.0f), 1.0f, 1.0f,
                glm::vec3(10.0));

        Engine::Entity* ceilingLampEntity = new Engine::Entity();
        Engine::ModelRenderer* ceilingLampRenderer = new Engine::ModelRenderer(ceilingLampMaterial, ceilingLampModel);
        ceilingLampEntity->AddComponent(ceilingLampRenderer);
        ceilingLampEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        ceilingLampEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 10.8041f, -5.17208f));

        ceilingLampEntity = new Engine::Entity();
        ceilingLampRenderer = new Engine::ModelRenderer(ceilingLampMaterial, ceilingLampModel);
        ceilingLampEntity->AddComponent(ceilingLampRenderer);
        ceilingLampEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        ceilingLampEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 10.8041f, 7.12522f));

        Textures.push_back(Engine::Texture(ceilingLampBaseMap));
        Textures.push_back(Engine::Texture(ceilingLampRmaoMap));
        Textures.push_back(Engine::Texture(ceilingLampNormalMap));
        Textures.push_back(Engine::Texture(ceilingLampEmissiveMap));

        Models.push_back(ceilingLampModel);
        Materials.push_back(ceilingLampMaterial);


        /*
         * Sun
         */

        Engine::DirectionalLight* light = new Engine::DirectionalLight();
        light->SetColor(glm::vec3(15.0f, 10.65f, 6.3f));
        Engine::Entity* entity = new Engine::Entity();
        entity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        entity->GetTransform()->SetEulerAngles(glm::vec3(-15.0f, 232.0f, 0.0f));
        entity->AddComponent(light);

        /*
         * Monitor Light
         */

        Engine::PointLight* pointLight = new Engine::PointLight();
        pointLight->SetColor(glm::vec3(0.0f, 6.310f, 10.0f));
        Engine::Entity* pointLightEntity = new Engine::Entity();
        pointLightEntity->GetTransform()->SetPosition(glm::vec3(-8.092f, 2.295f, -18.579f));
        pointLightEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        pointLightEntity->AddComponent(pointLight);

        /*
         * WallMonitor Light
         */
        Engine::PointLight* pointLight2 = new Engine::PointLight();
        pointLight2->SetColor(glm::vec3(0.0f, 6.310f, 10.0f));
        Engine::Entity* pointLightEntity2 = new Engine::Entity();
        pointLightEntity2->GetTransform()->SetPosition(glm::vec3(-6.88389f, 5.599f, -18.454f));
        pointLightEntity2->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        pointLightEntity2->AddComponent(pointLight2);


        /*
         * Ceiling Lights
         */
        Engine::SpotLight* spotLight = new Engine::SpotLight();
        spotLight->SetColor(glm::vec3(100.0f, 100.0f, 100.0f));
        spotLight->SetInnerAngle(15.0f);
        spotLight->SetOuterAngle(25.0f);
        spotLight->SetQuadraticFalloff(0.4f);
        Engine::Entity* spotLightEntity = new Engine::Entity();
        spotLightEntity->GetTransform()->SetPosition(glm::vec3(0.0f, 10.73f, -5.17208f));
        spotLightEntity->GetTransform()->SetEulerAngles(glm::vec3(-90.0f, 0.0f, 0.0f));
        spotLightEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        spotLightEntity->AddComponent(spotLight);

        Engine::SpotLight* spotLight2 = new Engine::SpotLight();
        spotLight2->SetColor(glm::vec3(100.0f, 100.0f, 100.0f));
        spotLight2->SetInnerAngle(25.0f);
        spotLight2->SetOuterAngle(30.0f);
        spotLight->SetQuadraticFalloff(0.4f);
        Engine::Entity* spotLightEntity2 = new Engine::Entity();
        spotLightEntity2->GetTransform()->SetPosition(glm::vec3(0.0f, 10.74f, 7.12522f));
        spotLightEntity2->GetTransform()->SetEulerAngles(glm::vec3(-90.0f, 0.0f, 0.0f));
        spotLightEntity2->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        spotLightEntity2->AddComponent(spotLight2);

        /*
         * Skybox
         */

        Models::Model* skyboxModel = Models::ModelManager::GetModel("./res/models/SkySphere.fbx");

        Materials::SkyboxMaterial* skyboxMaterial = new Materials::SkyboxMaterial(environmentMap);
        Engine::Entity* skybox = new Engine::Entity();
        skybox->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        Engine::ModelRenderer* skyboxRenderer = new Engine::ModelRenderer(skyboxMaterial, skyboxModel);
        skybox->AddComponent(skyboxRenderer);

        Models.push_back(skyboxModel);
        Materials.push_back(skyboxMaterial);

        /*
         * Particles
         */

        Models::Model* sphereModel = Models::ModelManager::GetModel("./res/models/SphereLowPoly.fbx");

        Engine::ParticleEmitter::EmitterSettings emitterSettings(1000.0f,
                                                                 sphereModel,
                                                                 glm::vec4(1.0f),
                                                                 glm::vec4(1.0f),
                                                                 glm::vec3(-26.0f, 1.0f, -23.0f),
                                                                 glm::vec3(26.0f, 13.0f, 15.0f),
                                                                 glm::vec3(-0.25f, -0.25f, -0.25f),
                                                                 glm::vec3(0.25f, 0.25f, 0.25f),
                                                                 glm::vec3(0.01f),
                                                                 glm::vec3(0.01f),
                                                                 glm::vec3(-0.05f),
                                                                 glm::vec3(0.05f),
                                                                 10.0f,
                                                                 15.0f);


        Engine::Entity* particleEntity = new Engine::Entity();
        particleEntity->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
        Engine::ParticleEmitter* particleEmitter = new Engine::ParticleEmitter(particleRenderShader,
                                                                               particleSpawnShader,
                                                                               particleUpdateShader, emitterSettings,
                                                                               10000);
        particleEntity->AddComponent(particleEmitter);

        Models.push_back(sphereModel);
    }
} // Scene
