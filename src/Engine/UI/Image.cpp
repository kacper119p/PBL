#include "Image.h"

#include "Materials/Ui/UiMaterial.h"
#include "Utility/AssertionsUtility.h"

namespace Engine::Ui
{
    Image::Image() = default;

    void Image::Render()
    {
        CHECK_MESSAGE(Material != nullptr, "Material not set.");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture.GetId());
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        Material->Use();
        Material->GetMainPass().SetUniform("Transform", Rect.GetLocalToWorldMatrix());
        Quad.Draw();
        glDisable(GL_BLEND);
    }
}
