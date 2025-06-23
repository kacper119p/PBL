#include "FireParticleEmitter.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/LightManager.h"
#include <algorithm>
#include <imgui.h>
#include <GLFW/glfw3.h>

#include "Engine/EngineObjects/RenderingManager.h"
#include "Engine/Rendering/SpriteQuad.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"

Engine::FireParticleEmitter::FireParticleEmitter() :
    ParticleEmitter(Materials::MaterialManager::GetMaterial("res/materials/VFX/Flame.mat"),
                    Shaders::ShaderManager::GetComputeShader("res/shaders/VFX/Fire/Flame/FlameSpawn.comp"),
                    Shaders::ShaderManager::GetComputeShader("res/shaders/VFX/Fire/Flame/FlameUpdate.comp")),
    ParticlesToSpawnProperty(SpawnShader.GetUniformLocation("ParticlesToSpawn")),
    DeltaTimeProperty(UpdateShader.GetUniformLocation("DeltaTime")),
    RandomProperty(SpawnShader.GetUniformLocation("Random")),
    ViewMatrixLocation(Material->GetMainPass().GetUniformLocation("ViewMatrix")),
    ProjectionMatrixLocation(Material->GetMainPass().GetUniformLocation("ProjectionMatrix")),
    ObjectToWorldMatrixLocation(Material->GetMainPass().GetUniformLocation("ObjectToWorldMatrix"))
{
}

Engine::FireParticleEmitter::~FireParticleEmitter()
{
    glDeleteBuffers(1, &ParticlesBuffer);
    glDeleteBuffers(1, &FreelistBuffer);
    glDeleteBuffers(1, &SortedIndicesBuffer);
}

void Engine::FireParticleEmitter::Render(const Engine::CameraRenderData& RenderData)
{
    Shaders::Shader::SetUniform(ViewMatrixLocation, RenderData.ViewMatrix);
    Shaders::Shader::SetUniform(ProjectionMatrixLocation, RenderData.ProjectionMatrix);
    Shaders::Shader::SetUniform(ObjectToWorldMatrixLocation, GetOwner()->GetTransform()->GetLocalToWorldMatrix());

    SetupMatrices(RenderData, Material->GetMainPass());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ParticlesBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SortedIndicesBuffer);
    SpriteQuad::DrawInstanced(MaxParticleCount);
}

void Engine::FireParticleEmitter::Start()
{
    ParticleEmitter::Start();

    glGenBuffers(1, &ParticlesBuffer);
    glGenBuffers(1, &FreelistBuffer);
    glGenBuffers(1, &SortedIndicesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticlesBuffer);

    const Particle* particles = new Particle[MaxParticleCount]{};
    glBufferData(GL_SHADER_STORAGE_BUFFER, MaxParticleCount * sizeof(Particle), particles, GL_DYNAMIC_DRAW);
    delete[] particles;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, FreelistBuffer);
    int* freeList = new int[MaxParticleCount + 1];
    freeList[0] = MaxParticleCount;
    for (int i = 1; i <= MaxParticleCount; ++i)
    {
        freeList[i] = i - 1;

    }
    glBufferData(GL_SHADER_STORAGE_BUFFER, (MaxParticleCount + 1) * sizeof(int), freeList, GL_DYNAMIC_DRAW);
    delete[] freeList;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SortedIndicesBuffer);
    const int* sortedIndices = new int[MaxParticleCount];
    glBufferData(GL_SHADER_STORAGE_BUFFER, (MaxParticleCount) * sizeof(int), sortedIndices, GL_DYNAMIC_DRAW);
    delete[] sortedIndices;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Engine::FireParticleEmitter::DispatchSpawnShaders(const float DeltaTime)
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

void Engine::FireParticleEmitter::DispatchUpdateShaders(const float DeltaTime)
{
    UpdateShader.Use();
    Shaders::ComputeShader::SetUniform(DeltaTimeProperty, DeltaTime);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, FreelistBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SortedIndicesBuffer);

    constexpr int workGroupSize = 64;
    constexpr int workGroupsCount = (MaxParticleCount + workGroupSize - 1) / workGroupSize;
    Shaders::ComputeShader::Dispatch(glm::ivec3(workGroupsCount, 1, 1));
}

#if EDITOR
void Engine::FireParticleEmitter::DrawImGui()
{
    if (ImGui::CollapsingHeader("Fire Particle Emitter"))
    {
    }
}
#endif
rapidjson::Value Engine::FireParticleEmitter::Serialize(rapidjson::Document::AllocatorType& Allocator) const
{
    START_COMPONENT_SERIALIZATION
    END_COMPONENT_SERIALIZATION
}

void Engine::FireParticleEmitter::DeserializeValuePass(const rapidjson::Value& Object,
                                                       Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_VALUE_PASS
    END_COMPONENT_DESERIALIZATION_VALUE_PASS
}

void Engine::FireParticleEmitter::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                            Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
}
