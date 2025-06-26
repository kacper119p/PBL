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

        bool FurnitureTaskFailed = false;
        bool WeaponTaskFailed = false;
        bool BookTaskFailed = false;
        bool CoinTaskFailed = false;

        int currentLevel = 0;
        bool isCurrentLevelCompleted = false;

        float LevelStartTime = 0.0f;
        float LevelEndTime = 0.0f;

        int PlayerGrade = 6;
        
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

        void SetCurrentLevel(int level) { currentLevel = level; }

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

        int GetCurrentLevel() const { return currentLevel; }

        bool IsCurrentLevelCompleted() const { return isCurrentLevelCompleted; }

        void SetIsCurrentLevelCompleted(bool completed) { isCurrentLevelCompleted = completed; }

        void SetLevelStartTime(float time) { LevelStartTime = time; }
        float GetLevelStartTime() const { return LevelStartTime; }
        void SetLevelEndTime(float time) { LevelEndTime = time; }
        float GetLevelEndTime() const { return LevelEndTime; }
        void SetPlayerGrade(int grade) { PlayerGrade = grade; }
        int GetPlayerGrade() const { return PlayerGrade; }

        void SetFurnitureTaskFailed(bool failed) { FurnitureTaskFailed = failed; }
        bool IsFurnitureTaskFailed() const { return FurnitureTaskFailed; }
        void SetWeaponTaskFailed(bool failed) { WeaponTaskFailed = failed; }
        bool IsWeaponTaskFailed() const { return WeaponTaskFailed; }
        void SetBookTaskFailed(bool failed) { BookTaskFailed = failed; }
        bool IsBookTaskFailed() const { return BookTaskFailed; }
        void SetCoinTaskFailed(bool failed) { CoinTaskFailed = failed; }
        bool IsCoinTaskFailed() const { return CoinTaskFailed; }
        void ResetTasks()
        {
            FurnitureTaskFailed = false;
            WeaponTaskFailed = false;
            BookTaskFailed = false;
            CoinTaskFailed = false;
        }

        bool cleanedBooks() 
        { 
            return CleanedUpBooks.size() == Books.size();
        }
        bool cleanedCoins() { return CleanedUpCoins.size() == Coins.size(); }
        bool cleanedWeapons() { return CleanedUpWeapons.size() == Weapons.size(); }


        int VacuumCount;
        int VacuumVolume;
    };
} // namespace Engine
