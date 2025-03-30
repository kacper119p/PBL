//
// Created by Kacper on 10.10.2024.
//

#include "Engine/EngineObjects/Camera.h"
#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

#include "Engine.h"
#include "Engine/EngineObjects/RenderingManager.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/EngineObjects/GizmoManager.h"
#include "imgui_internal.h"
#include "Materials/MaterialManager.h"
#include "Utility/SystemUtilities.h"
#include "Scene/SceneBuilder.h"
#include "Textures/TextureManager.h"

namespace SceneBuilding = Scene;

namespace Engine
{
    Engine::Engine()
    {
    }

    Engine::~Engine()
    {
        delete Camera;
    }

    int Engine::Run()
    {
        if (!Initialize())
        {
            spdlog::error("Failed to initialize project!");
            return EXIT_FAILURE;
        }

        spdlog::info("Initialized project.");

        ImGuiInit();
        spdlog::info("Initialized ImGui.");

        Camera->SetProjectionMatrix(glm::perspective(glm::radians(70.0f), float(WindowWidth) /
                                                                          float(WindowHeight), 0.1f, 1000.0f));

        try
        {
            SceneBuilding::SceneBuilder::Build(Scene);
        } catch (std::runtime_error& e)
        {
            spdlog::error(e.what());
            return EXIT_FAILURE;
        }

        spdlog::info("Successfully built scene.");

        float lastFrame = 0.0f;
        // Main loop
        while (!glfwWindowShouldClose(Window))
        {
            ++Frame;
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            float currentFrame = glfwGetTime();
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // Process I/O operations here
            HandleInput(deltaTime);

            // Update game objects' state here
            UpdateManager::GetInstance()->Update(deltaTime);

            int displayW, displayH;
            glfwMakeContextCurrent(Window);
            glfwGetFramebufferSize(Window, &displayW, &displayH);

            const CameraRenderData renderData(Camera->GetPosition(), Camera->GetTransform(),
                                              Camera->GetProjectionMatrix());
            RenderingManager::GetInstance()->RenderAll(renderData, WindowWidth, WindowHeight);

            // Draw ImGui
            ImGuiBegin();
            ImGuiRender();
            GizmoManager::GetInstance()->Manipulate(renderData);
            ImGuiEnd(); // this call effectively renders ImGui

            // End frame and swap buffers (double buffering)
            EndFrame();
        }

        spdlog::info("Closing project.");

        // Cleanup
        FreeResources();

        spdlog::info("Freed scene resources.");

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(Window);
        glfwTerminate();

        spdlog::info("Finished shutdown.");

        return 0;
    }

    bool Engine::Initialize()
    {
        glfwSetErrorCallback(GlfwErrorCallback);
        if (!glfwInit())
        {
            spdlog::error("Failed to initalize GLFW!");
            return false;
        }

        // For windowless fullscreen
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        // GL 4.6 + GLSL 460
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GlVersionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GlVersionMinor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only


        // Create window with graphics context
        Window = glfwCreateWindow(mode->width, mode->height, "Tide Engine", monitor, nullptr);
        if (!Window)
        {
            spdlog::error("Failed to create GLFW Window!");
            return false;
        }

        glfwMakeContextCurrent(Window);
        glfwSwapInterval(1); // Enable VSync - fixes FPS at the refresh rate of your screen

        glfwGetFramebufferSize(Window, &WindowWidth, &WindowHeight);

        glfwSetWindowUserPointer(Window, this);
        glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
        glfwSetCursorPosCallback(Window, MouseCallback);
        glfwSetMouseButtonCallback(Window, MouseButtonCallback);

        bool err = !gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        if (err)
        {
            spdlog::error("Failed to initialize OpenGL loader!");
            return false;
        }

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        RenderingManager::Initialize();
        LightManager::Initialize();
        UpdateManager::Initialize();
        GizmoManager::Initialize();
        Materials::MaterialManager::Initialize();

        Camera = new class Camera(glm::perspective(glm::radians(70.0f),
                                                   float(WindowWidth) / float(WindowHeight),
                                                   0.1f,
                                                   100.0f),
                                  0.0018f);
        Camera->SetPosition(glm::vec3(0.0f, 5.0f, 20.0f));

        return true;
    }

    void Engine::HandleInput(float deltaTime)
    {
        if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(Window, true);
            return;
        }

        const float cameraSpeed = 10.0f * deltaTime; // adjust accordingly
        if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            Camera->SetPosition(Camera->GetPosition() + cameraSpeed * Camera->GetForward());
        }
        if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            Camera->SetPosition(Camera->GetPosition() - cameraSpeed * Camera->GetForward());
        }
        if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            Camera->SetPosition(Camera->GetPosition() -
                                cameraSpeed * glm::normalize(glm::cross(Camera->GetForward(), Camera->GetUp())));
        }
        if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            Camera->SetPosition(Camera->GetPosition() +
                                cameraSpeed * glm::normalize(glm::cross(Camera->GetForward(), Camera->GetUp())));
        }
        if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS && GizmoManager::GetInstance()->GetManaged() != nullptr)
        {
            GizmoManager::GetInstance()->SetCurrentOperation(ImGuizmo::OPERATION::TRANSLATE);
        }
        if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS && GizmoManager::GetInstance()->GetManaged() != nullptr)
        {
            GizmoManager::GetInstance()->SetCurrentOperation(ImGuizmo::OPERATION::ROTATE);
        }
        if (glfwGetKey(Window, GLFW_KEY_T) == GLFW_PRESS && GizmoManager::GetInstance()->GetManaged() != nullptr)
        {
            GizmoManager::GetInstance()->SetCurrentOperation(ImGuizmo::OPERATION::SCALE);
        }
    }

    void Engine::ImGuiInit()
    {
        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void) io;

        ImGui_ImplGlfw_InitForOpenGL(Window, true);
        ImGui_ImplOpenGL3_Init(GlslVersion);

        // Setup style
        ImGui::StyleColorsDark();
    }

    void Engine::ImGuiBegin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void Engine::ImGuiRender()
    {
        ImGui::Begin("Inspector");

        const GLubyte* renderer = glGetString(GL_RENDERER);
        std::string cpuInfo = Utility::GetCpuInfo();
        int ram = Utility::GetTotalRamGB();
        const GLubyte* version = glGetString(GL_VERSION);
        const GLubyte* shadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

        ImGui::Text("GPU: %s", (char*) renderer);
        ImGui::Text("CPU: %s", cpuInfo.c_str());
        ImGui::Text("RAM: %dGB", ram);
        ImGui::Text("OpenGL version: %s", (char*) version);
        ImGui::Text("Shading Language version: %s", (char*) shadingLanguageVersion);

        ImGui::Text("Frame: %llu\nApplication average %.3f ms/frame\n(%.1f FPS)", Frame,
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        LightsGui::Draw();


        ImGui::End();
    }

    void Engine::ImGuiEnd()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Engine::EndFrame()
    {
        glfwPollEvents();
        glfwMakeContextCurrent(Window);
        glfwSwapBuffers(Window);
    }

    void Engine::FreeResources()
    {
        delete Scene;
        TextureManager::DeleteAllTextures();

        for (Models::Model* model : Scene->Models)
        {
            delete model;
        }

        for (Shaders::Shader shader : Scene->Shaders)
        {
            shader.Delete();
        }

        for (Materials::Material* material : Scene->Materials)
        {
            delete material;
        }
    }

    void Engine::GlfwErrorCallback(int Error, const char* Description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", Error, Description);
    }

    void Engine::FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
    {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(Window));
        engine->WindowWidth = Width;
        engine->WindowHeight = Height;
    }

    void Engine::MouseCallback(GLFWwindow* Window, double MouseX, double MouseY)
    {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(Window));
        if (!(engine->CameraMoved))
        {
            engine->CameraMoved = true;
            engine->LastMousePosition = glm::vec2(MouseX, MouseY);
            return;
        }

        class Camera* camera = engine->Camera;
        glm::vec2 mousePosition = glm::vec2(MouseX, MouseY);

        if (!camera->IsDragged())
        {
            engine->LastMousePosition = mousePosition;
            return;
        }

        glm::vec2 deltaMouse = mousePosition - engine->LastMousePosition;
        deltaMouse *= camera->GetSensitivity();
        camera->SetRotation(camera->GetPitch() - deltaMouse.y, camera->GetYaw() + deltaMouse.x);

        engine->LastMousePosition = mousePosition;
    }

    void Engine::MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
    {
        if (Button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(Window));
            class Camera* camera = engine->Camera;
            if (Action == GLFW_PRESS)
            {
                camera->SetIsDragged(true);
            }
            else if (Action == GLFW_RELEASE)
            {
                camera->SetIsDragged(false);
            }
        }
    }
} // Engine
