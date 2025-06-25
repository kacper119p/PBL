#pragma once
#include <string>

#include "GLFW/glfw3.h"
#include "Scene.h"

namespace Engine
{

    class SceneManager
    {
    private:
        static bool IsSceneChanging;
        static std::string NewScene;

    private:
        SceneManager() = delete;

    public:
        /**
         * @brief Saves scene to a file.
         * @param Path Path of a scene file.
         * @param Scene Scene to save.
         */
        static void SaveScene(const std::string& Path, Scene* Scene);

        /**
         * @brief Loads scene from a file.
         * @param Path Path of a scene file.
         * @param Scene Scene to load data to.
         */
        static void LoadScene(const std::string& Path, Scene* Scene);

        static void ChangeScene(const std::string& Path);

        static void UpdateScene(Scene* Scene, const CameraRenderData& RenderData, GLFWwindow* Window);

    };
} // Engine
