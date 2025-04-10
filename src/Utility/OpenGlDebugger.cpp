#include <sstream>

#include "../../cmake-build-debug-visual-studio-editor/_deps/spdlog-src/include/spdlog/spdlog.h"
#if DEBUG
#include "OpenGlDebugger.h"

#include <iostream>
#include <ostream>

#include "AssertionsUtility.h"

namespace Utility
{
    void OpenGlDebugger::Enable()
    {

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        CHECK_MESSAGE(flags & GL_CONTEXT_FLAG_DEBUG_BIT, "Debug context was not set up correctly.");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GlDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    }

    void APIENTRY OpenGlDebugger::GlDebugOutput(const GLenum Source,
                                                const GLenum Type,
                                                const unsigned int Id,
                                                const GLenum Severity,
                                                GLsizei Length,
                                                const char* Message,
                                                const void* UserParam)
    {
        // ignore non-significant error/warning codes
        if (Id == 131169 || Id == 131185 || Id == 131218 || Id == 131204)
            return;

        std::stringstream stream;

        stream << "OpenGL Error (" << Id << "): " << Message << " ";

        switch (Source)
        {
            case GL_DEBUG_SOURCE_API:
                stream << "Source: API | ";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                stream << "Source: Window System | ";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                stream << "Source: Shader Compiler | ";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                stream << "Source: Third Party | ";
                break;
            case GL_DEBUG_SOURCE_APPLICATION:
                stream << "Source: Application | ";
                break;
            case GL_DEBUG_SOURCE_OTHER:
                stream << "Source: Other | ";
                break;
            default:
                stream << "Source: Other |";
                break;;
        }

        switch (Type)
        {
            case GL_DEBUG_TYPE_ERROR:
                stream << "Type: Error | ";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                stream << "Type: Deprecated Behaviour | ";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                stream << "Type: Undefined Behaviour | ";
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                stream << "Type: Portability | ";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                stream << "Type: Performance | ";
                break;
            case GL_DEBUG_TYPE_MARKER:
                stream << "Type: Marker | ";
                break;
            case GL_DEBUG_TYPE_PUSH_GROUP:
                stream << "Type: Push Group | ";
            case GL_DEBUG_TYPE_POP_GROUP:
                stream << "Type: Pop Group | ";
                break;
            case GL_DEBUG_TYPE_OTHER:
                stream << "Type: Other | ";
                break;
            default:
                stream << "Type: Other | ";
                break;
        }

        switch (Severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:
                stream << "Severity: high";
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                stream << "Severity: medium";
                break;
            case GL_DEBUG_SEVERITY_LOW:
                stream << "Severity: low";
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                stream << "Severity: notification";
                break;
            default:
                stream << "Severity: other";
                break;
        }

        spdlog::error(stream.str());
    }
} // Utility
#endif
