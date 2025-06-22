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

    public:
        ScreenQuad();

        ~ScreenQuad();

    public:
        /**
         * @brief Draws quad covering whole screen
         */
        void Draw() const;

    private:
        static void Initialize();
    };
} // Engine
