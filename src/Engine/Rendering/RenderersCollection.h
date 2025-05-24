#pragma once
#include "Engine/Components/Renderers/Renderer.h"

namespace Engine
{
    class RenderersCollection
    {
    public:
        struct Pair
        {
            Materials::Material* Material;
            std::vector<Renderer*> Renderers;

            Pair(Materials::Material* Material, const std::vector<Renderer*>& Renderers) :
                Material(Material),
                Renderers(Renderers)
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
        void AddRenderer(Renderer* Renderer);

        void RemoveRenderer(const Renderer* Renderer);

    public:
        [[nodiscard]] std::vector<Pair>::iterator begin()
        {
            return Renderers.begin();
        }

        [[nodiscard]] std::vector<Pair>::iterator end()
        {
            return Renderers.end();
        }

        [[nodiscard]] std::vector<Pair>::const_iterator begin() const
        {
            return Renderers.begin();
        }

        [[nodiscard]] std::vector<Pair>::const_iterator end() const
        {
            return Renderers.end();
        }
    };
} // Engine
