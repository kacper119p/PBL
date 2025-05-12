#pragma once
#include <string>

#include "PlayerFactory.h"

namespace Engine
{
    template<class T>
    class PlayerRaii
    {
    private:
        std::string Name;

    public:
        explicit PlayerRaii(const std::string& Name) :
            Name(Name)
        {
            PlayerFactory::Register<T>(Name);
        }

    public:
        ~PlayerRaii()
        {
            PlayerFactory::Unregister(Name);
        }
    };

} // Engine
