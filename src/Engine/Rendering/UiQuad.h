#pragma once

namespace Engine
{
    /**
     * @brief Class used for rendering full screen quads.
     */
    class UiQuad
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
        UiQuad() = default;

    public:
        ~UiQuad();

    public:
        /**
         * @brief Draws quad covering whole screen
         */
        static void Draw();

    private:
        static void Initialize();
    };
} // Engine
