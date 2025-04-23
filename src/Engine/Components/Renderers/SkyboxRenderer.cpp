#include "SkyboxRenderer.h"

#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    void SkyboxRenderer::RenderDepth(const CameraRenderData& RenderData)
    {
    }

    void SkyboxRenderer::Render(const CameraRenderData& RenderData)
    {
        Material->Use();
        const Shaders::Shader shader = GetMaterial()->GetMainPass();
        shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Cube.Draw();
    }

    void SkyboxRenderer::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
    }

    void SkyboxRenderer::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                                const glm::mat4* SpaceTransformMatrices)
    {
    }

    rapidjson::Value SkyboxRenderer::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Material)
        END_COMPONENT_SERIALIZATION
    }

    void SkyboxRenderer::DeserializeValuePass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Material)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void SkyboxRenderer::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                   Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
