#pragma once
#include <string>

#include "MaterialManager.h"

namespace Materials
{
    template<class T>
    class MaterialRaii
    {
        std::string Name;

    public:
        explicit MaterialRaii(const std::string& Name) :
            Name(Name)
        {
            MaterialManager::GetMaterialFactory()->Register<T>(Name);
        }

    public:
        ~MaterialRaii()
        {
            MaterialManager::GetMaterialFactory()->Unregister(Name);
        }
    };
} // Materials
