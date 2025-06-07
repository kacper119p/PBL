#pragma once

#include "Engine/Components/Component.h"

namespace Engine
{
    /**
     * @brief Represents a navigation area used for pathfinding or AI logic.
     * This component can be attached to an entity to define if a region is walkable by agents.
     */
    class NavArea final : public Component
    {
    public:
        /**
         * @brief Default constructor.
         */
        NavArea() = default;

        /**
         * @brief Sets whether the navigation area is walkable.
         * @param Value True if the area should be walkable, false if area should be omitted during building NavMesh.
         */
        void SetWalkable(bool Value);

        /**
         * @brief Checks whether the navigation area is walkable.
         * @return True if the area is walkable; false otherwise.
         */
        [[nodiscard]] bool GetWalkable() const;

#if EDITOR
        void DrawImGui() override;
#endif

    private:
        SERIALIZATION_EXPORT_CLASS(NavArea)

        bool IsWalkable = false; ///< Indicates whether the navigation area is walkable.
    };
}
