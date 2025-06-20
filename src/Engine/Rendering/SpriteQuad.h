#pragma once
#include <cstdint>

namespace Engine
{
    /**
     * @brief Class used for rendering full screen quads.
     */
    class SpriteQuad
    {
    private:
        struct CachedData
        {
            unsigned int VertexArray = 0;
            unsigned int VertexBuffer = 0;
            unsigned int ElementBuffer = 0;

            ~CachedData();
        };

    private:
        static CachedData CachedData;

    private:
        SpriteQuad();

    public:
        ~SpriteQuad();

    public:
        /**
         * @brief Draws Sprite Quad
         */
        static void Draw();

        /**
         * @brief Draws given amount of instanced ScreenQuads.
         * @param InstanceCount Number of instances to draw.
         */
        static void DrawInstanced(int32_t InstanceCount);

    private:
        static void Initialize();
    };
} // Engine
