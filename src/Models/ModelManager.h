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
        static Model* GetModel(const char* Path);

        static bool DeleteModel(const char* Path);


        static bool DeleteModel(Model* Model);

        static void DeleteAllModels();


        static bool IsValid(const char* Path);


        static bool IsValid(const Model* Model);

        static std::string GetModelPath(const Model* Model);
    };

} // Models
