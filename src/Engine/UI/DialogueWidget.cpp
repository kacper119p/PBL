#include "DialogueWidget.h"

#include "Engine/Textures/TextureManager.h"
#include "Materials/MaterialManager.h"
#include "Materials/Ui/BasicImageMaterial.h"

namespace Engine::Ui
{
    DialogueWidget::DialogueWidget()
    {
        Materials::UiMaterial* imageMaterial = new Materials::BasicImageMaterial();

        Image = new class Image();
        Image->GetRect().SetParent(&GetRect());
        Image->SetTexture(TextureManager::GetTexture("res/textures/Default.dds"));
        Image->SetMaterial(imageMaterial);

        Image->GetRect().SetPositionPixels(glm::vec3(0, -468 + 25, 0.5));
        Image->GetRect().SetSizePixels(glm::vec2(1470, 144 + 50));

        Text = new class Text();
        Text->SetFont("EagleLakeRegular");
        Text->GetRect().SetParent(&GetRect());

        Text->GetRect().SetPositionPixels(glm::vec3(-700, -308, 0.5));
        Text->GetRect().SetSizePixels(glm::vec2(1400 * 2, 70));
    }

    DialogueWidget::~DialogueWidget()
    {
        delete Image->GetMaterial();
        delete Image;
        delete Text;
    }

    void DialogueWidget::Update(const float DeltaTime)
    {
        if (!FinishedAnimation)
        {
            Timer += DeltaTime;
            const int32_t characterCount = static_cast<int32_t>(Timer / TimePerCharacter);
            const std::string str = CurrentText.substr(0, characterCount);
            if (Text->GetText() != str)
            {
                Text->SetText(str);
                if (characterCount == CurrentText.length())
                {
                    FinishedAnimation = true;
                }
            }
        }
    }

    void DialogueWidget::Render()
    {
        Image->Render();
        Text->Render();
    }
}
