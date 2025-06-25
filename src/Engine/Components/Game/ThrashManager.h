#pragma once

#include "Engine/Components/Game/Furniture.h"
#include <vector>
namespace Engine
{
    class Thrash;
    class ThrashManager
    {
    private:
        static inline ThrashManager* Instance{nullptr};

        std::vector<Thrash*> Thrashes;
        std::vector<Furniture*> Furnitures;

        std::vector<Entity*> Coins;
        std::vector<Entity*> Weapons;
        std::vector<Entity*> Books;
        
        std::vector<Entity*> CleanedUpCoins;
        std::vector<Entity*> CleanedUpWeapons;
        std::vector<Entity*> CleanedUpBooks;
        
    public:
        ThrashManager() = default;
        ~ThrashManager() = default;
        static ThrashManager* GetInstance();

        void AddThrash(Thrash* thrash) { Thrashes.push_back(thrash); }
        void AddFurniture(Furniture* furniture) { Furnitures.push_back(furniture); }
        void AddCoin(Entity* coin) { Coins.push_back(coin); }
        void AddWeapon(Entity* weapon) { Weapons.push_back(weapon); }
        void AddBook(Entity* book) { Books.push_back(book); }

        void AddCleanedUpCoin(Entity* coin) { CleanedUpCoins.push_back(coin); }
        void AddCleanedUpWeapon(Entity* weapon) { CleanedUpWeapons.push_back(weapon); }
        void AddCleanedUpBook(Entity* book) { CleanedUpBooks.push_back(book); }

        void RemoveThrash(Thrash* thrash);
        void RemoveFurniture(Furniture* furniture);
        void RemoveCoin(Entity* coin);
        void RemoveWeapon(Entity* weapon);
        void RemoveBook(Entity* book);

        void RemoveCleanedUpCoin(Entity* coin);
        void RemoveCleanedUpWeapon(Entity* weapon);
        void RemoveCleanedUpBook(Entity* book);

        int GetThrashCount() { return Thrashes.size(); }
        int GetFurnitureCount() { return Furnitures.size(); }

        int GetCoinCount() { return Coins.size(); }
        int GetWeaponCount() { return Weapons.size(); }
        int GetBookCount() { return Books.size(); }

        int GetCleanedUpCoinCount() { return CleanedUpCoins.size(); }
        int GetCleanedUpWeaponCount() { return CleanedUpWeapons.size(); }
        int GetCleanedUpBookCount() { return CleanedUpBooks.size(); }

        int VacuumCount;
        int VacuumVolume;
    };
} // namespace Engine
