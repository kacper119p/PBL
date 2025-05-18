#pragma once
#if EDITOR
#include <glad/glad.h>
#include "Engine/EngineObjects/Scene/Scene.h"
#include "SceneHierarchyGUI.h"

namespace Engine
{
    class SceneViewGUI
    {
    private:
        GLuint m_FramebufferTexture = 0;

    public:
        void SetFramebufferTexture(GLuint texture) { m_FramebufferTexture = texture; }

        void Draw(CameraRenderData renderData, Scene* scene);
    };
} // namespace Engine
#endif
