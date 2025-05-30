#pragma once

#include <string>

#include "Materials/MaterialRaii.h" //Used in SERIALIZATION_EXPORT_MATERIAL
#include "rapidjson/document.h"
#include "Shaders/Shader.h"

#if EDITOR
#include "Engine/Textures/TextureManager.h"
#include "imgui.h"
#include <filesystem>
#include "Engine/Textures/Texture.h"
#include "Properties/TextureMaterialProperty.h"
#endif


#define SERIALIZATION_EXPORT_MATERIAL(__CLASS__)\
public:\
    static inline const std::string TypeName =std::string(#__CLASS__);\
    \
    rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const override;\
    \
    void Deserialize(const rapidjson::Value& Object) override;\
    \
    [[nodiscard]] std::string GetType() const override\
    {\
        return TypeName;\
    }\
    \
    static void Initialize();\
    \
private:\
    static inline const MaterialRaii<__CLASS__> RaiiHandle =  MaterialRaii<__CLASS__>(#__CLASS__);

#define START_MATERIAL_SERIALIZATION rapidjson::Value object(rapidjson::kObjectType);\
                                     object.AddMember("type", Serialization::Serialize(GetType(), Allocator), Allocator);

#define END_MATERIAL_SERIALIZATION  return object;

#define START_MATERIAL_DESERIALIZATION

#define END_MATERIAL_DESERIALIZATION

#define SERIALIZE_PROPERTY(__NAME__) object.AddMember(#__NAME__, Serialization::Serialize(__NAME__, Allocator), Allocator);

#define DESERIALIZE_PROPERTY(__NAME__) Serialization::Deserialize(Object, #__NAME__, __NAME__);

namespace Materials
{
    /**
     * @brief Base class for all materials.
     */
    class Material
    {
    private:
        const Shaders::Shader DepthPass;
        const Shaders::Shader MainPass;
        const Shaders::Shader DirectionalShadowPass;
        const Shaders::Shader PointSpotShadowPass;

        bool Transparent;

    public:
        Material(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                 const Shaders::Shader& DirectionalShadowPass, const Shaders::Shader& PointSpotShadowPass) :
            DepthPass(DepthPass), MainPass(Shader), DirectionalShadowPass(DirectionalShadowPass),
            PointSpotShadowPass(PointSpotShadowPass), Transparent(false)
        {
        }

        Material(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                 const Shaders::Shader& DirectionalShadowPass, const Shaders::Shader& PointSpotShadowPass,
                 bool Transparent) :
            DepthPass(DepthPass), MainPass(Shader), DirectionalShadowPass(DirectionalShadowPass),
            PointSpotShadowPass(PointSpotShadowPass), Transparent(Transparent)
        {
        }

    public:
        inline virtual ~Material() = default;

    public:
        /**
         * @brief Returns depth pass shader used by this material.
         */
        [[nodiscard]] const Shaders::Shader& GetDepthPass() const
        {
            return DepthPass;
        }

        /**
         * @brief Returns main pass shader used by this material.
         */
        [[nodiscard]] const Shaders::Shader& GetMainPass() const
        {
            return MainPass;
        }

        /**
         * @brief Returns point light shadow pass shader used by this material.
         */
        [[nodiscard]] const Shaders::Shader& GetPointSpotShadowPass() const
        {
            return PointSpotShadowPass;
        }

        /**
         * @brief Returns directional light shadow pass shader used by this material.
         */
        [[nodiscard]] const Shaders::Shader& GetDirectionalShadowPass() const
        {
            return DirectionalShadowPass;
        }

        [[nodiscard]] bool IsTransparent() const
        {
            return Transparent;
        }

        /**
         * @brief Returns class name of this material.
         */
        [[nodiscard]] virtual std::string GetType() const = 0;

        /**
         * @brief Binds depth pass and its uniforms used by this material to be used in the next draw call.
         */
        virtual void UseDepthPass() const = 0;

        /**
         * @brief Binds main pass and its uniforms used by this material to be used in the next draw call.
         */
        virtual void Use() const = 0;

        /**
         * @brief Binds point light shadow pass and its uniforms used by this material to be used in the next draw call.
         */
        virtual void UseDirectionalShadows() const = 0;

        /**
         * @brief Binds directional light shadow pass and its uniforms used by this material to be used in the next draw call.
         */
        virtual void UsePointSpotShadows() const = 0;

        /**
         * @brief Saves this material's properties to a JSON.
         * @param Allocator An allocator to be used.
         * @return Serialized data.
         */
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const = 0;

        /**
         * @brief Sets this material's properties from JSON.
         * @param Value Data to load properties from.
         */
        virtual void Deserialize(const rapidjson::Value& Value) = 0;
#if EDITOR
        virtual void DrawImGui() = 0;

        virtual void EditorReadMaterial(std::vector<std::string>& availableTextures, bool& scanned,
                                        const std::string& texturePath)
        {
            if (!scanned)
            {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(texturePath))
                {
                    if (entry.is_regular_file() && entry.path().extension() == ".dds")
                    {
                        std::string relPath = entry.path().lexically_normal().string();
                        std::replace(relPath.begin(), relPath.end(), '\\', '/');
                        size_t pos = relPath.find("/res/");
                        if (pos != std::string::npos)
                            relPath = "." + relPath.substr(pos);
                        availableTextures.emplace_back(relPath);
                    }
                }
                scanned = true;
            }
        };

        virtual void EditorSetMaterial(const char* label, TextureMaterialProperty& materialProp, bool& showPopup,
                                       const std::vector<std::string>& availableTextures,
                                       const std::string& texturePath, const char* popupLabel)
        {
            std::string currentPath = materialProp.GetValue().GetId() != 0
                                          ? Engine::TextureManager::GetTexturePath(materialProp.GetValue())
                                          : "None";

            ImGui::Text("%s", label);
            ImGui::Selectable(currentPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                {
                    const char* droppedPath = static_cast<const char*>(payload->Data);
                    if (std::filesystem::path(droppedPath).extension() == ".dds")
                    {
                        materialProp.SetValue(Engine::TextureManager::GetTexture(droppedPath));
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::IsItemClicked())
                showPopup = true;

            if (showPopup)
            {
                ImGui::OpenPopup(popupLabel);
                showPopup = false;
            }

            if (ImGui::BeginPopup(popupLabel))
            {
                for (const auto& path : availableTextures)
                {
                    std::filesystem::path fsPath(path);
                    std::string displayName = std::filesystem::relative(fsPath, texturePath).string();
                    if (ImGui::Selectable(displayName.c_str()))
                    {
                        materialProp.SetValue(Engine::TextureManager::GetTexture(path.c_str()));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    ImGui::TextDisabled("(%s)", path.c_str());
                }
                ImGui::EndPopup();
            }
        };
#endif
    };
}
