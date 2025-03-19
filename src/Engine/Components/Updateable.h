#pragma once
#include "Component.h"
#include "Interfaces/IUpdateable.h"

namespace Engine
{
    /**
     * @brief Provides base implementation for IUpdateable.
     */
    class Updateable : public Component, IUpdateable
    {
    public:
        Updateable() = default;

    public:
        ~Updateable() override;

    public:
        void OnAdd(Entity* NewOwner) override;
    };

} // Engine
