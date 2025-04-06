#pragma once
#include <string>

#include "SerializedObjectFactory.h"

namespace Serialization
{
    template<class T>
    class SerializedObjectRaii
    {
        std::string Name;

    public:
        explicit SerializedObjectRaii(const std::string& Name) :
            Name(Name)
        {
            SerializedObjectFactory::Register<T>(Name);
        }

    public:
        ~SerializedObjectRaii()
        {
            SerializedObjectFactory::Unregister(Name);
        }
    };
} // Engine
