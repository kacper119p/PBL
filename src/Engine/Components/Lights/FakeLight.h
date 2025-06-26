#pragma once

#include "glm/vec3.hpp"
#include "Engine/Components/Component.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    /**
    * @brief Directional light component.
    */
    class FakeLight final : public Component
    {
    public:
        FakeLight();

    public:
        ~FakeLight() override;

    public:
        void Start() override;
#if EDITOR
        void DrawImGui() override;
#endif
        SERIALIZATION_EXPORT_CLASS(FakeLight);
    };

} // Engine
