#include "TextManager.h"

#include <cstdio>
#include <cstdlib>

#include "Utility/AssertionsUtility.h"

namespace Engine::Ui
{
    TextManager* TextManager::Instance = nullptr;

    TextManager::TextManager()
    {
    }

    void TextManager::Initialize()
    {
        CHECK_MESSAGE(Instance != nullptr, "TextRenderer already initialized");
        Instance = new TextManager();
    }

    void TextManager::Shutdown()
    {
    }
}
