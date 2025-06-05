#include "PrefabLoader.h"

#include "Serialization/SerializationFilesUtility.h"

namespace Engine
{
    void PrefabLoader::LoadPrefab(const std::string& Path, Scene* const Scene, Transform* const Parent)
    {
        rapidjson::Document document;
        Serialization::ReadJsonFile(Path.c_str(), document);
        rapidjson::Value& content = document["Prefab"].GetArray();
        Entity::DeserializeEntity(content, Scene, Parent);
    }

    Entity* PrefabLoader::LoadPrefabDetached(const std::string& Path, Scene* Scene)
    {
        rapidjson::Document document;
        Serialization::ReadJsonFile(Path.c_str(), document);
        rapidjson::Value& content = document["Prefab"].GetArray();
        return Entity::DeserializeEntityDetached(content, Scene);
    }

    void PrefabLoader::SavePrefabToFile(const std::string& Path, const Entity* Entity)
    {
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Value content = rapidjson::Value(rapidjson::kArrayType);
        Entity->SerializeEntity(content, document.GetAllocator());
        document.AddMember("Prefab", content, document.GetAllocator());
        Serialization::WriteJsonFile(Path.c_str(), document);
    }
}
