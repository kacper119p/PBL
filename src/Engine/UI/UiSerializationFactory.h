#pragma once
#include <string>
#include <unordered_map>

namespace Engine::Ui
{
    class UiSerializationFactory
    {
    private:
        class IUiBuilder
        {
        public:
            virtual ~IUiBuilder() = default;

        public:
            [[nodiscard]] virtual class Ui* Build() const = 0;
        };

        template<class T>
        class UiBuilder final : public IUiBuilder
        {
        public:
            [[nodiscard]] Ui* Build() const override
            {
                return new T();
            }
        };

    private:
        std::unordered_map<std::string, IUiBuilder*> Builders;

    private:
        [[nodiscard]] static UiSerializationFactory* GetInstance()
        {
            static UiSerializationFactory factory;
            return &factory;
        }

    public:
        /**
         * @brief Registers a new class with this factory.
         * @tparam T Class to register.
         * @param Name TypeName of clas to register.
         */
        template<class T>
        static void Register(const std::string& Name)
        {
            static_assert(std::is_base_of_v<Ui, T>, "T must be derived from Ui.");
            GetInstance()->Builders.emplace(Name, new UiBuilder<T>());
        }


        /**
         * @brief Unregisters class from this factory.
         * @param Name TypeName of a class to unregister.
         */
        static void Unregister(const std::string& Name)
        {
            GetInstance()->Builders.erase(Name);
        }

        /**
         * @brief Creates a new UI which class is determined by TypeName.
         * @param TypeName TypeName of a UI.
         * @return Deserialized UI.
         */
        static Ui* CreateObject(const std::string& TypeName);

    public:

    };
}
