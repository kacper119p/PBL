#include "ParticleEmitter.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/EngineObjects/LightManager.h"
#include <algorithm>
#include <GLFW/glfw3.h>

Engine::ParticleEmitter::ParticleEmitter(const Shaders::Shader& RenderShader, const Shaders::ComputeShader& SpawnShader,
                                         const Shaders::ComputeShader& UpdateShader,
                                         const EmitterSettings& EmitterSettings,
                                         const int MaxParticleCount) :
    Settings(EmitterSettings), MaxParticleCount(MaxParticleCount), RenderShader(RenderShader),
    SpawnShader(SpawnShader),
    UpdateShader(UpdateShader)
{
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

void Engine::ParticleEmitter::Render(const Engine::CameraRenderData& RenderData)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    RenderShader.Use();
    LightManager::GetInstance()->SetupLightsForRendering(RenderShader);
    SetupMatrices(RenderData, RenderShader);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
    for (int i = 0; i < Settings.Model->GetMeshCount(); ++i)
    {
        Models::Mesh* mesh = Settings.Model->GetMesh(i);
        glBindVertexArray(mesh->GetVertexArray());
        glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->GetFaceCount(), MaxParticleCount);
        glBindVertexArray(0);
    }
    glDisable(GL_BLEND);
}

void Engine::ParticleEmitter::RenderDepth(const CameraRenderData& RenderData)
{
}

void Engine::ParticleEmitter::RenderDirectionalShadows(const Engine::CameraRenderData& RenderData)
{
}

void Engine::ParticleEmitter::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                                     const glm::mat4* SpaceTransformMatrices)
{
}

void Engine::ParticleEmitter::Update(float DeltaTime)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    Timer += DeltaTime;

    const int particlesToSpawn = std::min(static_cast<int>(Timer * Settings.SpawnRate), MaxParticleCount);
    Timer -= particlesToSpawn / Settings.SpawnRate;

    if (particlesToSpawn > 0)
    {
        SpawnShader.Use();
        SetEmitterSettingsUniforms(SpawnShader);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, FreelistBuffer);

        const int workGroupSize = SpawnShader.GetWorkGroupSize().x;
        const int workGroupsCount = (particlesToSpawn + workGroupSize - 1) / workGroupSize;

        SpawnShader.SetUniform("ParticlesToSpawn", particlesToSpawn);
        float time = static_cast<float>(glfwGetTime());
        SpawnShader.SetUniform("Random", *reinterpret_cast<unsigned int*>(&time));

        Shaders::ComputeShader::Dispatch(glm::ivec3(workGroupsCount, 1, 1));
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    UpdateShader.Use();
    UpdateShader.SetUniform("DeltaTime", DeltaTime);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, FreelistBuffer);

    const int workGroupSize = UpdateShader.GetWorkGroupSize().x;
    const int workGroupsCount = (MaxParticleCount + workGroupSize - 1) / workGroupSize;
    Shaders::ComputeShader::Dispatch(glm::ivec3(workGroupsCount, 1, 1));
}

void Engine::ParticleEmitter::OnAdd(Engine::Entity* NewOwner)
{
    Renderer::OnAdd(NewOwner);
    UpdateManager::GetInstance()->RegisterComponent(this);
}

Engine::ParticleEmitter::~ParticleEmitter()
{
    UpdateManager::GetInstance()->UnregisterComponent(this);
}

void Engine::ParticleEmitter::SetupMatrices(const Engine::CameraRenderData& RenderData,
                                            const Shaders::Shader& Shader) const
{
    {
        Shader.SetUniform("CameraPosition", RenderData.CameraPosition);
        Shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        Shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Shader.SetUniform("ObjectToWorldMatrix",
                          GetOwner()->GetTransform()->GetLocalToWorldMatrix());
    }
}

void Engine::ParticleEmitter::SetEmitterSettingsUniforms(Shaders::ComputeShader Shader) const
{
    Shader.SetUniform("EmitterSettings.MinColor", Settings.MinColor);
    Shader.SetUniform("EmitterSettings.MaxColor", Settings.MaxColor);
    Shader.SetUniform("EmitterSettings.MinOffset", Settings.MinOffset);
    Shader.SetUniform("EmitterSettings.MaxOffset", Settings.MaxOffset);
    Shader.SetUniform("EmitterSettings.MinVelocity", Settings.MinVelocity);
    Shader.SetUniform("EmitterSettings.MaxVelocity", Settings.MaxVelocity);
    Shader.SetUniform("EmitterSettings.MinScale", Settings.MinScale);
    Shader.SetUniform("EmitterSettings.MaxScale", Settings.MaxScale);
    Shader.SetUniform("EmitterSettings.MinAccel", Settings.MinAccel);
    Shader.SetUniform("EmitterSettings.MaxAccel", Settings.MaxAccel);
    Shader.SetUniform("EmitterSettings.MinLife", Settings.MinLife);
    Shader.SetUniform("EmitterSettings.MaxLife", Settings.MaxLife);
}

Engine::ParticleEmitter::EmitterSettings::EmitterSettings(const float SpawnRate, Models::Model* Model,
                                                          const glm::vec4& MinColor, const glm::vec4& MaxColor,
                                                          const glm::vec3& MinOffset, const glm::vec3& MaxOffset,
                                                          const glm::vec3& MinVelocity, const glm::vec3& MaxVelocity,
                                                          const glm::vec3& MinScale, const glm::vec3& MaxScale,
                                                          const glm::vec3& MinAccel, const glm::vec3& MaxAccel,
                                                          const float MinLife, const float MaxLife) :
    SpawnRate(SpawnRate),
    Model(Model),
    MinColor(MinColor),
    MaxColor(MaxColor),
    MinOffset(MinOffset),
    MaxOffset(MaxOffset),
    MinVelocity(MinVelocity),
    MaxVelocity(MaxVelocity),
    MinScale(MinScale),
    MaxScale(MaxScale),
    MinAccel(MinAccel),
    MaxAccel(MaxAccel),
    MinLife(MinLife),
    MaxLife(MaxLife)
{
}
