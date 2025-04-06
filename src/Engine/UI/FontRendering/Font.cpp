#include "Font.h"

#include <rapidjson/document.h>

#include "Serialization/SerializationUtility.h"
#include "Utility/TextureUtilities.h"

namespace Engine::Ui
{
    std::string Font::FontsFolder = "./res/fonts";

    Font::Font(const std::string& Name)
    {
        const std::string atlasPath = FontsFolder + Name + ".png";
        const std::string descriptorPath = FontsFolder + Name + ".json";

        rapidjson::Document document;

        int atlasWidth;
        int atlasHeight;
        GlyphAtlas = Utility::LoadTexture2DFromFile(atlasPath.c_str(), GL_RGBA, 4,GL_RGBA, atlasWidth, atlasHeight);

        Serialization::ReadJsonFile(descriptorPath.c_str(), document);

        CHECK_MESSAGE(document.IsObject() && document.HasMember("glyphs"), "Invalid format.");

        rapidjson::GenericArray glyphsJson = document["glyphs"].GetArray();


        for (const rapidjson::Value& glyphJson : glyphsJson)
        {
            char character = static_cast<char>(glyphJson["unicode"].GetInt());
            float advance = glyphJson["advance"].GetFloat();
            const rapidjson::Value& planeBounds = glyphJson["planeBounds"].GetObj();
            glm::vec2 horizontalPlaneBounds(planeBounds["left"].GetFloat(), planeBounds["right"].GetFloat());
            glm::vec2 verticalPlaneBounds(planeBounds["top"].GetFloat(), planeBounds["bottom"].GetFloat());
            const rapidjson::Value& atlasBounds = glyphJson["atlasBounds"].GetObj();
            glm::vec2 horizontalAtlasBounds(planeBounds["left"].GetFloat() / static_cast<float>(atlasWidth),
                                            planeBounds["right"].GetFloat() / static_cast<float>(atlasWidth));
            glm::vec2 verticalAtlasBounds(planeBounds["top"].GetFloat() / static_cast<float>(atlasHeight),
                                          planeBounds["bottom"].GetFloat() / static_cast<float>(atlasHeight));

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
