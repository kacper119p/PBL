#pragma once
#include "Component.h"
#include "Interfaces/IUpdateable.h"
#include "imgui.h"

namespace Engine
{
    /**
     * @brief Provides base implementation for IUpdateable.
     */
    class Updateable : public Component, public IUpdateable
    {
    public:
        Updateable() = default;

    public:
        ~Updateable() override;

    public:
        void Start() override;
    };

} // Engine
