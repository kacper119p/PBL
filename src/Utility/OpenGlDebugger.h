#pragma once
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#if DEBUG
namespace Utility
{
    /**
     * @brief OpenGL debug utility based on https://learnopengl.com/In-Practice/Debugging.
     */
    class OpenGlDebugger
    {
    public:
        static void Enable();

    private:
        static void APIENTRY GlDebugOutput(GLenum Source, GLenum Type, unsigned int Id, GLenum Severity,
                                           GLsizei Length, const char* Message, const void* UserParam);

    };

}
#endif
