#include "Component.h"

void Engine::Component::OnAdd(Engine::Entity* NewOwner)
{
    Owner = NewOwner;
}

 rapidjson::Value Engine::Component::Serialize(rapidjson::Document::AllocatorType& Allocator) const
{

}

 void Engine::Component::DeserializeValuePass(const rapidjson::Value& Object)
{
}

void Engine::Component::DeserializeReferencesPass(const rapidjson::Value& Object,
        std::unordered_map<GUID, SerializedObject*> ReferenceMap)
{
}

