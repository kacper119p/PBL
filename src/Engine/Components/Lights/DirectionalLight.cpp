#include "DirectionalLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"

namespace Engine
{
    void DirectionalLight::OnAdd(Entity* NewOwner)
    {
        Component::OnAdd(NewOwner);
        LightManager::GetInstance()->RegisterLight(this);
        LightsGui::RegisterLight(this);
    }

    DirectionalLight::DirectionalLight() = default;

    DirectionalLight::~DirectionalLight()
    {
        LightManager::GetInstance()->UnregisterLight(this);
        LightsGui::UnregisterLight(this);
    }
} // Engine
