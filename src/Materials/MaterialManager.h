#pragma once
#include <string>
#include <unordered_map>

#include "Serialization/SerializationFilesUtility.h"

namespace Materials
{
    class Material;

    class MaterialManager
    {
    private:
        class IMaterialBuilder
        {
        public:
            virtual ~IMaterialBuilder() = default;

            [[nodiscard]] virtual Material* Build() const = 0;
        };

        template<class T>
        class MaterialBuilder final : public IMaterialBuilder
        {
        public:
            [[nodiscard]] Material* Build() const override
            {
                return new T();
            }
        };

    private:
        [[nodiscard]] static std::unordered_map<std::string, Material*>& GetMaterials()
        {
            static std::unordered_map<std::string, Material*> materials;
            return materials;
        }

    public:
        class MaterialFactory final
        {
            std::unordered_map<std::string, IMaterialBuilder*> MaterialBuilders;

        public:
            template<class T>
            void Register(const std::string& Name)
            {
                MaterialBuilders.emplace(Name, new MaterialBuilder<T>());
            }

            void Unregister(const std::string& Name)
            {
                MaterialBuilders.erase(Name);
            }

            [[nodiscard]] const IMaterialBuilder* GetBuilder(const std::string& Name)
            {
                const auto iterator = MaterialBuilders.find(Name);
                if (iterator == MaterialBuilders.end())
                {
                    return nullptr;
                }
                return iterator->second;
            }
        };

    public:
        [[nodiscard]] static MaterialFactory* GetMaterialFactory()
        {
            static MaterialFactory factory;
            return &factory;
        }

    private:
        MaterialManager() = default;

    public
    :
        static Material* GetMaterial(const std::string& Path);

        static bool DeleteMaterial(const std::string& Path);

        static bool DeleteMaterial(Material* Material);

        static void DeleteAllMaterials();

        static std::string GetMaterialPath(const Material* Material);

        static void SaveMaterial(const std::string& Path);

        static void SaveMaterial(const Material* Material);

        static void SaveMaterial(const std::string& Path, const Material* Material);

        static void Initialize();

        template<class T>
        static T* GetMaterial(const std::string& Path)
        {
            static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
            Material* material = GetMaterial(Path);
#if DEBUG
            T* result = dynamic_cast<T*>(material);
            CHECK_MESSAGE(result != nullptr, "Wrong material type or material not found.");
            return result;
#else
            T* result = dynamic_cast<T*>(material);
            return result;
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

    private
    :
        static Material* LoadMaterialFromFile(const std::string& Path);

        static Material* DetermineMaterialType(const rapidjson::Value& Json);

        template<class T>
        static bool CheckTypeId(const std::string TypeId)
        {
            static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
            return T::TypeName == TypeId;
        }
    };

} // Materials
