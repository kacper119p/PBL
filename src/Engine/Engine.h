#pragma once

#include "stdint.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "Engine/EngineObjects/Camera.h"
#include "Engine/EngineObjects/Scene.h"
#include "Materials/Material.h"
#include "Models/Model.h"
#include "Shaders/Shader.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

namespace Engine
{
    class Engine
    {
    public:
        constexpr static const char* const GlslVersion = "#version 460";
        static constexpr int32_t GlVersionMajor = 4;
        static constexpr int32_t GlVersionMinor = 6;

    private:
        GLFWwindow* Window = nullptr;

        int WindowWidth = 0;
        int WindowHeight = 0;

        uint64_t Frame = 0;

        Camera* Camera = nullptr;
        bool CameraMoved = false;
        glm::vec2 LastMousePosition = glm::vec2(0, 0);

        Scene* Scene;
        std::vector<unsigned int> Textures;
        std::vector<Models::Model*> Models;
        std::vector<Shaders::Shader> Shaders;
        std::vector<Materials::Material*> Materials;

    public:
        Engine();

    public:
        virtual ~Engine();

    public:
        int Run();

    private:
        bool Initialize();

        void HandleInput(float deltaTime);

        void ImGuiInit();

        void ImGuiBegin();

        void ImGuiRender();

        void ImGuiEnd();

        void EndFrame();

        void FreeResources();

        static void GlfwErrorCallback(int Error, const char* Description);

        static void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);

        static void MouseCallback(GLFWwindow* Window, double MouseX, double MouseY);

        static void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods);
    };
} // Engine
