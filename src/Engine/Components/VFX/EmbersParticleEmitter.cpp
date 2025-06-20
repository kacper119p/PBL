#include "EmbersParticleEmitter.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/LightManager.h"
#include <algorithm>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "Engine/EngineObjects/RenderingManager.h"
#include "Engine/Rendering/SpriteQuad.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"

Engine::EmbersParticleEmitter::EmbersParticleEmitter() :
    ParticleEmitter(Materials::MaterialManager::GetMaterial("res/materials/VFX/Embers.mat"),
                    Shaders::ShaderManager::GetComputeShader("res/shaders/VFX/Fire/Embers/EmbersSpawn.comp"),
                    Shaders::ShaderManager::GetComputeShader("res/shaders/VFX/Fire/Embers/EmbersUpdate.comp")),
    ParticlesToSpawnProperty(SpawnShader.GetUniformLocation("ParticlesToSpawn")),
    DeltaTimeProperty(UpdateShader.GetUniformLocation("DeltaTime")),
    RandomProperty(SpawnShader.GetUniformLocation("Random")),
    ViewMatrixLocation(Material->GetMainPass().GetUniformLocation("ViewMatrix")),
    ProjectionMatrixLocation(Material->GetMainPass().GetUniformLocation("ProjectionMatrix")),
    ObjectToWorldMatrixLocation(Material->GetMainPass().GetUniformLocation("ObjectToWorldMatrix"))
{
}

Engine::EmbersParticleEmitter::~EmbersParticleEmitter()
{
    glDeleteBuffers(1, &ParticlesBuffer);
    glDeleteBuffers(1, &FreelistBuffer);
}

void Engine::EmbersParticleEmitter::Render(const Engine::CameraRenderData& RenderData)
{
    Shaders::Shader::SetUniform(ViewMatrixLocation, RenderData.ViewMatrix);
    Shaders::Shader::SetUniform(ProjectionMatrixLocation, RenderData.ProjectionMatrix);
    Shaders::Shader::SetUniform(ObjectToWorldMatrixLocation, GetOwner()->GetTransform()->GetLocalToWorldMatrix());

    SetupMatrices(RenderData, Material->GetMainPass());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ParticlesBuffer);
    SpriteQuad::DrawInstanced(MaxParticleCount);
}

void Engine::EmbersParticleEmitter::Start()
{
    ParticleEmitter::Start();

    glGenBuffers(1, &ParticlesBuffer);
    glGenBuffers(1, &FreelistBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticlesBuffer);

    const Particle* particles = new Particle[MaxParticleCount]{};
    glBufferData(GL_SHADER_STORAGE_BUFFER, MaxParticleCount * sizeof(Particle), particles, GL_DYNAMIC_DRAW);
    delete[] particles;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, FreelistBuffer);
    int* freeList = new int[MaxParticleCount + 1]{0};
    freeList[0] = MaxParticleCount;
    for (int i = 1; i <= MaxParticleCount; ++i)
    {
        freeList[i] = i - 1;
    }
    glBufferData(GL_SHADER_STORAGE_BUFFER, (MaxParticleCount + 1) * sizeof(int), freeList, GL_DYNAMIC_DRAW);
    delete[] freeList;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Engine::EmbersParticleEmitter::DispatchSpawnShaders(const float DeltaTime)
{
    Timer += DeltaTime;

    const int particlesToSpawn = std::min(static_cast<int>(Timer * SpawnRate), MaxParticleCount);
    Timer -= static_cast<float>(particlesToSpawn) / SpawnRate;

    if (particlesToSpawn > 0)
    {
        SpawnShader.Use();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, FreelistBuffer);

        constexpr int workGroupSize = 64;
        const int workGroupsCount = (particlesToSpawn + workGroupSize - 1) / workGroupSize;

        Shaders::ComputeShader::SetUniform(ParticlesToSpawnProperty, particlesToSpawn);
        float time = static_cast<float>(glfwGetTime());
        Shaders::ComputeShader::SetUniform(RandomProperty, *reinterpret_cast<unsigned int*>(&time));

        Shaders::ComputeShader::Dispatch(glm::ivec3(workGroupsCount, 1, 1));
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

void Engine::EmbersParticleEmitter::DispatchUpdateShaders(const float DeltaTime)
{
    UpdateShader.Use();
    Shaders::ComputeShader::SetUniform(DeltaTimeProperty, DeltaTime);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, FreelistBuffer);

    constexpr int workGroupSize = 64;
    constexpr int workGroupsCount = (MaxParticleCount + workGroupSize - 1) / workGroupSize;
    Shaders::ComputeShader::Dispatch(glm::ivec3(workGroupsCount, 1, 1));
}

#if EDITOR
void Engine::EmbersParticleEmitter::DrawImGui()
{
    if (ImGui::CollapsingHeader("Embers Particle Emitter"))
    {
    }
}
#endif
rapidjson::Value Engine::EmbersParticleEmitter::Serialize(rapidjson::Document::AllocatorType& Allocator) const
{
    START_COMPONENT_SERIALIZATION
    END_COMPONENT_SERIALIZATION
}

void Engine::EmbersParticleEmitter::DeserializeValuePass(const rapidjson::Value& Object,
                                                         Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_VALUE_PASS
    END_COMPONENT_DESERIALIZATION_VALUE_PASS
}

void Engine::EmbersParticleEmitter::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                              Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
}
