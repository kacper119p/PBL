#include "SceneManager.h"

#include "Serialization/SerializationFilesUtility.h"
#include "Engine/EngineObjects/RenderingManager.h"
#include "Engine/Rendering/Postprocessing/TransitionTint.h"

namespace Engine
{
    bool SceneManager::IsSceneChanging = false;
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
        IsSceneChanging = true;
        NewScene = Path;
    }

    void SceneManager::UpdateScene(Scene* Scene, const CameraRenderData& RenderData, GLFWwindow* Window)
    {
        constexpr float fadeInTime = 1.0f;
        constexpr float fadeOutTime = 1.0f;

        if (IsSceneChanging)
        {
            float startTime = static_cast<float>(glfwGetTime());
            float totalTime = 0.0f;

            TransitionTint::Enable();

            do
            {
                totalTime = static_cast<float>(glfwGetTime() - startTime);
                TransitionTint::SetTintColor(glm::vec4(0.0f, 0.0f, 0.0f, totalTime / fadeInTime));
                RenderingManager::GetInstance()->RenderAll(RenderData, 0.0f);
                glfwPollEvents();
                glfwMakeContextCurrent(Window);
                glfwSwapBuffers(Window);
            } while (totalTime < fadeInTime);

            TransitionTint::SetTintColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0));

            LoadScene(NewScene, Scene);
            IsSceneChanging = false;
            NewScene = "";

            startTime = static_cast<float>(glfwGetTime());
            totalTime = 0.0f;
            do
            {
                totalTime = static_cast<float>(glfwGetTime() - startTime);
                TransitionTint::SetTintColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f - totalTime / fadeOutTime));
                RenderingManager::GetInstance()->RenderAll(RenderData, 0.0f);
                glfwPollEvents();
                glfwMakeContextCurrent(Window);
                glfwSwapBuffers(Window);
            } while (totalTime < fadeOutTime);

            TransitionTint::SetTintColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0));
            TransitionTint::Disable();
            glfwPollEvents();
            glfwMakeContextCurrent(Window);
            glfwSwapBuffers(Window);
        }
    }
} // Engine
