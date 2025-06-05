#include "ParticleEmitter.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/EngineObjects/LightManager.h"
#include <algorithm>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "Engine/EngineObjects/RenderingManager.h"
#include "Serialization/SerializationUtility.h"

Engine::ParticleEmitter::ParticleEmitter(Materials::Material* const Material, const Shaders::ComputeShader& SpawnShader,
                                         const Shaders::ComputeShader& UpdateShader,
                                         const EmitterSettings& EmitterSettings, const int MaxParticleCount) :
    Settings(EmitterSettings), MaxParticleCount(MaxParticleCount), SpawnShader(SpawnShader),
    UpdateShader(UpdateShader)
{
    this->Material = Material;
    ParticlesToSpawnProperty = SpawnShader.GetUniformLocation("ParticlesToSpawn");
    RandomProperty = SpawnShader.GetUniformLocation("Random");
    DeltaTimeProperty = UpdateShader.GetUniformLocation("DeltaTime");
}

void Engine::ParticleEmitter::Render(const Engine::CameraRenderData& RenderData)
{
    SetupMatrices(RenderData, Material->GetMainPass());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ParticlesBuffer);
    for (int i = 0; i < Settings.Model->GetMeshCount(); ++i)
    {
        Models::Mesh* mesh = Settings.Model->GetMesh(i);
        glBindVertexArray(mesh->GetVertexArray());
        glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->GetFaceCount(), MaxParticleCount);
        glBindVertexArray(0);
    }
}

void Engine::ParticleEmitter::SetMaterial(Materials::Material* Material)
{
    if (Material == this->Material)
    {
        return;
    }
    RenderingManager::GetInstance()->UnregisterParticleEmitter(this);
    this->Material = Material;
    if (Material == nullptr)
    {
        return;
    }
    RenderingManager::GetInstance()->RegisterParticleEmitter(this);
}

void Engine::ParticleEmitter::Start()
{
    if (Material != nullptr)
    {
        RenderingManager::GetInstance()->RegisterParticleEmitter(this);
    }

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

void Engine::ParticleEmitter::DispatchSpawnShaders(const float DeltaTime)
{
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

        Shaders::ComputeShader::SetUniform(ParticlesToSpawnProperty, particlesToSpawn);
        float time = static_cast<float>(glfwGetTime());
        Shaders::ComputeShader::SetUniform(RandomProperty, *reinterpret_cast<unsigned int*>(&time));

        Shaders::ComputeShader::Dispatch(glm::ivec3(workGroupsCount, 1, 1));
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

void Engine::ParticleEmitter::DispatchUpdateShaders(const float DeltaTime)
{
    UpdateShader.Use();
    Shaders::ComputeShader::SetUniform(DeltaTimeProperty, DeltaTime);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, FreelistBuffer);

    const int workGroupSize = UpdateShader.GetWorkGroupSize().x;
    const int workGroupsCount = (MaxParticleCount + workGroupSize - 1) / workGroupSize;
    Shaders::ComputeShader::Dispatch(glm::ivec3(workGroupsCount, 1, 1));
}

Engine::ParticleEmitter::~ParticleEmitter()
{
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
#if EDITOR
void Engine::ParticleEmitter::DrawImGui()
{
    if (ImGui::CollapsingHeader("Emitter Settings"))
    {
        ImGui::SliderFloat("Spawn Rate", &Settings.SpawnRate, 0.0f, 1000.0f);

        ImGui::ColorEdit4("Min Color", glm::value_ptr(Settings.MinColor));
        ImGui::ColorEdit4("Max Color", glm::value_ptr(Settings.MaxColor));

        ImGui::DragFloat3("Min Offset", glm::value_ptr(Settings.MinOffset), 0.1f);
        ImGui::DragFloat3("Max Offset", glm::value_ptr(Settings.MaxOffset), 0.1f);

        ImGui::DragFloat3("Min Velocity", glm::value_ptr(Settings.MinVelocity), 0.1f);
        ImGui::DragFloat3("Max Velocity", glm::value_ptr(Settings.MaxVelocity), 0.1f);

        ImGui::DragFloat3("Min Scale", glm::value_ptr(Settings.MinScale), 0.1f);
        ImGui::DragFloat3("Max Scale", glm::value_ptr(Settings.MaxScale), 0.1f);

        ImGui::DragFloat3("Min Accel", glm::value_ptr(Settings.MinAccel), 0.1f);
        ImGui::DragFloat3("Max Accel", glm::value_ptr(Settings.MaxAccel), 0.1f);

        ImGui::SliderFloat("Min Life", &Settings.MinLife, 0.0f, 30.0f);
        ImGui::SliderFloat("Max Life", &Settings.MaxLife, 0.0f, 30.0f);
    }

    if (ImGui::CollapsingHeader("Particle System Control"))
    {
        ImGui::InputInt("Max Particles", &MaxParticleCount);
        ImGui::Text("Current Timer: %.2f", Timer);
    }
}
#endif
rapidjson::Value Engine::ParticleEmitter::Serialize(rapidjson::Document::AllocatorType& Allocator) const
{
    START_COMPONENT_SERIALIZATION
    SERIALIZE_FIELD(MaxParticleCount)
    SERIALIZE_FIELD(Settings.SpawnRate)
    SERIALIZE_FIELD(Settings.Model)
    SERIALIZE_FIELD(Settings.MinColor)
    SERIALIZE_FIELD(Settings.MaxColor)
    SERIALIZE_FIELD(Settings.MinOffset)
    SERIALIZE_FIELD(Settings.MaxOffset)
    SERIALIZE_FIELD(Settings.MinVelocity)
    SERIALIZE_FIELD(Settings.MaxVelocity)
    SERIALIZE_FIELD(Settings.MinScale)
    SERIALIZE_FIELD(Settings.MaxScale)
    SERIALIZE_FIELD(Settings.MinAccel)
    SERIALIZE_FIELD(Settings.MaxAccel)
    SERIALIZE_FIELD(Settings.MinLife)
    SERIALIZE_FIELD(Settings.MaxLife)
    SERIALIZE_FIELD(Material)
    SERIALIZE_FIELD(SpawnShader)
    SERIALIZE_FIELD(UpdateShader)
    END_COMPONENT_SERIALIZATION
}

void Engine::ParticleEmitter::DeserializeValuePass(const rapidjson::Value& Object,
                                                   Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_VALUE_PASS
    DESERIALIZE_VALUE(MaxParticleCount)
    DESERIALIZE_VALUE(Settings.SpawnRate)
    DESERIALIZE_VALUE(Settings.Model)
    DESERIALIZE_VALUE(Settings.MinColor)
    DESERIALIZE_VALUE(Settings.MaxColor)
    DESERIALIZE_VALUE(Settings.MinOffset)
    DESERIALIZE_VALUE(Settings.MaxOffset)
    DESERIALIZE_VALUE(Settings.MinVelocity)
    DESERIALIZE_VALUE(Settings.MaxVelocity)
    DESERIALIZE_VALUE(Settings.MinScale)
    DESERIALIZE_VALUE(Settings.MaxScale)
    DESERIALIZE_VALUE(Settings.MinAccel)
    DESERIALIZE_VALUE(Settings.MaxAccel)
    DESERIALIZE_VALUE(Settings.MinLife)
    DESERIALIZE_VALUE(Settings.MaxLife)
    DESERIALIZE_VALUE(Material)
    DESERIALIZE_VALUE(SpawnShader)
    DESERIALIZE_VALUE(UpdateShader)
    ParticlesToSpawnProperty = SpawnShader.GetUniformLocation("ParticlesToSpawn");
    RandomProperty = SpawnShader.GetUniformLocation("Random");
    DeltaTimeProperty = UpdateShader.GetUniformLocation("DeltaTime");
    END_COMPONENT_DESERIALIZATION_VALUE_PASS
}

void Engine::ParticleEmitter::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                        Serialization::ReferenceTable& ReferenceMap)
{
    START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
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
