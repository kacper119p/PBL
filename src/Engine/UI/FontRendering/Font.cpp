#include "Font.h"

#include <rapidjson/document.h>

#include "Serialization/SerializationUtility.h"
#include "Utility/TextureUtilities.h"

namespace Engine::Ui
{
    std::string Font::FontsFolder = "./res/fonts/";

    Font::Font(const std::string& Name) :
        Name(Name)
    {
        const std::string atlasPath = FontsFolder + Name + ".dds";
        const std::string descriptorPath = FontsFolder + Name + ".json";

        rapidjson::Document document;

        int atlasWidth;
        int atlasHeight;
        GlyphAtlas = Utility::LoadTexture2DFromFile(atlasPath.c_str(), GL_RGBA, 4,GL_RGBA, atlasWidth, atlasHeight);

        Serialization::ReadJsonFile(descriptorPath.c_str(), document);

        CHECK_MESSAGE(document.IsObject() && document.HasMember("glyphs"), "Invalid format.");

        LineHeight = document["metrics"]["lineHeight"].GetFloat();

        rapidjson::GenericArray glyphsJson = document["glyphs"].GetArray();


        for (const rapidjson::Value& glyphJson : glyphsJson)
        {
            char character = static_cast<char>(glyphJson["unicode"].GetInt());
            float advance = glyphJson["advance"].GetFloat();
            glm::vec2 horizontalPlaneBounds(0.0f, 0.0f);
            glm::vec2 verticalPlaneBounds(0.0f, 0.0f);
            const auto planeBoundsIterator = glyphJson.FindMember("planeBounds");
            if (planeBoundsIterator != glyphJson.MemberEnd())
            {
                const rapidjson::Value& planeBounds = planeBoundsIterator->value.GetObj();
                horizontalPlaneBounds = glm::vec2(planeBounds["left"].GetFloat(), planeBounds["right"].GetFloat());
                verticalPlaneBounds = glm::vec2(-planeBounds["bottom"].GetFloat(), -planeBounds["top"].GetFloat());
            }

            glm::vec2 horizontalAtlasBounds(0.0f, 0.0f);
            glm::vec2 verticalAtlasBounds(0.0f, 0.0f);
            const auto atlasBoundsIterator = glyphJson.FindMember("atlasBounds");
            if (atlasBoundsIterator != glyphJson.MemberEnd())
            {
                const rapidjson::Value& atlasBounds = atlasBoundsIterator->value.GetObj();
                horizontalAtlasBounds = glm::vec2(atlasBounds["left"].GetFloat() / static_cast<float>(atlasWidth),
                                                  atlasBounds["right"].GetFloat() / static_cast<float>(atlasWidth));
                verticalAtlasBounds = glm::vec2(atlasBounds["bottom"].GetFloat() / static_cast<float>(atlasHeight),
                                                atlasBounds["top"].GetFloat() / static_cast<float>(atlasHeight));
            }

            Glyphs.emplace(character,
                           Glyph(character,
                                 advance,
                                 horizontalPlaneBounds,
                                 verticalPlaneBounds,
                                 horizontalAtlasBounds,
                                 verticalAtlasBounds));
        }
    }
}
