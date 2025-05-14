#pragma once
#include <string>

#include "GameModeFactory.h"

namespace Engine
{
    template<class T>
    class GameModeRaii
    {
    private:
        std::string Name;

    public:
        explicit GameModeRaii(const std::string& Name) :
            Name(Name)
        {
            GameModeFactory::Register<T>(Name);
        }

    public:
        ~GameModeRaii()
        {
            GameModeFactory::Unregister(Name);
        }
    };

}
