#include "SampleUi (2).h"
#include "GLFW/glfw3.h"
#include "Engine/Textures/TextureManager.h"
#include "Engine/UI/Image.h"
#include "Engine/UI/Text.h"
#include "Materials/MaterialManager.h"
#include "Materials/Ui/BasicImageMaterial.h"
#include "Engine/Components/Game/ThrashManager.h"

namespace Engine::Ui
{
    SampleUi2::SampleUi2()
    {
        ThrashCount = AddElement<Text>(nullptr);
        ThrashCount->GetRect().SetPositionPixels(glm::vec3(-960, 540, 0));
        ThrashCount->GetRect().SetSizePixels(glm::vec2(960, 64));
        ThrashCount->SetFont("Lato");
        int thrashCount = ThrashManager::GetInstance()->GetThrashCount();
        ThrashCount->SetText(std::to_string(thrashCount));
        
        VacuumCount = AddElement<Text>(nullptr);
        VacuumCount->GetRect().SetPositionPixels(glm::vec3(-960, 640, 0));
        VacuumCount->GetRect().SetSizePixels(glm::vec2(960, 64));
        VacuumCount->SetFont("Lato");
        int vacuumCount = ThrashManager::GetInstance()->VacuumCount;
        VacuumCount->SetText(std::to_string(vacuumCount));
        
        VacuumVolume = AddElement<Text>(nullptr);
        VacuumVolume->GetRect().SetPositionPixels(glm::vec3(-960, 740, 0));
        VacuumVolume->GetRect().SetSizePixels(glm::vec2(960, 64));
        VacuumVolume->SetFont("Lato");
        int vacuumVolume = ThrashManager::GetInstance()->VacuumVolume;
        VacuumVolume->SetText(std::to_string(vacuumVolume));
    }

    void SampleUi2::Update(const float DeltaTime)
    {
        int thrashCount = ThrashManager::GetInstance()->GetThrashCount();
        ThrashCount->SetText(std::to_string(thrashCount));

        int vacuumCount = ThrashManager::GetInstance()->VacuumCount;
        VacuumCount->SetText(std::to_string(vacuumCount));

        int vacuumVolume = ThrashManager::GetInstance()->VacuumVolume;
        VacuumVolume->SetText(std::to_string(vacuumVolume));
    }
}
