#pragma once
#include "Engine/Components/Component.h"
#include "Shaders/Shader.h"

namespace Engine
{

    class BloodSource : public Component
    {
    public:
        void Draw() const;
    };

}
