#include "Component.h"

void Engine::Component::OnAdd(Engine::Entity* NewOwner)
{
    Owner = NewOwner;
}
