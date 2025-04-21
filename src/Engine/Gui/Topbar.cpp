#if EDITOR
#include "TopBar.h"
#include "imgui.h"

void TopBar::Draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                // Handle New
            }
            if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S"))
            {
                // Handle New
            }
            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Materials"))
        {
            if (ImGui::MenuItem("Create/Edit"))
            {
                // Handle New
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
#endif
