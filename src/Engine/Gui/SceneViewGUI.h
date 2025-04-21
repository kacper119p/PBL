#pragma once
#if EDITOR
#include <glad/glad.h>
namespace Engine
{
    class SceneViewGUI {
    private:
        GLuint m_FramebufferTexture = 0;

    public:
        void SetFramebufferTexture(GLuint texture) { m_FramebufferTexture = texture; }
        void Draw();
    };
} // namespace Engine
#endif
