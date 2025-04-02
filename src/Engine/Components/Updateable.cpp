#include "Updateable.h"

#include "Engine/EngineObjects/UpdateManager.h"

namespace Engine
{
    Updateable::~Updateable()
    {
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

    void Updateable::Start()
    {
        Component::Start();
        UpdateManager::GetInstance()->RegisterComponent(this);
    }
} // Engine
