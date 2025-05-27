#include "GizmoMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    Shaders::Shader GizmoMaterial::Shader;
    Shaders::Shader GizmoMaterial::DepthShader;

    void GizmoMaterial::UseDepthPass() const
    {
    }

    void GizmoMaterial::Use() const
    {
        Shader.Use();
    }

    void GizmoMaterial::UseDirectionalShadows() const
    {
    }

    void GizmoMaterial::UsePointSpotShadows() const
    {
    }

#if EDITOR
    void GizmoMaterial::DrawImGui()
    {
    }
#endif

    rapidjson::Value GizmoMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void GizmoMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }

    void GizmoMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles(
                        "./res/shaders/Gizmo/Gizmo.vert",
                        nullptr,
                        "./res/shaders/Gizmo/Gizmo.frag"));
        DepthShader = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles(
                        "./res/shaders/Gizmo/Gizmo.vert",
                        nullptr,
                        "./res/shaders/Gizmo/Depth.frag"));
    }
} // Materials
