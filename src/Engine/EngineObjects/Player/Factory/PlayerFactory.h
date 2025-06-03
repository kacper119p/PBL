#pragma once
#include <algorithm>
#include <string>
#include <unordered_map>

namespace Engine
{
    class Player;
}

namespace Engine
{

    class PlayerFactory
    {
    private:
        class IPlayerBuilder
        {
        public:
            virtual ~IPlayerBuilder() = default;

        public:
            virtual Player* Build() = 0;
        };

        template<class T>
        class PlayerBuilder final : public IPlayerBuilder
        {
        public:
            Player* Build() override
            {
                return new T();
            }
        };

    private:
        [[nodiscard]] static PlayerFactory* GetInstance()
        {
            static PlayerFactory factory;
            return &factory;
        }

    private:
        std::unordered_map<std::string, IPlayerBuilder*> Builders;
#if EDITOR
        std::vector<std::string> AvailablePlayers;
#endif

    public:
        /**
         * @brief Registers a new class with this factory.
         * @tparam T Class to register.
         * @param Name TypeName of clas to register.
         */
        template<class T>
        static void Register(const std::string& Name)
        {
            static_assert(std::is_base_of_v<Player, T>, "T must be derived from Player.");
            GetInstance()->Builders.emplace(Name, new PlayerBuilder<T>());
#if EDITOR
            if constexpr (!std::is_abstract_v<T>)
            {
                GetInstance()->AvailablePlayers.emplace_back(Name);
                std::ranges::sort(GetInstance()->AvailablePlayers);
            }
#endif
        }


        /**
         * @brief Unregisters class from this factory.
         * @param Name TypeName of a class to unregister.
         */
        static void Unregister(const std::string& Name)
        {
            const auto iterator = GetInstance()->Builders.find(Name);
            delete iterator->second;
            GetInstance()->Builders.erase(iterator);
#if EDITOR
            std::erase(GetInstance()->AvailablePlayers, Name);
#endif
        }

        /**
         * @brief Creates a new Player which class is determined by TypeName.
         * @param TypeName TypeName of a Player.
         * @return Deserialized Player.
         */
        static Player* CreateObject(const std::string& TypeName);
#if EDITOR
        /**
         * @brief Returns TypeNames of all available Players.
         */
        static const std::vector<std::string> GetAvailableComponents()
        {
            return GetInstance()->AvailablePlayers;
        }
#endif
    };


} // Engine
