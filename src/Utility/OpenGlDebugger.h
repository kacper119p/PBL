#pragma once

#if DEBUG

#include <glad/glad.h>
#include "GLFW/glfw3.h"

namespace Utility
{
    /**
     * @brief OpenGL debug utility based on https://learnopengl.com/In-Practice/Debugging.
     */
    class OpenGlDebugger
    {
    public:
        /**
         * @brief Enables OpenGL Debugger. <br>
         * GLFW_OPENGL_DEBUG_CONTEXT window hint must be set to true before creating glfw window, to work correctly.
         */
        static void Enable();

    private:
        static void APIENTRY GlDebugOutput(GLenum Source, GLenum Type, unsigned int Id, GLenum Severity,
                                           GLsizei Length, const char* Message, const void* UserParam);

    };

}
#endif
