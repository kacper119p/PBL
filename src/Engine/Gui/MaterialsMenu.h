#pragma once
#if EDITOR
#include <string>
#include "Materials/Material.h"
namespace Engine
{
    class MaterialsMenu
    {
    public:
        void DrawMaterialEditor();
        static Materials::Material* CreateMaterialAsset(const std::string& typeName, const std::string& path);
    };
    }// namespace Engine
#endif
