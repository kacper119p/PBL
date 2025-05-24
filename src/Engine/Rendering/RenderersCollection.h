#pragma once
#include "Engine/Components/Renderers/ParticleEmitter.h"
#include "Engine/Components/Renderers/Renderer.h"

namespace Engine
{
    template<class T>
    class TRenderersCollection
    {
    public:
        struct Pair
        {
            Materials::Material* Material;
            std::vector<T*> Renderers;

            Pair(Materials::Material* Material) :
                Material(Material),
                Renderers(std::vector<T*>())
            {
            }

            friend bool operator==(const Pair& Lhs, const Materials::Material* const Rhs)
            {
                return Lhs.Material == Rhs;
            }

            friend bool operator!=(const Pair& Lhs, const Materials::Material* const Rhs)
            {
                return !(Lhs == Rhs);
            }
        };

    private:
        std::vector<Pair> Renderers;

    public:
        void AddRenderer(T* const Renderer)
        {
            const auto iterator = std::find(Renderers.begin(), Renderers.end(), Renderer->GetMaterial());
            if (iterator != Renderers.end())
            {
                iterator->Renderers.emplace_back(Renderer);
                return;
            }

            Pair& pair = Renderers.emplace_back(Renderer->GetMaterial());
            pair.Renderers.emplace_back(Renderer);
        }

        void RemoveRenderer(const T* const Renderer)
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

    public:
        [[nodiscard]] typename std::vector<Pair>::iterator begin()
        {
            return Renderers.begin();
        }

        [[nodiscard]] typename std::vector<Pair>::iterator end()
        {
            return Renderers.end();
        }

        [[nodiscard]] typename std::vector<Pair>::const_iterator begin() const
        {
            return Renderers.begin();
        }

        [[nodiscard]] typename std::vector<Pair>::const_iterator end() const
        {
            return Renderers.end();
        }
    };

    typedef TRenderersCollection<Renderer> RenderersCollection;
    typedef TRenderersCollection<ParticleEmitter> ParticleEmittersCollection;
}
