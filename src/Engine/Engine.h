#pragma once

#include "Components/Audio/AudioListener.h"
#include "Components/Audio/BackgroundAudioPlayer.h"
#include "Components/Colliders/BoxCollider.h"
#include "glfw/glfw3.h"
#include "Engine/EngineObjects/Camera.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Models/Model.h"
#include "Shaders/Shader.h"
#include "Engine/Components/Camera/CameraFollow.h"

#if EDITOR
#include "imgui_impl/imgui_impl_glfw.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "Engine/GUI/EditorGUI.h"
#endif

namespace Engine
{
    class Engine final
    {
    public:
        constexpr static const char* const GlslVersion = "#version 460";
        static constexpr int32_t GlVersionMajor = 4;
        static constexpr int32_t GlVersionMinor = 6;
        Collider* Collider = nullptr;

        GLuint EditorFramebuffer;
        GLuint EditorColorTexture;
        GLuint EditorDepthRBO = 0;

    private:
        GLFWwindow* Window = nullptr;

        int WindowWidth = 0;
        int WindowHeight = 0;

        uint64_t Frame = 0;

        Camera* Camera = nullptr;
        AudioListener* AudioListener = nullptr;
        BackgroundAudioPlayer* BackgroundAudioPlayer = nullptr;
        bool CameraMoved = false;
        glm::vec2 LastMousePosition = glm::vec2(0, 0);

        Scene* CurrentScene;
#if EDITOR
        EditorGUI EditorGUI;
#endif

    public:
        Engine();

    public:
        virtual ~Engine();

    public:
        int Run();

    private:
        bool Initialize();

        void HandleInput(float deltaTime);
#if EDITOR
        void ImGuiInit();

        void ImGuiBegin();

        void ImGuiRender();

        void ImGuiEnd();
#endif
        void EndFrame();

        void FreeResources();

        static void GlfwErrorCallback(int Error, const char* Description);

        static void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);

        static void MouseCallback(GLFWwindow* Window, double MouseX, double MouseY);

        static void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods);

        void InitEditorFramebuffer();

    public:
        Scene* GetCurrentScene() const { return CurrentScene; };
    };
} // Engine
