#pragma once

namespace Engine
{
    /**
     * @brief Class used to render cube maps.
     */
    class CubeGeometry
    {
    private:
        unsigned int VertexArray;
        unsigned int VertexBuffer;
        unsigned int ElementBuffer;

    public:
        CubeGeometry();

        virtual ~CubeGeometry();

    public:
        /**
         * @brief Renders cube map.
         */
        void Draw();
    };

} // Engine
