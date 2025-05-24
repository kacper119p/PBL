#include "RenderersCollection.h"

namespace Engine
{
    void RenderersCollection::AddRenderer(Renderer* const Renderer)
    {
        const auto iterator = std::find(Renderers.begin(), Renderers.end(), Renderer->GetMaterial());
        if (iterator != Renderers.end())
        {
            iterator->Renderers.emplace_back(Renderer);
            return;
        }

        Pair& pair = Renderers.emplace_back(Renderer->GetMaterial(), std::vector<class Renderer*>());
        pair.Renderers.emplace_back(Renderer);
    }

    void RenderersCollection::RemoveRenderer(const Renderer* const Renderer)
    {
        const auto iterator = std::find(Renderers.begin(), Renderers.end(), Renderer->GetMaterial());
        if (iterator != Renderers.end())
        {
            std::erase(iterator->Renderers, Renderer);
            if (iterator->Renderers.empty())
            {
                Renderers.erase(iterator);
            }
        }
    }
}
