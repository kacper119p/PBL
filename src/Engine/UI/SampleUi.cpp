#include "SampleUi.h"

#include "Image.h"
#include "Engine/Textures/TextureManager.h"
#include "Materials/MaterialManager.h"
#include "Materials/Ui/BasicImageMaterial.h"

namespace Engine::Ui
{
    SampleUi::SampleUi()
    {
        Image* image = AddElement<Image>(nullptr);
        image->GetRect().SetPositionPixels(glm::vec3(-960, 540, 0));
        image->GetRect().SetSizePixels(glm::vec2(960, 540));
        Materials::UiMaterial* imageMaterial = new Materials::BasicImageMaterial();
        image->SetMaterial(imageMaterial);
        image->SetTexture(TextureManager::GetTexture("./res/textures/Box/Base.png"));
    }

    void SampleUi::Update(float DeltaTime)
    {
    }
}
