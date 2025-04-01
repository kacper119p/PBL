#pragma once

#include <iterator>
#include <cstddef>
#include <vector>


#include "Engine/Components/Component.h"
#include "Engine/Components/Transform.h"

namespace Engine
{
    class Component;

    /**
     * @brief Base class for all objects that exist in a scene.
     */
    class Entity final : public Serialization::SerializedObject
    {
    public:
        struct Iterator
        {
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = Component*;
            using pointer = Component**;
            using reference = Component*&;

        private:
            pointer ptr;

        public:
            explicit Iterator(pointer ptr) :
                ptr(ptr)
            {
            }

        public:
            reference operator*() const { return *ptr; }
            pointer operator->() const { return ptr; }

            Iterator& operator++()
            {
                ptr++;
                return *this;
            }

            Iterator operator++(int)
            {
                const Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            friend bool operator==(const Iterator& a, const Iterator& b)
            {
                return a.ptr == b.ptr;
            }

            friend bool operator!=(const Iterator& a, const Iterator& b)
            {
                return a.ptr != b.ptr;
            }
        };

    private:
        Transform Transform;
        std::vector<Component*> Components;

    public:
        Entity() :
            Transform(Engine::Transform(this)), Components(std::vector<Component*>())
        {
        }

        ~Entity() override;

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

    public:
        Iterator begin()
        {
            return Iterator(Components.data());
        }

        Iterator end()
        {
            return Iterator(Components.data() + Components.size());
        }

        SERIALIZATION_METHODS_DECLARATIONS
    };
}
