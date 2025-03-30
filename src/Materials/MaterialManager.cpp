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
    std::unordered_map<std::string, Material*> MaterialManager::Materials;

    Material* MaterialManager::GetMaterial(const std::string& Path)
    {
        if (const auto iterator = Materials.find(Path); iterator != Materials.end())
        {
            return iterator->second;
        }

        Material* material = LoadMaterialFromFile(Path);
        Materials.emplace(Path, material);
        return material;
    }

    bool MaterialManager::DeleteMaterial(const std::string& Path)
    {
        if (const auto iterator = Materials.find(Path); iterator != Materials.end())
        {
            delete iterator->second;
            Materials.erase(Path);
            return true;
        }
        return false;
    }

    bool MaterialManager::DeleteMaterial(Material* const Material)
    {
        for (const auto& pair : Materials)
        {
            if (pair.second == Material)
            {
                delete Material;
                Materials.erase(pair.first);
                return true;
            }
        }
        return false;
    }

    void MaterialManager::DeleteAllMaterials()
    {
        for (const auto& pair : Materials)
        {
            delete pair.second;
        }
        Materials.clear();
    }

    std::string MaterialManager::GetMaterialPath(const Material* const Material)
    {
        for (const auto& pair : Materials)
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
#if DEBUG || EDITOR
        const auto iterator = Materials.find(Path);
        CHECK_MESSAGE(iterator != Materials.end(), "Material was not loaded from a file")
        SaveMaterial(Path, iterator->second);
#else
        SaveMaterial(Path, Materials[Path]);
#endif
    }

    void MaterialManager::SaveMaterial(const Material* const Material)
    {
        for (const auto& pair : Materials)
        {
            if (pair.second == Material)
            {
                SaveMaterial(pair.first, pair.second);
                return;
            }
        }
#if DEBUG || EDITOR
        CHECK_MESSAGE(false, "Material was not loaded from a file")
#endif
    }

    void MaterialManager::SaveMaterial(const std::string& Path, const Material* const Material)
    {
        rapidjson::MemoryPoolAllocator<> allocator;
        rapidjson::Value json = Material->Serialize(allocator);
        Serialization::WriteJsonFile(Path.c_str(), json);
    }

    Material* MaterialManager::LoadMaterialFromFile(const std::string& Path)
    {
        rapidjson::Value data = Serialization::ReadJsonFile(Path.c_str());
        Material* material = DetermineMaterialType(data);
        if (material == nullptr)
        {
            return nullptr;
        }
        material->Deserialize(data);
        return material;
    }

    Material* MaterialManager::DetermineMaterialType(rapidjson::Value& Json)
    {
        const std::string typeId = Json["type"].GetString();

        if (CheckTypeId<PbrMaterial>(typeId))
        {
            return new PbrMaterial();
        }
        if (CheckTypeId<ReflectiveMaterial>(typeId))
        {
            return new ReflectiveMaterial();
        }
        if (CheckTypeId<RefractiveMaterial>(typeId))
        {
            return new RefractiveMaterial();
        }
        if (CheckTypeId<SkyboxMaterial>(typeId))
        {
            return new SkyboxMaterial();
        }
        if (CheckTypeId<WaterMaterial>(typeId))
        {
            return new WaterMaterial();
        }
        return nullptr;
    }
} // Materials
