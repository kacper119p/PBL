#include "PointLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"

namespace Engine
{
    void PointLight::OnAdd(Entity* NewOwner)
    {
        Component::OnAdd(NewOwner);
        LightManager::GetInstance()->RegisterLight(this);
        LightsGui::RegisterLight(this);
    }

    PointLight::~PointLight()
    {
        LightManager::GetInstance()->UnregisterLight(this);
        LightsGui::UnregisterLight(this);
    }
} // Engine
