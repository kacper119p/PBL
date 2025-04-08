#include "SampleUi.h"

#include "Image.h"
#include "Text.h"
#include "GLFW/glfw3.h"
#include "Engine/Textures/TextureManager.h"
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
        MovingImage->GetRect().SetPositionPixels(glm::vec3(720, 0, 0.5));
        MovingImage->GetRect().SetSizePixels(glm::vec2(480, 270));
        MovingImage->SetMaterial(imageMaterial);
        MovingImage->SetTexture(TextureManager::GetTexture("./res/textures/Logos/WIR_logo_2.png"));

        ChangingText = AddElement<Text>(nullptr);
        ChangingText->GetRect().SetPositionPixels(glm::vec3(-720, 0, 0.5));
        ChangingText->GetRect().SetSizePixels(glm::vec2(480, 270));
        ChangingText->SetFont("Lato");
        ChangingText->SetText("Changing");
    }

    void SampleUi::Update(float DeltaTime)
    {
        MovingImage->Rect.SetPositionPixels(glm::vec3(720 * std::cos(glfwGetTime()), 0, 0));
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
            }
            else
            {
                Change = 0;
                ChangingText->SetText("Changing");
            }
        }
    }
}
