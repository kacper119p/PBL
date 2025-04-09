#include "ModelManager.h"

#include "Model.h"
#include "ModelAnimated.h"
#include "Animation.h"
#include "Animator.h"


namespace Models
{
    std::unordered_map<std::string, Model*> ModelManager::Models;
    std::unordered_map<std::string, ModelAnimated*> ModelManager::ModelsAnimated;
    std::unordered_map<std::string, Animation*> ModelManager::Animations;
    std::unordered_map<std::string, Animator*> ModelManager::Animators;

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

    ModelAnimated* ModelManager::GetAnimatedModel(const char* Path)
    {
        const std::string path = Path;

        if (const auto iterator = ModelsAnimated.find(path); iterator != ModelsAnimated.end())
        {
            return iterator->second;
        }

        ModelAnimated* newModel = new ModelAnimated(Path);

        ModelsAnimated.emplace(path, newModel);
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

    bool ModelManager::DeleteAnimatedModel(ModelAnimated* Model)
    {
        const std::string path = Model->GetPath();
        if (const auto iterator = ModelsAnimated.find(path); iterator != ModelsAnimated.end())
        {
            delete iterator->second;
            ModelsAnimated.erase(iterator);
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

    void ModelManager::DeleteAllAnimatedModels() {
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
        return Models.contains(Model->GetPath()); }

    bool ModelManager::IsValid(const ModelAnimated* Model) { return ModelsAnimated.contains(Model->GetPath()); }

    std::string ModelManager::GetModelPath(const Model* const Model)
    {
        return Model->GetPath(); }
    std::string ModelManager::GetAnimatedModelPath(const ModelAnimated* Model) { return Model->GetPath(); }
} // Models
