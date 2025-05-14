#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine
{
    class GameMode;
}

namespace Engine
{
    class GameModeFactory
    {
    private:
        class IGameModeBuilder
        {
        public:
            virtual ~IGameModeBuilder() = default;

        public:
            virtual GameMode* Build() = 0;
        };

        template<class T>
        class GameModeBuilder final : public IGameModeBuilder
        {
        public:
            GameMode* Build() override
            {
                return new T();
            }
        };

    private:
        [[nodiscard]] static GameModeFactory* GetInstance()
        {
            static GameModeFactory factory;
            return &factory;
        }

    private:
        std::unordered_map<std::string, IGameModeBuilder*> Builders;
#if EDITOR
        std::vector<std::string> AvailableGameModes;
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
            static_assert(std::is_base_of_v<GameMode, T>, "T must be derived from GameMode.");
            GetInstance()->Builders.emplace(Name, new GameModeBuilder<T>());
#if EDITOR
            if constexpr (!std::is_abstract_v<T>)
            {
                GetInstance()->AvailableGameModes.emplace_back(Name);
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
            std::erase(GetInstance()->AvailableGameModes, Name);
#endif
        }

        /**
         * @brief Creates a new GameMode which class is determined by TypeName.
         * @param TypeName TypeName of a GameMode.
         * @return Deserialized GameMode.
         */
        static GameMode* CreateObject(const std::string& TypeName);
#if EDITOR
        /**
         * @brief Returns TypeNames of all available GameModes.
         */
        static const std::vector<std::string> GetAvailableComponents()
        {
            return GetInstance()->AvailableGameModes;
        }
#endif
    };
}
