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

    void ThrashManager::RemoveCoin(Entity* coin)
    {
        auto it = std::find(Coins.begin(), Coins.end(), coin);
        if (it != Coins.end())
        {
            Coins.erase(it);
        }
    }

    void ThrashManager::RemoveWeapon(Entity* weapon)
    {
        auto it = std::find(Weapons.begin(), Weapons.end(), weapon);
        if (it != Weapons.end())
        {
            Weapons.erase(it);
        }
    }

    void ThrashManager::RemoveBook(Entity* book)
    {
        auto it = std::find(Books.begin(), Books.end(), book);
        if (it != Books.end())
        {
            Books.erase(it);
        }
    }

    void ThrashManager::RemoveCleanedUpCoin(Entity* coin)
    {
        auto it = std::find(CleanedUpCoins.begin(), CleanedUpCoins.end(), coin);
        if (it != CleanedUpCoins.end())
        {
            CleanedUpCoins.erase(it);
        }
    }

    void ThrashManager::RemoveCleanedUpWeapon(Entity* weapon)
    {
        auto it = std::find(CleanedUpWeapons.begin(), CleanedUpWeapons.end(), weapon);
        if (it != CleanedUpWeapons.end())
        {
            CleanedUpWeapons.erase(it);
        }
    }

    void ThrashManager::RemoveCleanedUpBook(Entity* book)
    {
        auto it = std::find(CleanedUpBooks.begin(), CleanedUpBooks.end(), book);
        if (it != CleanedUpBooks.end())
        {
            CleanedUpBooks.erase(it);
        }
    }
} // namespace Engine
