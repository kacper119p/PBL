#include "MaterialManager.h"

#include "PbrMaterial.h"
#include "ReflectiveMaterial.h"
#include "RefractiveMaterial.h"
#include "SkyboxMaterial.h"
#include "WaterMaterial.h"
#include "Serialization/SerializationFilesUtility.h"
#include "Utility/AssertionsUtility.h"

namespace Materials
{
    Material* MaterialManager::GetMaterial(const std::string& Path)
    {
        if (const auto iterator = GetMaterials().find(Path); iterator != GetMaterials().end())
        {
            return iterator->second;
        }

        Material* material = LoadMaterialFromFile(Path);
        GetMaterials().emplace(Path, material);
        return material;
    }

    bool MaterialManager::DeleteMaterial(const std::string& Path)
    {
        if (const auto iterator = GetMaterials().find(Path); iterator != GetMaterials().end())
        {
            delete iterator->second;
            GetMaterials().erase(Path);
            return true;
        }
        return false;
    }

    bool MaterialManager::DeleteMaterial(Material* const Material)
    {
        for (const auto& pair : GetMaterials())
        {
            if (pair.second == Material)
            {
                delete Material;
                GetMaterials().erase(pair.first);
                return true;
            }
        }
        return false;
    }

    void MaterialManager::DeleteAllMaterials()
    {
        for (const auto& pair : GetMaterials())
        {
            delete pair.second;
        }
        GetMaterials().clear();
    }

    std::string MaterialManager::GetMaterialPath(const Material* const Material)
    {
        for (const auto& pair : GetMaterials())
        {
            if (pair.second == Material)
            {
                return pair.first;
            }
        }
        return std::string();
    }

    void MaterialManager::SaveMaterial(const std::string& Path)
    {
#if DEBUG
        const auto iterator = Materials.find(Path);
        CHECK_MESSAGE(iterator != Materials.end(), "Material was not loaded from a file")
        SaveMaterial(Path, iterator->second);
#else
        SaveMaterial(Path, GetMaterials()[Path]);
#endif
    }

    void MaterialManager::SaveMaterial(const Material* const Material)
    {
        for (const auto& pair : GetMaterials())
        {
            if (pair.second == Material)
            {
                SaveMaterial(pair.first, pair.second);
                return;
            }
        }
        CHECK_MESSAGE(false, "Material was not loaded from a file")
    }

    void MaterialManager::SaveMaterial(const std::string& Path, const Material* const Material)
    {
        rapidjson::MemoryPoolAllocator<> allocator;
        rapidjson::Value json = Material->Serialize(allocator);
        Serialization::WriteJsonFile(Path.c_str(), json);
    }

    void MaterialManager::Initialize()
    {
        PbrMaterial::Initialize();;
        ReflectiveMaterial::Initialize();
        RefractiveMaterial::Initialize();
        SkyboxMaterial::Initialize();
        WaterMaterial::Initialize();
    }

    Material* MaterialManager::LoadMaterialFromFile(const std::string& Path)
    {
        rapidjson::Document data;
        Serialization::ReadJsonFile(Path.c_str(), data);
        Material* material = DetermineMaterialType(data);
        if (material == nullptr)
        {
            return nullptr;
        }
        material->Deserialize(data);
        return material;
    }

    Material* MaterialManager::DetermineMaterialType(const rapidjson::Value& Json)
    {
        const std::string typeId = Json.FindMember("type")->value.GetString();
        const IMaterialBuilder* const builder = GetMaterialFactory()->GetBuilder(typeId);
        if (builder == nullptr)
        {
            return nullptr;
        }

        return builder->Build();
    }
} // Materials
