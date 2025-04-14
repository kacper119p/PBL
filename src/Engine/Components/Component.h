#pragma once
#include "Serialization/SerializedObject.h"

namespace Engine
{
    class Entity;

    /**
     * @brief Base class for all components attached to entities.
     */
    class Component : public Serialization::SerializedObject
    {

    private:
        Entity* Owner = nullptr;

    public:
        /**
         * @brief Initializes Component with default values.
         */
        Component() = default;

    public:
        ~Component() override = default;

    public:
        /**
         * @brief
         * @return Entity this component is attached to.
         */
        [[nodiscard]] Entity* GetOwner() const
        {
            return Owner;
        }

        /**
         * @brief Sets owner of this Component.
         * @param InOwner New Owner.
         */
        void SetOwner(Entity* InOwner)
        {
            Owner = InOwner;
        }

        /**
         * @brief Invoked after this component is created.
         */
        virtual void Start();

        /**
         * @brief Invoked just before deleting this component.
         */
        virtual void OnDestroy();

        virtual void DrawImGui() {};
    };
}
