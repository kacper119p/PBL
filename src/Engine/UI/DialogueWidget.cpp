#include "DialogueWidget.h"

#include "Engine/Textures/TextureManager.h"
#include "Materials/MaterialManager.h"
#include "Materials/Ui/BasicImageMaterial.h"
#include "Math/Easings.h"

namespace Engine::Ui
{
    DialogueWidget::DialogueWidget()
    {
        Materials::UiMaterial* imageMaterial = new Materials::BasicImageMaterial();

        Image = new class Image();
        Image->GetRect().SetParent(&GetRect());
        Image->SetTexture(TextureManager::GetTexture("res/textures/UI/Tekst.dds"));
        Image->SetMaterial(imageMaterial);

        PlayerImage = new class Image();
        PlayerImage->GetRect().SetParent(&GetRect());
        PlayerImage->SetTexture(TextureManager::GetTexture("res/textures/UI/Gryzia.dds"));
        PlayerImage->SetMaterial(imageMaterial);

        BossImage = new class Image();
        BossImage->GetRect().SetParent(&GetRect());
        BossImage->SetTexture(TextureManager::GetTexture("res/textures/UI/Szrajber.dds"));
        BossImage->SetMaterial(imageMaterial);

        PlayerImage->GetRect().SetPositionPixels(glm::vec3(HiddenPositionImage));
        PlayerImage->GetRect().SetSizePixels(glm::vec2(300, 300));
        PlayerImage->GetRect().SetPositionPixels(ShownPositionImage + glm::vec3(-750, 150, 0));

        BossImage->GetRect().SetPositionPixels(glm::vec3(HiddenPositionImage));
        BossImage->GetRect().SetSizePixels(glm::vec2(300, 300));
        BossImage->GetRect().SetPositionPixels(ShownPositionImage + glm::vec3(750, 150, 0));

        Image->GetRect().SetPositionPixels(glm::vec3(HiddenPositionImage));
        Image->GetRect().SetSizePixels(glm::vec2(1470, 144 + 50));

        Text = new class Text();
        Text->SetFont("EagleLakeRegular");
        Text->GetRect().SetParent(&GetRect());

        Text->GetRect().SetPositionPixels(glm::vec3(HiddenPositionText));
        Text->GetRect().SetSizePixels(glm::vec2(1200 * 2, 52));
    }

    DialogueWidget::~DialogueWidget()
    {
        delete Image->GetMaterial();
        delete Image;
        delete PlayerImage;
        delete BossImage;
        delete Text;
    }

    void DialogueWidget::Update(const float DeltaTime)
    {
        if (FinishedAnimation)
        {
            return;
        }
        Timer += DeltaTime;

        switch (CurrentAnimation)
        {
            case AnimationType::TextEnter:
            {
                if (!FinishedAnimation)
                {

                    const int32_t characterCount = static_cast<int32_t>(Timer / TimePerCharacter);
                    const std::string str = CurrentText.substr(0, characterCount);
                    if (Text->GetText() != str)
                    {
                        Text->SetText(str);
                        if (characterCount >= CurrentText.length())
                        {
                            Timer = 0.0;
                            CurrentAnimation = AnimationType::TextWait;
                        }
                    }
                }
                break;
            }
            case AnimationType::TextWait:
            {
                if (Timer >= WaitTimeTime)
                {
                    SetSpeaker(Speaker::None);
                    FinishedAnimation = true;
                }
                break;
            }
            case AnimationType::Enter:
            {
                float time = Timer / TransitionTime;
                time = Math::EaseOutBack(time);
                Text->GetRect().SetPositionPixels(glm::mix(HiddenPositionText, ShownPositionText, time));
                Image->GetRect().SetPositionPixels(glm::mix(HiddenPositionImage, ShownPositionImage, time));
                if (Timer >= TransitionTime)
                {
                    FinishedAnimation = true;
                }
                break;
            }
            case AnimationType::Exit:
            {
                float time = 1.0f - Timer / TransitionTime;
                time = Math::EaseOutBack(time);
                Text->GetRect().SetPositionPixels(glm::mix(HiddenPositionText, ShownPositionText, time));
                Image->GetRect().SetPositionPixels(glm::mix(HiddenPositionImage, ShownPositionImage, time));
                if (Timer >= TransitionTime)
                {
                    Text->SetText(std::string());
                    FinishedAnimation = true;
                }
                break;
            }
        }
    }

    void DialogueWidget::Render()
    {
        if (!Active)
        {
            return;
        }
        Image->Render();
        Text->Render();
        if (ShowPlayer)
        {
            PlayerImage->Render();
        }
        if (ShowBoss)
        {
            BossImage->Render();
        }
    }
}
