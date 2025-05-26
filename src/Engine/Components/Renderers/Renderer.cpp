#include "Renderer.h"
#include "Engine/EngineObjects/RenderingManager.h"

namespace Engine
{
    void Renderer::Start()
    {
        Component::Start();
        if (Material == nullptr)
        {
            return;
        }
        RenderingManager::GetInstance()->RegisterRenderer(this);
    }

    void Renderer::SetMaterial(Materials::Material* const Material)
    {
        if (Material == this->Material)
        {
            return;
        }
        RenderingManager::GetInstance()->UnregisterRenderer(this);
        this->Material = Material;
        if (Material == nullptr)
        {
            return;
        }
        RenderingManager::GetInstance()->RegisterRenderer(this);
    }

    Renderer::~Renderer()
    {
        RenderingManager::GetInstance()->UnregisterRenderer(this);
    }
} // Engine
