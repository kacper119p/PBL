#pragma once

namespace Engine
{

    class Plane
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
        Plane();

    private:
        ~Plane();

    public:
        /**
         * @brief Draws quad covering whole screen
         */
        static void Draw();

    private:
        static void Initialize();
    };

} // Engine
