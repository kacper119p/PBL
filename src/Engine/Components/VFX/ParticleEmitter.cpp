#include "ParticleEmitter.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/LightManager.h"


#include "Engine/EngineObjects/RenderingManager.h"
#include "Serialization/SerializationUtility.h"

Engine::ParticleEmitter::ParticleEmitter(Materials::Material* const Material, const Shaders::ComputeShader& SpawnShader,
                                         const Shaders::ComputeShader& UpdateShader) :
    Material(Material),
    SpawnShader(SpawnShader),
    UpdateShader(UpdateShader)
{
    SetMaterial(Material);
}

Engine::ParticleEmitter::~ParticleEmitter() = default;

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
