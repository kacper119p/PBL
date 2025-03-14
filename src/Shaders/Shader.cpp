//
// Created by Kacper on 07.10.2024.
//
#include <format>
#include "Shader.h"
#include "ShaderException.h"

namespace
{
    constexpr int ErrorLogSize = 512;
}

Shaders::Shader::Shader(const ShaderSourceFiles& MainPass)
{
    try
    {
        Id = CompilePass(MainPass);
    } catch (ShaderException& e)
    {
        throw e;
    }
}

unsigned int Shaders::Shader::CompilePass(const Shaders::ShaderSourceFiles& Pass)
{
    unsigned int vertexShader;
    try
    {
        vertexShader = CompileShader(Pass.VertexShader, GL_VERTEX_SHADER);
    } catch (ShaderException& e)
    {
        throw e;
    }

    unsigned int geometryShader = 0;
    if (Pass.GeometryShader)
    {
        try
        {
            geometryShader = CompileShader(Pass.GeometryShader, GL_GEOMETRY_SHADER);
        } catch (ShaderException& e)
        {
            throw e;
        }
    }

    unsigned int fragmentShader;
    try
    {
        fragmentShader = CompileShader(Pass.FragmentShader, GL_FRAGMENT_SHADER);
    } catch (ShaderException& e)
    {
        if (Pass.GeometryShader)
        {
            glDeleteShader(geometryShader);
        }
        glDeleteShader(vertexShader);
        throw e;
    }

    unsigned int id = glCreateProgram();

    glAttachShader(id, vertexShader);
    if (Pass.GeometryShader)
    {
        glAttachShader(id, geometryShader);
    }
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    int success;
    char errorInfo[ErrorLogSize];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, ErrorLogSize, nullptr, errorInfo);
        glDeleteProgram(id);
        if (Pass.GeometryShader)
        {
            glDeleteShader(geometryShader);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw ShaderException(std::format("Failed to link shader: {}\n", errorInfo));
    }

    glDetachShader(id, vertexShader);
    glDeleteShader(vertexShader);
    if (Pass.GeometryShader)
    {
        glDetachShader(id, geometryShader);
        glDeleteShader(geometryShader);
    }
    glDetachShader(id, fragmentShader);
    glDeleteShader(fragmentShader);
    return id;
}
