#include "SerializedObjectFactory.h"

namespace Serialization
{
    SerializedObject* SerializedObjectFactory::CreateObject(const rapidjson::Value& Object)
    {
        const std::string typeName = Object["type"].GetString();
        const auto iterator = GetInstance()->Builders.find(typeName);
        if (iterator == GetInstance()->Builders.end())
        {
            return nullptr;
        }
        return iterator->second->Build();
    }
} // Engine
