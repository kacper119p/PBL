#include "SceneViewGUI.h"
#include "imgui.h"
#include "imgui_internal.h"
void Engine::SceneViewGUI::Draw()
{
    ImGui::Begin("Scene");

    ImVec2 size = ImGui::GetContentRegionAvail();

    if (m_FramebufferTexture != 0)
    {
        ImGui::Text("Scene view");
        ImGui::Image((ImTextureID) (uintptr_t) m_FramebufferTexture, size, ImVec2(0, 1), ImVec2(1, 0));
    }
    else
    {
        ImGui::Text("No framebuffer texture set.");
    }

    ImGui::End();
}
