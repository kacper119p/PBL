#include "FontId.h"

#include "TextManager.h"

namespace Engine::Ui
{

    Font* FontId::operator*() const
    {
        return TextManager::GetFont(*this);
    }

    const Font* FontId::operator->() const
    {
        return TextManager::GetFont(*this);
    }
}
