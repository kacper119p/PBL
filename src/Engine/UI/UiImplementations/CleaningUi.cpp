#include "CleaningUi.h"

#include <format>

#include "Engine/Components/BloodSystem/BloodManager.h"
#include "Engine/Components/Game/ThrashManager.h"
#include "Engine/Input/InputManager.h"
#include "GLFW/glfw3.h"
#include "Engine/Textures/TextureManager.h"
#include "Materials/Ui/BasicImageMaterial.h"
#include "Math/Easings.h"

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
        TrashText->SetText("Clear trash xx/xx");

        FloorText = AddElement<Text>(TaskListBackground);
        FloorText->GetRect().SetPositionPixels(glm::vec3(-180, 270, 0));
        FloorText->GetRect().SetSizePixels(glm::vec2(480, 100));
        FloorText->SetFont("Lato");
        FloorText->SetText("Clear floor xx%");
    }

    void CleaningUi::Update(float DeltaTime)
    {
        const bool hideKeyPressed = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_J)
                                    || InputManager::GetInstance().IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_Y);

        if (hideKeyPressed && !HideKeyPressedLastFrame)
        {
            Hidden = !Hidden;
        }

        HideKeyPressedLastFrame = hideKeyPressed;

        HiddenTime += (Hidden ? 1.0f : -1.0f) * DeltaTime / 0.66f;
        HiddenTime = std::min<float>(std::max<float>(HiddenTime, 0.0f), 1.0f);
        constexpr glm::vec a = glm::vec3(600, 0, 0);
        constexpr glm::vec b = glm::vec3(1280, 0, 0);
        const float t = Math::EaseInOutBack(HiddenTime);
        TaskListBackground->GetRect().SetPositionPixels(a + t * (b - a));

        if (const BloodManager* bloodManager = BloodManager::GetCurrent())
        {
            const float bloodFill = bloodManager->GetBloodFill();
            if (bloodFill > ReferenceBloodFill)
            {
                ReferenceBloodFill = bloodFill;
            }
            FloorText->SetText(std::format("Clear floor {:.2f}%", (1.0f - bloodFill / ReferenceBloodFill) * 100.0f));
        }

        if (ThrashManager* trashManager = ThrashManager::GetInstance())
        {
            const int32_t trashCount = trashManager->GetThrashCount();
            if (trashCount > ReferenceTrashCount)
            {
                ReferenceTrashCount = trashCount;
            }
            TrashText->SetText(std::format("Clear trash {:02}/{:02}", trashCount, ReferenceTrashCount));
        }
    }
}
