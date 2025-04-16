#include "Engine/EngineObjects/Camera.h"
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

#include "Engine.h"
#include "Engine/EngineObjects/RenderingManager.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Materials/Material.h"
#include "Materials/MaterialManager.h"
#include "Models/ModelManager.h"
#include "Utility/SystemUtilities.h"
#include "Scene/SceneBuilder.h"
#include "Shaders/ShaderManager.h"
#include "Textures/TextureManager.h"
#include "UI/FontRendering/TextManager.h"
#include "Engine/Components/Audio/AudioSource.h"
#include "Engine/Components/Audio/AudioListener.h"
#include "UI/FontRendering/TextManager.h"
#include "UI/UiImplementations/SampleUi.h"
#include "tracy/Tracy.hpp"
#if DEBUG
#include "Utility/OpenGlDebugger.h"
#endif

#if EDITOR
#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "Engine/EngineObjects/GizmoManager.h"
#endif

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
#if EDITOR
        ImGuiInit();
        spdlog::info("Initialized ImGui.");
#endif

        Camera->SetProjectionMatrix(glm::perspective(glm::radians(70.0f), float(WindowWidth) /
                                                                          float(WindowHeight), 0.1f, 1000.0f));

        try
        {
            SceneBuilding::SceneBuilder::Build(CurrentScene);
            EditorGUI.SetHierarchyRoot(CurrentScene->GetRoot()->GetTransform());
        } catch (std::runtime_error& e)
        {
            spdlog::error(e.what());
            throw;
        }

        spdlog::info("Successfully built scene.");

        float lastFrame = 0.0f;

        // Main loop
        while (!glfwWindowShouldClose(Window))
        {
            ZoneScopedN("GameLoop");
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
            AudioListener->UpdateListener();

            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, EditorFramebuffer);
            glBlitFramebuffer(0, 0, displayW, displayH, 0, 0, displayW, displayH, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);

#if EDITOR
            // Draw ImGui
            ImGuiBegin();
            ImGuiRender();
            GizmoManager::GetInstance()->Manipulate(renderData);
            ImGuiEnd(); // this call effectively renders ImGui
#endif

            // End frame and swap buffers (double buffering)
            EndFrame();
            FrameMark;
        }

        spdlog::info("Closing project.");

        // Cleanup
        FreeResources();

        spdlog::info("Freed scene resources.");
#if EDITOR
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
#endif

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


#if DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

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
        glEnable(GL_MULTISAMPLE);

        RenderingManager::Initialize(glm::ivec2(WindowWidth, WindowHeight));
        LightManager::Initialize();
        UpdateManager::Initialize();
        Materials::MaterialManager::Initialize();
        Ui::TextManager::Initialize();
#if EDITOR
        //for editor game screen
        InitEditorFramebuffer();
        EditorGUI.SetSceneViewFramebuffer(EditorColorTexture);
        GizmoManager::Initialize();
#endif
#if DEBUG
        Utility::OpenGlDebugger::Enable();
#endif

        Camera = new class Camera(glm::perspective(glm::radians(70.0f),
                                                   float(WindowWidth) / float(WindowHeight),
                                                   0.1f,
                                                   100.0f),
                                  0.0018f);
        Camera->SetPosition(glm::vec3(0.0f, 5.0f, 20.0f));

        AudioListener = new class AudioListener(*Camera);
        spdlog::info("Sounds loaded.");

        return true;
    }

   void Engine::HandleInput(float deltaTime)
    {
#if EDITOR
        ImGuiIO& io = ImGui::GetIO();

        // Only capture input when mouse is inside the scene view rectangle
        float mouseX = io.MousePos.x;
        float mouseY = io.MousePos.y;

        float sceneX = GizmoManager::GetInstance()->rectX;
        float sceneY = GizmoManager::GetInstance()->rectY;
        float sceneW = GizmoManager::GetInstance()->rectWidth;
        float sceneH = GizmoManager::GetInstance()->rectHeight;

        // Check if mouse is inside the scene rect
        bool insideSceneView =
                mouseX >= sceneX && mouseX <= sceneX + sceneW && mouseY >= sceneY && mouseY <= sceneY + sceneH;

        // Cancel input if ImGui wants mouse or we're outside the scene view or manipulating gizmo
        if (!insideSceneView || io.WantCaptureMouse || ImGuizmo::IsUsing())
        {
            return;
        }
#endif

        if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(Window, true);
            return;
        }

        const float cameraSpeed = 10.0f * deltaTime;

        // WASD movement
        if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            Camera->SetPosition(Camera->GetPosition() + cameraSpeed * Camera->GetForward());
        }
        if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            Camera->SetPosition(Camera->GetPosition() - cameraSpeed * Camera->GetForward());
        }
        if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            Camera->SetPosition(Camera->GetPosition() -
                                cameraSpeed * glm::normalize(glm::cross(Camera->GetForward(), Camera->GetUp())));
        }
        if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            Camera->SetPosition(Camera->GetPosition() +
                                cameraSpeed * glm::normalize(glm::cross(Camera->GetForward(), Camera->GetUp())));
        }

#if EDITOR
        // Gizmo switching keys
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
#endif
    }


#if EDITOR
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

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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

        //LightsGui::Draw();
        EditorGUI.Render(Frame, CurrentScene);

    }

    void Engine::ImGuiEnd()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
#endif

    void Engine::EndFrame()
    {
        glfwPollEvents();
        glfwMakeContextCurrent(Window);
        glfwSwapBuffers(Window);
    }

    void Engine::FreeResources()
    {
        delete CurrentScene;
        TextureManager::DeleteAllTextures();
        Shaders::ShaderManager::FreeResources();
        Models::ModelManager::DeleteAllModels();
        Materials::MaterialManager::DeleteAllMaterials();
        AudioManager::DestroyInstance();
        delete AudioListener;
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

    void Engine::InitEditorFramebuffer()
    {
        glGenFramebuffers(1, &EditorFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, EditorFramebuffer);

        // Create color texture
        glGenTextures(1, &EditorColorTexture);
        glBindTexture(GL_TEXTURE_2D, EditorColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WindowWidth, WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EditorColorTexture, 0);

        // Create depth renderbuffer
        glGenRenderbuffers(1, &EditorDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, EditorDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, EditorDepthRBO);

        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            spdlog::error("Editor framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // Engine
