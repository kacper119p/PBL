#pragma once
#include "Engine/Components/Renderers/Renderer.h"

namespace Engine
{
    class RenderersCollection
    {
    private:
        std::unordered_map<Materials::Material*, std::vector<Renderer*>> Renderers;

    public:
        void AddRenderer(Renderer* Renderer);

        void RemoveRenderer(const Renderer* Renderer);

    public:
        [[nodiscard]] std::unordered_map<Materials::Material*, std::vector<Renderer*>>::iterator begin()
        {
            return Renderers.begin();
        }

        [[nodiscard]] std::unordered_map<Materials::Material*, std::vector<Renderer*>>::iterator end()
        {
            return Renderers.end();
        }

        [[nodiscard]] std::unordered_map<Materials::Material*, std::vector<Renderer*>>::const_iterator begin() const
        {
            return Renderers.begin();
        }

        [[nodiscard]] std::unordered_map<Materials::Material*, std::vector<Renderer*>>::const_iterator end() const
        {
            return Renderers.end();
        }
    };
} // Engine
