#pragma once
#include <string>
#include <unordered_map>
#include "rapidjson/document.h"

namespace Serialization
{
    class SerializedObject;
}

namespace Engine
{
    class SerializedObjectFactory
    {
    private:
        class ISerializedObjectBuilder
        {
        public:
            virtual ~ISerializedObjectBuilder() = default;

        public:
            [[nodiscard]] virtual Serialization::SerializedObject* Build() const = 0;
        };

        template<class T>
        class SerializedObjectBuilder final : public ISerializedObjectBuilder
        {
        public:
            [[nodiscard]] Serialization::SerializedObject* Build() const override
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
        template<class T>
        static void Register(const std::string& Name)
        {
            GetInstance()->Builders.emplace(Name, new SerializedObjectBuilder<T>());
        }

        static void Unregister(const std::string& Name)
        {
            GetInstance()->Builders.erase(Name);
        }

        static Serialization::SerializedObject* CreateObject(const rapidjson::Value& Object);
    };

} // Engine
