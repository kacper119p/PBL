#pragma once
#include <string>
#include <unordered_map>

namespace Models
{
    class Model;

    class ModelManager final
    {
    private:
        static std::unordered_map<std::string, Model*> Models;

    private:
        ModelManager() = default;

    public:
        /**
         * @brief Gets cached model or loads it from a given file.
         * @param Path Model's file.
         * @return Loaded model.
         */
        static Model* GetModel(const char* Path);

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

        /**
         * @brief Removes all loaded models from memory.
         */
        static void DeleteAllModels();

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

        /**
         * @brief Finds filepath of this model.
         * @param Model Model
         * @return Model's file path.
         */
        static std::string GetModelPath(const Model* Model);
    };

} // Models
