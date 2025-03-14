#pragma once

#include <stdexcept>

namespace Engine
{
    /**
     * @brief Thrown when trying to add a new light after already adding maximum supported number of lights.
     */
    class LightCapacityReachedException : public std::runtime_error
    {
    public:
        explicit LightCapacityReachedException(const runtime_error& Other) :
            runtime_error(Other)
        {
        }

        explicit LightCapacityReachedException(const char* Message) :
            runtime_error(Message)
        {
        }

        explicit LightCapacityReachedException(const std::string& Message) :
            runtime_error(Message)
        {
        }
    };

} // Engine
