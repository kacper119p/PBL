#include "SampleUi.h"
#include "GLFW/glfw3.h"
#include "Engine/Textures/TextureManager.h"
#include "Engine/UI/Image.h"
#include "Engine/UI/Text.h"
#include "Materials/MaterialManager.h"
#include "Materials/Ui/BasicImageMaterial.h"

namespace Engine::Ui
{
    SampleUi::SampleUi()
    {
        Image* image = AddElement<Image>(nullptr);
        image->GetRect().SetPositionPixels(glm::vec3(480, 270, 0));
        image->GetRect().SetSizePixels(glm::vec2(960, 540));
        Materials::UiMaterial* imageMaterial = new Materials::BasicImageMaterial();
        image->SetMaterial(imageMaterial);
        image->SetTexture(TextureManager::GetTexture("./res/textures/Logos/WIR_logo_2.png"));

        MovingImage = AddElement<Image>(nullptr);
        MovingImage->GetRect().SetPositionPixels(glm::vec3(720, -200, 0.5));
        MovingImage->GetRect().SetSizePixels(glm::vec2(480, 270));
        MovingImage->SetMaterial(imageMaterial);
        MovingImage->SetTexture(TextureManager::GetTexture("./res/textures/Logos/WIR_logo_2.png"));

        ChangingText = AddElement<Text>(nullptr);
        ChangingText->GetRect().SetPositionPixels(glm::vec3(-960, -480, 0.5));
        ChangingText->GetRect().SetSizePixels(glm::vec2(480, 180));
        ChangingText->SetFont("Lato");
        ChangingText->SetText("Changing");

        ChangingText2 = AddElement<Text>(nullptr);
        ChangingText2->GetRect().SetPositionPixels(glm::vec3(-960, -380, 0.5));
        ChangingText2->GetRect().SetSizePixels(glm::vec2(480, 72));
        ChangingText2->SetFont("Lato");
        ChangingText2->SetText("Changing");

        Text* text = AddElement<Text>(nullptr);
        text->GetRect().SetPositionPixels(glm::vec3(-960, 468, 0));
        text->GetRect().SetSizePixels(glm::vec2(960, 72));
        text->SetFont("Lato");
        text->SetText(
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    }

    void SampleUi::Update(const float DeltaTime)
    {
        MovingImage->Rect.SetPositionPixels(glm::vec3(720 * std::cos(glfwGetTime()), -200, 0));
        MovingImage->Rect.SetRotation(glfwGetTime() * 180);
        MovingImage->GetRect().SetSizePixels(
                glm::vec2(480, 270) * static_cast<float>(std::cos(glfwGetTime()) * 0.25f + 0.75));
        Timer += DeltaTime;
        if (Timer > 1.0f)
        {
            Timer = 0.0f;
            if (Change == 0)
            {
                Change = 1;
                ChangingText->SetText("Text");
                ChangingText2->SetText("Text");
            }
            else
            {
                Change = 0;
                ChangingText->SetText("Changing");
                ChangingText2->SetText("Changing");
            }
        }
    }
}
