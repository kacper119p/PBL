#pragma once
#include <algorithm>
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

        class IMaterialInitializer
        {
        public:
            virtual ~IMaterialInitializer() = default;

            virtual void Initialize() const = 0;
        };

        template<class T>
        class MaterialInitializer final : public IMaterialInitializer
        {
        public:
            void Initialize() const override
            {
                T::Initialize();
            }
        };

    private:
        [[nodiscard]] static std::unordered_map<std::string, Material*>& GetMaterials()
        {
            static std::unordered_map<std::string, Material*> materials;
            return materials;
        }

    public:
        /**
         * @brief Factory used in deserialization of materials.
         */
        class MaterialFactory final
        {
        private:
            std::unordered_map<std::string, IMaterialBuilder*> MaterialBuilders;
#if EDITOR
            std::vector<std::string> MaterialTypes;
#endif

        public:
            /**
             * @brief Registers a new material class in the factory.
             * @tparam T Material class to be registered.
             * @param Name TypeName of a material class.
             */
            template<class T>
            void Register(const std::string& Name)
            {
                MaterialBuilders.emplace(Name, new MaterialBuilder<T>());
#if EDITOR
                if constexpr (std::is_base_of_v<Material, T> && !std::is_abstract_v<T>)
                {
                    MaterialTypes.emplace_back(Name);
                    std::ranges::sort(MaterialTypes);
                }
#endif
            }

            /**
             * @brief
             * @param Name TypeName of a material class.
             */
            void Unregister(const std::string& Name)
            {
                MaterialBuilders.erase(Name);
#if EDITOR
                std::erase(MaterialTypes, Name);
#endif
            }

            /**
             * @brief Returns a Material builder for a given material class based on TypeName.
             * @param Name Material's TypeName.
             * @return A Material builder.
             */
            [[nodiscard]] const IMaterialBuilder* GetBuilder(const std::string& Name)
            {
                const auto iterator = MaterialBuilders.find(Name);
                if (iterator == MaterialBuilders.end())
                {
                    return nullptr;
                }
                return iterator->second;
            }
#if EDITOR
            /**
             * @brief Returns TypeNames of all available components.
             */
        public:
            std::vector<std::string> GetAvailableMaterialTypes() const
            {
                return MaterialTypes;
            }
#endif
        };

        /**
         * @brief Used to initialize material's shaders.
         */
        class MaterialInitializerManager final
        {
        private:
            std::vector<IMaterialInitializer*> MaterialInitializers;

        public:
            /**
             * @brief Initialize materials.
             */
            void Initialize() const
            {
                for (const IMaterialInitializer* initializer : MaterialInitializers)
                {
                    initializer->Initialize();
                }
            }

            /**
             * @brief Register a new material to a initializer.
             * @tparam T Material's class.
             */
            template<class T>
            void Register()
            {
                static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
                MaterialInitializers.emplace_back(new MaterialInitializer<T>());
            }

            /**
             * @brief Unregisters material class from a initializer.
             * @tparam T Material's class.
             */
            template<class T>
            void Unregister()
            {
                static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
                for (IMaterialInitializer* initializer : MaterialInitializers)
                {
                    if (dynamic_cast<MaterialInitializer<T>*>(initializer))
                    {
                        std::erase(MaterialInitializers, initializer);
                        return;
                    }
                }
            }
        };

    public:
        /**
         * @brief Returns the MaterialFactory singleton.
         */
        [[nodiscard]] static MaterialFactory* GetMaterialFactory()
        {
            static MaterialFactory factory;
            return &factory;
        }

        /**
         * @brief Returns the MaterialInitializer singleton.
         */
        [[nodiscard]] static MaterialInitializerManager& GetMaterialInitializer()
        {
            static MaterialInitializerManager materialInitializer;
            return materialInitializer;
        }

    private:
        MaterialManager() = default;

    public:
        /**
         * @brief Gets cached material or loads it from file.
         * @param Path Filepath of a material file.
         * @return Material provided by a given file.
         */
        static Material* GetMaterial(const std::string& Path);

        /**
         * @brief Deletes material object.
         * @param Path Filepath of a material file.
         * @return True if material existed, false otherwise.
         */
        static bool DeleteMaterial(const std::string& Path);

        /**
         * @brief Deletes material object.
         * @param Material Material to be deleted.
         * @return
         */
        static bool DeleteMaterial(Material* Material);

        /**
         * @brief Deletes all materials.
         */
        static void DeleteAllMaterials();

        /**
         * @brief Finds material's filepath.
         * @param Material Material.
         * @return Material's filepath, empty string if material is not loaded.
         */
        static std::string GetMaterialPath(const Material* Material);

        /**
         * @brief
         * @param Path
         */
        static void SaveMaterial(const std::string& Path);

        /**
         * @brief Saves material to its file.
         * @param Material Material to be saved.
         */
        static void SaveMaterial(const Material* Material);

        /**
         * @brief Saves material to a file.
         * @param Path Path of a material file.
         * @param Material Material to be saved.
         */
        static void SaveMaterial(const std::string& Path, const Material* Material);

        /**
         * @brief Loads materials' shaders.
         */
        static void Initialize();

        /**
         * @brief Gets cached material or loads it from file.
         * @tparam T Material's class
         * @param Path Material's filepath.
         * @return Material, nullptr if type doesn't match.
         */
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

        /**
         * @brief Creates a new material with default values.
         * @tparam T Material's class.
         * @param Path File to save material to.
         * @return Created material object.
         */
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
