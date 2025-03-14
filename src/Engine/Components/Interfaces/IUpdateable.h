#pragma once

namespace Engine
{

    class IUpdateable
    {
    public:
        virtual ~IUpdateable() = default;

    public:
        /**
         * @brief Called once per frame.
         * @param DeltaTime Time since last frame.
         */
        virtual void Update(float DeltaTime) = 0;
    };

} // Engine
