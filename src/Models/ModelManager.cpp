#include "ModelManager.h"

#include "Model.h"

namespace Models
{
    std::unordered_map<std::string, Model*> ModelManager::Models;

    Model* ModelManager::GetModel(const char* const Path)
    {
        const std::string path = Path;

        if (const auto iterator = Models.find(path); iterator != Models.end())
        {
            return iterator->second;
        }

        Model* newModel = new Model(Path);

        Models.emplace(path, newModel);
        return newModel;
    }

    bool ModelManager::DeleteModel(const char* const Path)
    {
        const std::string path = Path;
        if (const auto iterator = Models.find(path);
            iterator != Models.end())
        {
            delete iterator->second;
            Models.erase(iterator);
            return true;
        }
        return false;
    }

    bool ModelManager::DeleteModel(Model* const Model)
    {
        const std::string path = Model->GetPath();
        if (const auto iterator = Models.find(path);
            iterator != Models.end())
        {
            delete iterator->second;
            Models.erase(iterator);
            return true;
        }
        return false;
    }

    void ModelManager::DeleteAllModels()
    {
        for (const auto& pair : Models)
        {
            delete pair.second;
        }
        Models.clear();
    }

    bool ModelManager::IsValid(const char* const Path)
    {
        return Models.contains(Path);
    }

    bool ModelManager::IsValid(const Model* const Model)
    {
        return Models.contains(Model->GetPath());
    }

    std::string ModelManager::GetModelPath(const Model* const Model)
    {
        return Model->GetPath();
    }
} // Models
