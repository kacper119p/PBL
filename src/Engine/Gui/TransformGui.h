#pragma once

#include "Engine/Components/Transform.h"

namespace Engine
{
    /**
     * @brief Used for ImGui transform properties manipulation.
     */
    class TransformGui
    {
    public:
        virtual ~TransformGui() = 0;

    public:
        static void Draw(Transform* Transform);
    };

} // Engine
