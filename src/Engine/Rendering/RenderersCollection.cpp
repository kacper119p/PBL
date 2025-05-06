#include "RenderersCollection.h"

namespace Engine
{
    void RenderersCollection::AddRenderer(Renderer* const Renderer)
    {
        const auto iterator = Renderers.find(Renderer->GetMaterial());
        if (iterator != Renderers.end())
        {
            iterator->second.emplace_back(Renderer);
            return;
        }

        const auto pair = Renderers.emplace(Renderer->GetMaterial(), std::vector<class Renderer*>());
        pair.first->second.emplace_back(Renderer);
    }

    void RenderersCollection::RemoveRenderer(const Renderer* const Renderer)
    {
        const auto iterator = Renderers.find(Renderer->GetMaterial());
        if (iterator != Renderers.end())
        {
            std::erase(iterator->second, Renderer);
            if (iterator->second.empty())
            {
                Renderers.erase(iterator);
            }
        }
    }
} // Engine
