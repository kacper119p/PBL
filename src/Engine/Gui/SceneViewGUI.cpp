#include "SceneViewGUI.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
void Engine::SceneViewGUI::Draw()
{
    ImGui::Begin("Scene");

    ImVec2 avail = ImGui::GetContentRegionAvail();

    // Compute 16:9 aspect ratio size
    float aspectWidth = avail.x;
    float aspectHeight = aspectWidth * 9.0f / 16.0f;

    if (aspectHeight > avail.y)
    {
        aspectHeight = avail.y;
        aspectWidth = aspectHeight * 16.0f / 9.0f;
    }

    ImVec2 imageSize = ImVec2(aspectWidth, aspectHeight);

    // Get top-left corner position for the centered image
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 imagePos =
            ImVec2(cursorPos.x + (avail.x - imageSize.x) * 0.5f, cursorPos.y + (avail.y - imageSize.y) * 0.5f);

    // Set ImGuizmo rect (must be in screen space)
    ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

    // Draw the image (framebuffer)
    if (m_FramebufferTexture != 0)
    {
        ImGui::SetCursorScreenPos(imagePos); // move cursor to top-left of image
        ImGui::Image((ImTextureID) (uintptr_t) m_FramebufferTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));
    }
    else
    {
        ImGui::Text("No framebuffer texture set.");
    }

    ImGui::End();
}
