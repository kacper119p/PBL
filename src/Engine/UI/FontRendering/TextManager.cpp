#include "TextManager.h"

#include <cstdio>
#include <cstdlib>

#include "Font.h"
#include "Utility/AssertionsUtility.h"

namespace Engine::Ui
{
    TextManager* TextManager::Instance = nullptr;

    TextManager::TextManager() = default;

    void TextManager::Initialize()
    {
        CHECK_MESSAGE(Instance == nullptr, "TextRenderer already initialized");
        Instance = new TextManager();
    }

    void TextManager::Shutdown()
    {
        delete Instance;
    }

    FontId TextManager::GetFontId(const std::string& Name)
    {
        for (uint8_t i = 0; i < Instance->FontTopPointer; i++)
        {
            if (Name == Instance->Fonts[i].GetName())
            {
                return FontId(i + 1);
            }
        }

        Instance->Fonts[Instance->FontTopPointer] = Font(Name);
        Instance->FontTopPointer++;
        return FontId(Instance->FontTopPointer);
    }

    Font* TextManager::GetFont(const FontId Id)
    {
        const uint8_t id = Id.GetId() - 1;
        CHECK_MESSAGE(id < Instance->FontTopPointer, "Id out of bounds")
        return &Instance->Fonts[id];
    }
}
