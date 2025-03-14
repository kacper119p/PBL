#pragma once

namespace Engine::Rendering
{
    /**
     * @brief Class used for rendering full screen quads.
     */
    class ScreenQuad
    {
    private:
        unsigned int VertexArray;
        unsigned int VertexBuffer;
        unsigned int ElementBuffer;

    public:
        ScreenQuad();

        virtual ~ScreenQuad();

    public:
        /**
         * @brief Draws quad covering whole screen
         */
        void Draw();
    };
} // Engine
