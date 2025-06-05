#include "ThrashManager.h"
namespace Engine
{

    ThrashManager* ThrashManager::GetInstance()
    {
        if (!Instance)
        {
            Instance = new ThrashManager();
        }
        return Instance;
    }

    void ThrashManager::RemoveThrash(Engine::Thrash* thrash)
    {
        auto it = std::find(Thrashes.begin(), Thrashes.end(), thrash);
        if (it != Thrashes.end())
        {
            Thrashes.erase(it);
        }
    }
    void ThrashManager::RemoveFurniture(Furniture* furniture)
    {
        auto it = std::find(Furnitures.begin(), Furnitures.end(), furniture);
        if (it != Furnitures.end())
        {
            Furnitures.erase(it);
        }
    }
} // namespace Engine
