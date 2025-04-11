#include "EditorGUI.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Utility/SystemUtilities.h"
void Engine::EditorGUI::Init() { }
void Engine::EditorGUI::Render(uint64_t Frame)
{ 
    SetupDockspace();
    m_Hierarchy.Draw();
    RenderInspector(Frame);
    AudioManager::GetInstance().RenderGlobalVolumeImGui();
}
void Engine::EditorGUI::RenderInspector(uint64_t Frame)
{
    ImGui::Begin("Properties");

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

    ImGui::Text("Frame: %llu\nApplication average %.3f ms/frame\n(%.1f FPS)", Frame, 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);


    ImGui::End();
}
void Engine::EditorGUI::SetupDockspace()
{
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

     ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0)); // Transparent background

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    if (!m_Initialized)
    {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.2f, nullptr, &dock_main);
        ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.25f, nullptr, &dock_main);
        ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.25f, nullptr, &dock_main);

        // Create a new dock for the Properties, Audio Manager, and Inspector to be inside one dock
        ImGuiID dock_right_top = ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Up, 0.6f, nullptr, &dock_right);

        // Dock the windows into the same dock
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
        ImGui::DockBuilderDockWindow("Properties", dock_right_top);
        ImGui::DockBuilderDockWindow("Audio Manager", dock_right_top);
        ImGui::DockBuilderDockWindow("Inspector", dock_right_top);
        // Optionally: ImGui::DockBuilderDockWindow("Assets", dock_bottom);

        ImGui::DockBuilderFinish(dockspace_id);
        m_Initialized = true;
    }

    ImGui::End();
}
