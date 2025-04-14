#pragma once
#include "UiSerializationFactory.h"

namespace Engine::Ui
{
    template<class T>
    class UiSerializationRaii
    {
    private:
        std::string Name;

    public:
        explicit UiSerializationRaii(const std::string& Name) :
            Name(Name)
        {
            UiSerializationFactory::Register<T>(Name);
        }

    public:
        ~UiSerializationRaii()
        {
            UiSerializationFactory::Unregister(Name);
        }
    };
}
