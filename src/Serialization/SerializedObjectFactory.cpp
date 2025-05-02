#include "SerializedObjectFactory.h"

namespace Serialization
{
    SerializedObject* SerializedObjectFactory::CreateObject(const std::string& TypeName)
    {

        const auto iterator = GetInstance()->Builders.find(TypeName);
        if (iterator == GetInstance()->Builders.end())
        {
            return nullptr;
        }
        return iterator->second->Build();
    }
} // Engine
