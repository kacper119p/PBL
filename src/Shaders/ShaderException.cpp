//
// Created by Kacper on 07.10.2024.
//

#include "ShaderException.h"

Shaders::ShaderException::ShaderException(const runtime_error& other) : runtime_error(other)
{
}

Shaders::ShaderException::ShaderException(const char* message) : runtime_error(message)
{
}

Shaders::ShaderException::ShaderException(const std::string& message) : runtime_error(message)
{
}
