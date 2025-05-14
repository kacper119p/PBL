#if EDITOR
#include "MaterialsMenu.h"

#include "imgui.h"
#include "Materials/MaterialManager.h"
void Engine::MaterialsMenu::DrawMaterialEditor() 
{ 
   ImGui::Begin("Material Creator");
   static char text[128] = "NewMaterial.mat"; 
   ImGui::InputText("##text_input", text, IM_ARRAYSIZE(text));

   static std::string selectedMaterialType = "Select Material Type";
   static std::vector<std::string> materialTypes = Materials::MaterialManager::GetMaterialFactory()->GetAvailableMaterialTypes();

   if (ImGui::BeginCombo("Material Type", selectedMaterialType.c_str()))
   {
       for (const auto& materialType : materialTypes)
       {
           bool isSelected = (selectedMaterialType == materialType);
           if (ImGui::Selectable(materialType.c_str(), isSelected))
           {
               selectedMaterialType = materialType;
           }
           if (isSelected)
           {
               ImGui::SetItemDefaultFocus();
           }
       }
       ImGui::EndCombo();
   }

   if (ImGui::Button("Create New Material"))
   {
       if (!selectedMaterialType.empty() && selectedMaterialType != "Select Material Type")
       {
           char path[128];
           snprintf(path, sizeof(path), "./res/Materials/SampleScene/%s", text);
           Materials::Material* newMat = CreateMaterialAsset(selectedMaterialType, path);
           Materials::MaterialManager::SaveMaterial(path, newMat);
       }
   }
   ImGui::End();
}

Materials::Material* Engine::MaterialsMenu::CreateMaterialAsset(const std::string& typeName, const std::string& path)
{
    auto* builder = Materials::MaterialManager::GetMaterialFactory()->GetBuilder(typeName);
    if (!builder)
    {
        // Handle error: material type not registered
        return nullptr;
    }

    Materials::Material* material = builder->Build();
    if (!material)
    {
        // Handle error: failed to construct material
        return nullptr;
    }
    return material;
}
#endif
