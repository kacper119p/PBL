#pragma once

#include <iterator>
#include <cstddef>
#include <vector>


#include "Engine/Components/Component.h"
#include "Engine/Components/Transform.h"
#include "Utility/Cloneable/TCloneable.h"

namespace Engine
{
    class Component;
    class Component;
    class Scene;

    /**
     * @brief Base class for all objects that exist in a scene.
     */
    class Entity : public Serialization::SerializedObject, public Utility::TCloneable<Entity>
    {
        friend class Scene;

    private:
        static std::vector<Entity*> ToDestroy;

    private:
        Transform Transform;
        Scene* Scene = nullptr;
        std::vector<Component*> Components;
        std::string Name = "New Entity";

    public:
        /**
         * @brief Initializes a new Entity with default values.
         */
        Entity() :
            Transform(Engine::Transform(this)), Components(std::vector<Component*>())
        {
        }

        ~Entity() override;

    public:
        /**
         * @brief Returns transform of this entity.
         */
        [[nodiscard]] class Transform* GetTransform()
        {
            return &Transform;
        }

        /**
         * @brief Returns transform of this entity.
         */
        [[nodiscard]] const class Transform* GetTransform() const
        {
            return &Transform;
        }

        /**
         * @brief Returns scene this entity belongs to.
         */
        [[nodiscard]] class Scene* GetScene() const
        {
            return Scene;
        }

        /**
         * @brief Sets scene for this entity.
         */
        void SetScene(class Scene* Scene)
        {
            this->Scene = Scene;
        }

        /**
         * @brief Adds a new component to this entity.
         * @tparam T Component's class.
         */
        template<class T>
        T* AddComponent()
        {
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
            T* component = new T();
            component->SetOwner(this);
            Components.push_back(component);
            component->Start();
            return component;
        }

#if EDITOR
        /**
         * @brief Adds a new component to this Entity. To be used with Editor GUI.
         * @param Component A new component.
         */
        void AddComponent(Component* Component)
        {
            Component->SetOwner(this);
            Components.push_back(Component);
            Component->Start();
        }

        void RemoveComponent(Component* Component)
        {
            std::erase(Components, Component);
            Component->OnDestroy();
            delete Component;
        }
#endif

        /**
         * @brief Finds component of given class in this entity.
         * @tparam T Class of sought component.
         * @return Component if found, nullptr otherwise.
         */
        template<class T>
        [[nodiscard]] T* GetComponent() const
        {
            static_assert(std::is_base_of_v<Component, T>, "Class not derived from IComponent");
            for (Component* component : Components)
            {
                if (T* result = dynamic_cast<T*>(component))
                {
                    return result;
                }
            }
            return nullptr;
        }

        /**
         * @brief Removes component from this entity by class.
         * @tparam T Class of component to be removed.
         */
        template<class T>
        void RemoveComponent()
        {
            {
                static_assert(std::is_base_of_v<Component, T>, "Class not derived from IComponent");
                if (Component* component = GetComponent<T>())
                {
                    std::erase(Components, component);
                    component->OnDestroy();
                    delete component;
                }
            }
        }

        /**
         * @brief Returns name of this entity.
         */
        [[nodiscard]] std::string GetName() const
        {
            return Name;
        }

        /**
        * @brief Sets a name of this entity.
        * @return A new name.
        */
        void SetName(const std::string& Name)
        {
            this->Name = Name;
        }

        [[nodiscard]] std::vector<Component*>::iterator begin()
        {
            return Components.begin();
        }

        [[nodiscard]] std::vector<Component*>::iterator end()
        {
            return Components.end();
        }

        [[nodiscard]] std::vector<Component*>::const_iterator begin() const
        {
            return Components.begin();
        }

        [[nodiscard]] std::vector<Component*>::const_iterator end() const
        {
            return Components.end();
        }

        void Destroy()
        {
            ToDestroy.push_back(this);
        }

        static void DestroyQueued()
        {
            for (int i = 0; i < ToDestroy.size(); ++i)
            {
                delete ToDestroy[i];
            }
            ToDestroy.clear();
        }

        [[nodiscard]] Entity* CloneAsConcrete() const override;

        void SerializeEntity(rapidjson::Value& Object, rapidjson::Document::AllocatorType& Allocator) const;

        static Entity* DeserializeEntity(rapidjson::Value& Object, class Scene* Scene, class Transform* Parent);

#if EDITOR
        void DrawImGui();
#endif
        SERIALIZATION_EXPORT_CLASS(Entity);
    };
}
