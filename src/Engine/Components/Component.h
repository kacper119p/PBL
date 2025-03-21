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
        Component() = default;

    public:
        virtual ~Component() = default;

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
         * @brief Invoked when this component is attached to a new Entity.
         * @param NewOwner New owner of this component.
         */
        virtual void OnAdd(Entity* NewOwner);

    public:
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const override;

        void DeserializeValuePass(const rapidjson::Value& Object) override;

        void DeserializeReferencesPass(const rapidjson::Value& Object,
                                       std::unordered_map<GUID, SerializedObject*> ReferenceMap) override;
    };
}
