#include "SceneManager.h"

#include "Serialization/SerializationFilesUtility.h"
#include "Engine/EngineObjects/LightManager.h"

namespace Engine
{
    void SceneManager::SaveScene(const std::string& Path, Scene* const Scene)
    {
        rapidjson::MemoryPoolAllocator<> allocator;
        const rapidjson::Value json = Scene->Serialize(allocator);
        Serialization::WriteJsonFile(Path.c_str(), json);
    }

    void SceneManager::LoadScene(const std::string& Path, Scene* Scene)
    {
        rapidjson::Document data;
        Serialization::ReadJsonFile(Path.c_str(), data);
        Scene->Deserialize(data);
        Scene->SetPath(Path);
    }
} // Engine
