#pragma once
#include <GLFW/glfw3.h>

namespace Engine
{
    class WindowManager
    {
        GLFWwindow* Window = nullptr;

        static inline WindowManager* Instance{nullptr};

    public:
        WindowManager() = default;
        ~WindowManager() = default;
        static WindowManager* GetInstance()
        {
            if (!Instance)
            {
                Instance = new WindowManager();
            }
            return Instance;
        }

        void QuitGame()
        {
            if (Window)
            {
                glfwSetWindowShouldClose(Window, true);
            }
        };

        void SetWindow(GLFWwindow* window) { Window = window; }
    };
} // namespace Engine
