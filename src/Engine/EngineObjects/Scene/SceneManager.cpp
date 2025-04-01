#include "SceneManager.h"

#include "Serialization/SerializationFilesUtility.h"

namespace Engine
{
    void SceneManager::SaveScene(const std::string& Path, const Scene* const Scene)
    {
        rapidjson::MemoryPoolAllocator<> allocator;
        const rapidjson::Value json = Scene->Serialize(allocator);
        Serialization::WriteJsonFile(Path.c_str(), json);
    }
} // Engine
