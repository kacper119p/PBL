#pragma once
#include <string>
#include <unordered_map>

#include "rapidjson/document.h"

namespace Engine
{
    class Component;
}

namespace Serialization
{
    class SerializedObject;

    class SerializedObjectFactory
    {
    private:
        class ISerializedObjectBuilder
        {
        public:
            virtual ~ISerializedObjectBuilder() = default;

        public:
            [[nodiscard]] virtual SerializedObject* Build() const = 0;
        };

        template<class T>
        class SerializedObjectBuilder final : public ISerializedObjectBuilder
        {
        public:
            [[nodiscard]] SerializedObject* Build() const override
            {
                return new T();
            }
        };

    private:
        std::unordered_map<std::string, ISerializedObjectBuilder*> Builders;
#if EDITOR
        std::vector<std::string> AvailableComponents;
#endif

    private:
        [[nodiscard]] static SerializedObjectFactory* GetInstance()
        {
            static SerializedObjectFactory factory;
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
            GetInstance()->Builders.emplace(Name, new SerializedObjectBuilder<T>());
#if EDITOR
            if constexpr (std::is_base_of_v<Engine::Component, T> && !std::is_abstract_v<T>)
            {
                GetInstance()->AvailableComponents.emplace_back(Name);
            }
#endif
        }


        /**
         * @brief Unregisters class from this factory.
         * @param Name TypeName of a class to unregister.
         */
        static void Unregister(const std::string& Name)
        {
            GetInstance()->Builders.erase(Name);
#if EDITOR
            std::erase(GetInstance()->AvailableComponents, Name);
#endif
        }

        /**
         * @brief Creates a new object which class is determined by TypeName provided by json data.
         * @param Object Json object to read TypeName from.
         * @return Created object.
         */
        static SerializedObject* CreateObject(const std::string& TypeName);

#if EDITOR
        /**
         * @brief Returns TypeNames of all available components.
         */
        static const std::vector<std::string>* GetAvailableComponents()
        {
            return &GetInstance()->AvailableComponents;
        }
#endif
    };

} // Engine
