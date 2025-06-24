#include "VacuumVfx.h"

#include "GLFW/glfw3.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/Textures/TextureManager.h"
#include "Math/Easings.h"
#include "Math/Math.h"
#include "Models/ModelManager.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    VacuumVfx::VacuumVfx()
    {
        Material = Materials::MaterialManager::GetMaterial("res/materials/VFX/Vacuum.mat");
        TimeLocation = Material->GetMainPass().GetUniformLocation("Time");
        TextureLocation = Material->GetMainPass().GetUniformLocation("Texture");

        CameraLocationLocation = Material->GetMainPass().GetUniformLocation("CameraLocation");
        ViewMatrixLocation = Material->GetMainPass().GetUniformLocation("ViewMatrix");
        ProjectionMatrixLocation = Material->GetMainPass().GetUniformLocation("ProjectionMatrix");
        ObjectToWorldMatrixLocation = Material->GetMainPass().GetUniformLocation("ObjectToWorldMatrix");

        VisibilityLocation = Material->GetMainPass().GetUniformLocation("Visibility");

        InnerTexture = TextureManager::GetTexture("res/textures/VFX/Vacuum/VacuumInner.dds").GetHandleReadonly();
        OuterTexture = TextureManager::GetTexture("res/textures/VFX/Vacuum/VacuumOuter.dds").GetHandleReadonly();

        Model = Models::ModelManager::GetModel("res/models/VFX/Vacuum/VacuumVFX.glb");
    }

    VacuumVfx::~VacuumVfx()
    {
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }
#if EDITOR
    void VacuumVfx::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Vacuum VFX"))
        {
            ImGui::Text(
                    "THE COMPONENT IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE COMPONENT OR THE USE OR OTHER DEALINGS IN THE COMPONENT.");

        }
    }
#endif
    void VacuumVfx::Start()
    {
        Renderer::Start();
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    void VacuumVfx::Update(const float DeltaTime)
    {
        Time += (Active ? TransitionSpeed : -TransitionSpeed) * DeltaTime;
        Time = Math::Clamp(Time, 0.0f, 1.0f);
    }

    void VacuumVfx::RenderDepth(const CameraRenderData& RenderData)
    {
    }

    void VacuumVfx::Render(const CameraRenderData& RenderData)
    {
        const float size = Math::EaseOutQuart(Time);

        Shaders::Shader::SetUniform(TimeLocation, static_cast<float>(glfwGetTime()) * NoiseSpeed);

        Shaders::Shader::SetUniform(CameraLocationLocation, RenderData.CameraPosition);
        Shaders::Shader::SetUniform(ViewMatrixLocation, RenderData.ViewMatrix);
        Shaders::Shader::SetUniform(ProjectionMatrixLocation, RenderData.ProjectionMatrix);
        Shaders::Shader::SetUniform(VisibilityLocation, size);


        const glm::mat4& objectToWorld = GetOwner()->GetTransform()->GetLocalToWorldMatrix();
        Shaders::Shader::SetUniform(ObjectToWorldMatrixLocation, objectToWorld);

        const Models::Mesh* mesh = Model->GetMesh(0);

        Shaders::Shader::SetTextureHandle(TextureLocation, InnerTexture);
        mesh->Draw();

        const glm::mat4 outerScaledMatrix = glm::scale(objectToWorld, glm::vec3(1.1f));
        Shaders::Shader::SetUniform(ObjectToWorldMatrixLocation, outerScaledMatrix);
        Shaders::Shader::SetTextureHandle(TextureLocation, OuterTexture);
        mesh->Draw();
        glEnable(GL_CULL_FACE);
    }

    void VacuumVfx::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
    }

    void VacuumVfx::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                           const glm::mat4* SpaceTransformMatrices)
    {
    }

    inline rapidjson::Value VacuumVfx::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        END_COMPONENT_SERIALIZATION
    }

    void VacuumVfx::DeserializeValuePass(const rapidjson::Value& Object,
                                         Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void VacuumVfx::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
}
