#include "Updateable.h"

#include "Engine/EngineObjects/UpdateManager.h"

namespace Engine
{
    Updateable::~Updateable()
    {
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

    void Updateable::OnAdd(Entity* NewOwner)
    {
        Component::OnAdd(NewOwner);
        UpdateManager::GetInstance()->RegisterComponent(this);
    }
} // Engine
