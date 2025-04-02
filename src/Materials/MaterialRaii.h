#pragma once
#include <string>

#include "MaterialManager.h"

namespace Materials
{
    /**
     * @brief Handle for materials to register to MaterialInitializer and MaterialFactory.
     * @tparam T Material's class.
     */
    template<class T>
    class MaterialRaii
    {
        std::string Name;

    public:
        /**
         * @brief Registers material to MaterialInitializer and MaterialFactory.
         * @param Name Material's TypeName.
         */
        explicit MaterialRaii(const std::string& Name) :
            Name(Name)
        {
            MaterialManager::GetMaterialFactory()->Register<T>(Name);
            MaterialManager::GetMaterialInitializer().Register<T>();
        }

    public:
        ~MaterialRaii()
        {
            MaterialManager::GetMaterialFactory()->Unregister(Name);
            MaterialManager::GetMaterialInitializer().Unregister<T>();
        }
    };
} // Materials
