#include "Entity.h"


namespace Engine
{
    Entity::~Entity()
    {
        for (const Component* component : Components)
        {
            delete component;
        }
    }
}
