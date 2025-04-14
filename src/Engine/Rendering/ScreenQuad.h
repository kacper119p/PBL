#pragma once

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
            unsigned int VertexArray = 0;
            unsigned int VertexBuffer = 0;
            unsigned int ElementBuffer = 0;

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
