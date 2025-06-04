#include "CleaningUi.h"

#include <format>

#include "Engine/Components/BloodSystem/BloodManager.h"
#include "GLFW/glfw3.h"
#include "Engine/Textures/TextureManager.h"
#include "Materials/Ui/BasicImageMaterial.h"

namespace Engine::Ui
{
    CleaningUi::CleaningUi()
    {
        TaskListBackground = AddElement<Image>(nullptr);
        //TaskListBackground->GetRect().SetPositionPixels(glm::vec3(480, 0, 0));
        TaskListBackground->GetRect().SetPositionPixels(glm::vec3(1280, 0, 0));
        TaskListBackground->GetRect().SetSizePixels(glm::vec2(960, 880));
        Materials::UiMaterial* imageMaterial = new Materials::BasicImageMaterial();
        TaskListBackground->SetMaterial(imageMaterial);
        TaskListBackground->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/TaskListBackground.dds"));

        TrashText = AddElement<Text>(TaskListBackground);
        TrashText->GetRect().SetPositionPixels(glm::vec3(-180, 400, 0));
        TrashText->GetRect().SetSizePixels(glm::vec2(480, 100));
        TrashText->SetFont("Lato");
        TrashText->SetText("Clear trash xxx/xxx");

        FloorText = AddElement<Text>(TaskListBackground);
        FloorText->GetRect().SetPositionPixels(glm::vec3(-180, 270, 0));
        FloorText->GetRect().SetSizePixels(glm::vec2(480, 100));
        FloorText->SetFont("Lato");
        FloorText->SetText("Clear floor xx%");
    }

    void CleaningUi::Update(float DeltaTime)
    {
        float t = 0;
        glm::vec a = glm::vec3(600, 0, 0);
        glm::vec b = glm::vec3(1280, 0, 0);
        TaskListBackground->GetRect().SetPositionPixels(a + t * (b - a));

        if (BloodManager* bloodManager = BloodManager::GetCurrent())
        {
            const float bloodFill = bloodManager->GetBloodFill();
            if (bloodFill > ReferenceBloodFill)
            {
                ReferenceBloodFill = bloodFill;
            }
            FloorText->SetText(std::format("Clear floor {:.2f}%", (1.0f - bloodFill / ReferenceBloodFill) * 100.0f));
        }
    }
}
