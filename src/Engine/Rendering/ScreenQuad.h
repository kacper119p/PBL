#pragma once
#include <cstdint>


namespace Engine::Rendering
{
    /**
     * @brief Class used for rendering full screen quads.
     */
    class ScreenQuad
    {
    private:
        struct CachedData
        {
            uint32_t VertexArray = 0;
            uint32_t VertexBuffer = 0;

            ~CachedData();
        };

    private:
        static CachedData CachedData;

    private:
        ScreenQuad() = default;

    public:
        ~ScreenQuad();

    public:
        /**
         * @brief Draws quad covering whole screen
         */
        static void Draw();

        static void Initialize();
    };
} // Engine
