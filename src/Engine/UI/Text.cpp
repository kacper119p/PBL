#include "Text.h"

#include <glad/glad.h>

#include "FontRendering/FontVertex.h"
#include "FontRendering/TextManager.h"
#include "Shaders/ShaderManager.h"

namespace Engine::Ui
{

    Text::Text()
    {
        Shader = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Font/Font.vert", "", "./res/shaders/Font/Font.frag"));
    }

    void Text::Render()
    {
        if (VertexBuffer == 0)
        {
            return;
        }
        Shader.Use();
        Shader.SetUniform("Size", Rect.GetSize().y);
        Shader.SetUniform("Transform", Rect.GetLocalToWorldMatrixNoScale());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Font->GetGlyphAtlas());
        glBindVertexArray(VertexArray);
        glDrawArrays(GL_TRIANGLES, 0, VertexCount);
        glBindVertexArray(0);
    }

    void Text::UpdateMesh()
    {
        const class Font* font = *Font;
        VertexCount = static_cast<int32_t>(String.length() * 6);

        FontVertex* const vertices = new FontVertex[VertexCount];

        float x = 0.0f;
        float y = 0.0f;
        size_t j = 0;
        for (size_t i = 0; i < String.length(); i++)
        {
            const Glyph& glyph = font->GetGlyph(String[i]);
            if (glyph.GetCharacter() == ' ') // Optimize space out.
            {
                VertexCount -= 6;
                if (x * GetRect().GetSize().y >= GetRect().GetSize().x)
                {
                    y -= 1.0f;
                    x = 0.0f;
                }
                else
                {
                    x += glyph.GetAdvance();
                }
                continue;
            }
            vertices[j * 6 + 0].Position = glm::vec2(x + glyph.GetPlaneHorizontalBounds().x,
                                                     y + glyph.GetPlaneVerticalBounds().y);
            vertices[j * 6 + 0].TexCoords = glm::vec2(glyph.GetAtlasHorizontalBounds().x,
                                                      glyph.GetAtlasVerticalBounds().y);
            vertices[j * 6 + 1].Position = glm::vec2(x + glyph.GetPlaneHorizontalBounds().x,
                                                     y + glyph.GetPlaneVerticalBounds().x);
            vertices[j * 6 + 1].TexCoords = glm::vec2(glyph.GetAtlasHorizontalBounds().x,
                                                      glyph.GetAtlasVerticalBounds().x);
            vertices[j * 6 + 2].Position = glm::vec2(x + glyph.GetPlaneHorizontalBounds().y,
                                                     y + glyph.GetPlaneVerticalBounds().y);
            vertices[j * 6 + 2].TexCoords = glm::vec2(glyph.GetAtlasHorizontalBounds().y,
                                                      glyph.GetAtlasVerticalBounds().y);

            vertices[j * 6 + 3].Position = glm::vec2(x + glyph.GetPlaneHorizontalBounds().y,
                                                     y + glyph.GetPlaneVerticalBounds().y);
            vertices[j * 6 + 3].TexCoords = glm::vec2(glyph.GetAtlasHorizontalBounds().y,
                                                      glyph.GetAtlasVerticalBounds().y);
            vertices[j * 6 + 4].Position = glm::vec2(x + glyph.GetPlaneHorizontalBounds().x,
                                                     y + glyph.GetPlaneVerticalBounds().x);
            vertices[j * 6 + 4].TexCoords = glm::vec2(glyph.GetAtlasHorizontalBounds().x,
                                                      glyph.GetAtlasVerticalBounds().x);
            vertices[j * 6 + 5].Position = glm::vec2(x + glyph.GetPlaneHorizontalBounds().y,
                                                     y + glyph.GetPlaneVerticalBounds().x);
            vertices[j * 6 + 5].TexCoords = glm::vec2(glyph.GetAtlasHorizontalBounds().y,
                                                      glyph.GetAtlasVerticalBounds().x);

            x += glyph.GetAdvance();
            ++j;
        }


        glGenVertexArrays(1, &VertexArray);
        glGenBuffers(1, &VertexBuffer);
        glBindVertexArray(VertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(VertexCount * sizeof(FontVertex)), vertices,
                     GL_STATIC_DRAW);

        //Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              reinterpret_cast<void*>(offsetof(FontVertex, Position)));

        //TexCoords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              reinterpret_cast<void*>(offsetof(FontVertex, TexCoords)));

        glBindVertexArray(0);
        delete [] vertices;
    }

}
