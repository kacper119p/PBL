#pragma once
#include <string>
#include <unordered_map>

#include "Material.h"
#include "Serialization/SerializationFilesUtility.h"

namespace Materials
{
    class MaterialManager
    {
    private:
        static std::unordered_map<std::string, Material*> Materials;

    private:
        MaterialManager() = default;

    public:
        static Material* GetMaterial(const std::string& Path);

        static bool DeleteMaterial(const std::string& Path);

        static bool DeleteMaterial(Material* Material);

        static void DeleteAllMaterials();

        static std::string GetMaterialPath(const Material* Material);

        template<class T>
        static Material* GetMaterial(const std::string& Path)
        {
            static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
            Material* material = GetMaterial(Path);
#if EDITOR || DEBUG
            T* result = dynamic_cast<T*>(material);
            assert(result != nullptr);
            return result;
#else
            return reinterpret_cast<T*>(material);
#endif

        }

        template<class T>
        static Material* CreateMaterialAsset(const std::string& Path)
        {
            static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");

            T* material = new T();
            rapidjson::MemoryPoolAllocator<> allocator;
            rapidjson::Value& materialJson = material->Serialize(allocator);
            Serialization::WriteJsonFile(Path.c_str(), materialJson);
            return material;
        }

    private:
        static Material* LoadMaterialFromFile(const std::string& Path);

        static Material* DetermineMaterialType(rapidjson::Value& Json);

        template<class T>
        static bool CheckTypeId(const std::string TypeId)
        {
            static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
            return std::strcmp(TypeId.c_str(), typeid(T).name()) == 0;
        }
    };
} // Materials
