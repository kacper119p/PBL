#pragma once
#include <string>
#include <unordered_map>

namespace Models
{
    class Model;
    class ModelAnimated;
    class Animation;
    class Animator;

    class ModelManager final
    {
    private:
        static std::unordered_map<std::string, Model*> Models;
        static std::unordered_map<std::string, ModelAnimated*> ModelsAnimated;
        static std::unordered_map<std::string, Animation*> Animations;
        static std::unordered_map<std::string, Animator*> Animators;

    private:
        ModelManager() = default;

    public:
        /**
         * @brief Gets cached model or loads it from a given file.
         * @param Path Model's file.
         * @return Loaded model.
         */
        static Model* GetModel(const char* Path);

        static ModelAnimated* GetAnimatedModel(const char* Path);

        /**
         * @brief Deletes model's object.
         * @param Path Filepath of a model to be deleted.
         * @return True if model was loaded, false otherwise.
         */
        static bool DeleteModel(const char* Path);

        /**
         * @brief Deletes model's object.
         * @param Model Model to be deleted.
         * @return True if model was loaded, false otherwise.
         */
        static bool DeleteModel(Model* Model);
        static bool DeleteAnimatedModel(ModelAnimated* Model);

        /**
         * @brief Removes all loaded models from memory.
         */
        static void DeleteAllModels();
        static void DeleteAllAnimatedModels();

        /**
         * @brief Checks if model is valid.
         * @param Path Model's filepath.
         * @return True if model is loaded, false otherwise.
         */
        static bool IsValid(const char* Path);

        /**
         * @brief Checks if model is valid.
         * @param Model Model's object.
         * @return True if model is loaded, false otherwise.
         */
        static bool IsValid(const Model* Model);
        static bool IsValid(const ModelAnimated* Model);

        /**
         * @brief Finds filepath of this model.
         * @param Model Model
         * @return Model's file path.
         */
        static std::string GetModelPath(const Model* Model);
        static std::string GetAnimatedModelPath(const ModelAnimated* Model);
    };

} // Models
