#pragma once  
#include <GLFW/glfw3.h>  
#include <glm/glm.hpp>  
#include <array>  
#include "Engine/Components/Interfaces/IUpdateable.h"  
#include "Engine/EngineObjects/UpdateManager.h"

class InputManager : public Engine::IUpdateable  
{  
public:  
   static InputManager& GetInstance();  

   void Init(GLFWwindow* window);  
   void Update(float DeltaTime) override;  

   bool IsKeyPressed(int key) const;  
   bool IsMouseButtonPressed(int button) const;  
   glm::vec2 GetMousePosition() const;  
   glm::vec2 GetMouseDelta() const;  
   float GetScrollOffset() const;  

   bool IsGamepadButtonPressed(int button) const;  
   float GetGamepadAxis(int axis) const;  

private:  
   InputManager();  
   ~InputManager() = default;  

   InputManager(const InputManager&) = delete;  
   InputManager& operator=(const InputManager&) = delete;  
   InputManager(InputManager&&) = delete;  
   InputManager& operator=(InputManager&&) = delete;  

   GLFWwindow* m_Window = nullptr;  

   std::array<bool, GLFW_KEY_LAST> m_Keys = {};  
   std::array<bool, GLFW_MOUSE_BUTTON_LAST> m_MouseButtons = {};  

   glm::vec2 m_MousePosition = {0.0f, 0.0f};  
   glm::vec2 m_LastMousePosition = {0.0f, 0.0f};  
   glm::vec2 m_MouseDelta = {0.0f, 0.0f};  
   float m_ScrollOffset = 0.0f;  

   std::array<bool, 15> m_GamepadButtons = {};  
   std::array<float, 6> m_GamepadAxes = {};  

   // Callback bridging  
   static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);  
   static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);  
   static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);  
   static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);  
};
