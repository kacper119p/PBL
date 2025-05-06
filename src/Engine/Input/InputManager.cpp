#include "InputManager.h"
#include <iostream>
#include "Engine/EngineObjects/UpdateManager.h"

InputManager& InputManager::GetInstance()
{
    static InputManager instance;
    return instance;
}

InputManager::InputManager() :
    m_Window(nullptr), m_Keys{}, m_MouseButtons{}, m_MousePosition(0.0f), m_LastMousePosition(0.0f), m_MouseDelta(0.0f),
    m_ScrollOffset(0.0f), m_GamepadButtons{}, m_GamepadAxes{}
{
}

void InputManager::Init(GLFWwindow* window) {
    m_Window = window;
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}

void InputManager::Update() 
{
    m_MouseDelta = m_MousePosition - m_LastMousePosition;
    m_LastMousePosition = m_MousePosition;
    m_ScrollOffset = 0.0f;

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            for (int i = 0; i < m_GamepadButtons.size(); ++i)
                m_GamepadButtons[i] = state.buttons[i] == GLFW_PRESS;

            for (int i = 0; i < m_GamepadAxes.size(); ++i)
                m_GamepadAxes[i] = state.axes[i];
        }
    }
}

bool InputManager::IsKeyPressed(int key) const { return key >= 0 && key < GLFW_KEY_LAST && m_Keys[key]; }

bool InputManager::IsMouseButtonPressed(int button) const
{
    return button >= 0 && button < GLFW_MOUSE_BUTTON_LAST && m_MouseButtons[button];
}

glm::vec2 InputManager::GetMousePosition() const { return m_MousePosition; }

glm::vec2 InputManager::GetMouseDelta() const { return m_MouseDelta; }

float InputManager::GetScrollOffset() const
{
    return m_ScrollOffset; }

bool InputManager::IsGamepadButtonPressed(int button) const
{
    return button >= 0 && button < m_GamepadButtons.size() && m_GamepadButtons[button];
}

float InputManager::GetGamepadAxis(int axis) const
{
    return axis >= 0 && axis < m_GamepadAxes.size() ? m_GamepadAxes[axis] : 0.0f;
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto& self = GetInstance();
    if (key >= 0 && key < GLFW_KEY_LAST)
    {
        self.m_Keys[key] = (action != GLFW_RELEASE);
    }
}

void InputManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto& self = GetInstance();
    if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
    {
        self.m_MouseButtons[button] = (action != GLFW_RELEASE);
    }
}

void InputManager::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto& self = GetInstance();
    self.m_MousePosition = {static_cast<float>(xpos), static_cast<float>(ypos)};
}

void InputManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto& self = GetInstance();
    self.m_ScrollOffset = static_cast<float>(yoffset);
}
