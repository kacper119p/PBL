#pragma once

#include <stdexcept>

namespace Shaders
{
    class ShaderException : public std::runtime_error
    {
    public:
        explicit ShaderException(const runtime_error& other);

        explicit ShaderException(const char* message);

        explicit ShaderException(const std::string& message);
    };
} //Shaders
