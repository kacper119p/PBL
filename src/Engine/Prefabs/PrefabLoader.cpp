#include "PrefabLoader.h"

#include "Serialization/SerializationFilesUtility.h"

namespace Engine
{
    void PrefabLoader::LoadPrefab(const std::string& Path, Scene* Scene)
    {
        rapidjson::Document document;
        Serialization::ReadJsonFile(Path.c_str(), document);
        Entity::DeserializeEntity(document, Scene);
    }

    void PrefabLoader::SavePrefabToFile(const std::string& Path, const Entity* Entity)
    {
        rapidjson::Document document;
        document.SetArray();
        Entity->SerializeEntity(document, document.GetAllocator());
        Serialization::WriteJsonFile(Path.c_str(), document);
    }
}
