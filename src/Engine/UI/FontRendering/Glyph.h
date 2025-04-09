#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Engine::Ui
{

    class Glyph
    {
    private:
        char Character;
        float Advance;
        glm::vec2 PlaneHorizontalBounds;
        glm::vec2 PlaneVerticalBounds;
        glm::vec2 AtlasHorizontalBounds;
        glm::vec2 AtlasVerticalBounds;

    public:
        Glyph(const char Character, const float Advance, const glm::vec2& PlaneHorizontalBounds,
              const glm::vec2& PlaneVerticalBounds, const glm::vec2& AtlasHorizontalBounds,
              const glm::vec2& AtlasVerticalBounds) :
            Character(Character),
            Advance(Advance),
            PlaneHorizontalBounds(PlaneHorizontalBounds),
            PlaneVerticalBounds(PlaneVerticalBounds),
            AtlasHorizontalBounds(AtlasHorizontalBounds),
            AtlasVerticalBounds(AtlasVerticalBounds)
        {
        }

    public:
        [[nodiscard]] char GetCharacter() const
        {
            return Character;
        }

        [[nodiscard]] float GetAdvance() const
        {
            return Advance;
        }

        [[nodiscard]] glm::vec2 GetPlaneHorizontalBounds() const
        {
            return PlaneHorizontalBounds;
        }

        [[nodiscard]] glm::vec2 GetPlaneVerticalBounds() const
        {
            return PlaneVerticalBounds;
        }

        [[nodiscard]] glm::vec2 GetAtlasHorizontalBounds() const
        {
            return AtlasHorizontalBounds;
        }

        [[nodiscard]] glm::vec2 GetAtlasVerticalBounds() const
        {
            return AtlasVerticalBounds;
        }
    };

}
