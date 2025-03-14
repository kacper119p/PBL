#pragma once

#include <vector>

#include "Engine/Components/Component.h"
#include "Engine/Components/Transform.h"

namespace Engine
{
    class Component;

    /**
     * @brief Base class for all objects that exist in a scene.
     */
    class Entity
    {
    private:
        Transform Transform;
        std::vector<Component*> Components;

    public:
        Entity() :
            Transform(Engine::Transform(this)), Components(std::vector<Component*>())
        {
        }

        virtual ~Entity();

    public:
        /**
         * @brief Returns transform of this entity.
         */
        class Transform* GetTransform()
        {
            return &Transform;
        }

        /**
         * @brief Adds a new component to this entity.
         * @param Component Component to be added.
         */
        void AddComponent(Component* Component)
        {
            Components.push_back(Component);
            Component->OnAdd(this);
        }

        /**
         * @brief Finds component of given class in this entity.
         * @tparam T Class of sought component.
         * @return Component if found, nullptr otherwise.
         */
        template<class T>
        [[nodiscard]] Component* GetComponent() const
        {
            {
                static_assert(std::is_base_of_v<Component, T>, "Class not derived from IComponent");
                for (Component* component : Components)
                {
                    if (T* result = dynamic_cast<T>(component))
                    {
                        return result;
                    }
                }
                return nullptr;
            }
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
                    delete component;
                }
            }
        }
    };
}
