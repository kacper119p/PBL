#pragma once

#include <stdexcept>

namespace Engine
{
    /**
     * @brief Thrown when trying to create a new instance of singleton when one already exists.
     */
    class SingletonAlreadyExistsException : public std::runtime_error
    {
    public:
        explicit SingletonAlreadyExistsException(const runtime_error& Other) :
            runtime_error(Other)
        {
        }

        explicit SingletonAlreadyExistsException(const char* Message) :
            runtime_error(Message)
        {
        }

        explicit SingletonAlreadyExistsException(const std::string& Message) :
            runtime_error(Message)
        {
        }
    };

} // Engine
