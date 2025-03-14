#include "SpotLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"

namespace Engine
{
    void SpotLight::OnAdd(Entity* NewOwner)
    {
        Component::OnAdd(NewOwner);
        LightManager::GetInstance()->RegisterLight(this);
        LightsGui::RegisterLight(this);
    }

    SpotLight::~SpotLight()
    {
        LightManager::GetInstance()->UnregisterLight(this);
        LightsGui::UnregisterLight(this);
    }
} // Engine
