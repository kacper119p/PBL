#pragma once
#include <string>
#include <unordered_map>
#include "rapidjson/document.h"

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
         * @brief Creates a new object which class is determined by TypeName provided by json data.
         * @param Object Json object to read TypeName from.
         * @return Created object.
         */
        static SerializedObject* CreateObject(const rapidjson::Value& Object);
    };

} // Engine
