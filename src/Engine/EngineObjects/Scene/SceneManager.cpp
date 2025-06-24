#include "SceneManager.h"

#include "Serialization/SerializationFilesUtility.h"
#include "Engine/EngineObjects/LightManager.h"

namespace Engine
{
    bool SceneManager::isSceneChanging = false;
    std::string SceneManager::NewScene = "";

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
    void SceneManager::ChangeScene(const std::string& Path) 
    { 
        isSceneChanging = true;
        NewScene = Path;
    }
    void SceneManager::UpdateScene(Scene* Scene) 
    {
        if (isSceneChanging)
        {
            LoadScene(NewScene, Scene);
            isSceneChanging = false;
            NewScene = "";
        }
    }
} // Engine
