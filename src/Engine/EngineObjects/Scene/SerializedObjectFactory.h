#pragma once
#include "Serialization/SerializedObject.h"

namespace Engine
{

    class SerializedObjectFactory
    {
    public:
        static Serialization::SerializedObject* CreateObject(const rapidjson::Value& Object);
    };

} // Engine
