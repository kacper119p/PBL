#pragma once
#include <queue>
#include <string>

#include "Image.h"
#include "Text.h"

namespace Engine::Ui
{
    class DialogueWidget final : public UiElement
    {
    private:
        enum class AnimationType : uint8_t
        {
            Enter,
            Exit,
            TextEnter
        };

        static constexpr glm::vec3 ShownPositionImage = glm::vec3(0, -468 + 25, 0.5);
        static constexpr glm::vec3 ShownPositionText = glm::vec3(-700, -308, 0.5);

        static constexpr glm::vec3 HiddenPositionImage = ShownPositionImage + glm::vec3(0, 300, 0);
        static constexpr glm::vec3 HiddenPositionText = ShownPositionText + glm::vec3(0, 300, 0);
        static constexpr float TransitionTime = 1.25f;

        static constexpr float TimePerCharacter = 0.025f;

        std::queue<std::string> Lines;
        std::string CurrentText;

        Image* Image = nullptr;
        Text* Text = nullptr;

        float Timer = 0.0f;
        bool FinishedAnimation = true;
        bool Hidden = true;
        AnimationType CurrentAnimation = AnimationType::Enter;

    public:
        DialogueWidget();

    public:
        ~DialogueWidget() override;

        void PushLine(const std::string& Line)
        {
            Lines.push(Line);
        }

        void PopLine()
        {
            if (!FinishedAnimation)
            {
                return;
            }

            CurrentAnimation = AnimationType::TextEnter;
            FinishedAnimation = false;
            Timer = 0.0f;
            if (Lines.empty())
            {
                FinishedAnimation = true;
                CurrentText = std::string();
            }
            else
            {
                CurrentText = std::move(Lines.front());
                Lines.pop();
            }
            Text->SetText(std::string());
        }

        [[nodiscard]] bool IsFinished() const
        {
            return FinishedAnimation && Lines.empty() && Hidden;
        }

        [[nodiscard]] bool IsAnimationFinished() const
        {
            return FinishedAnimation;
        }

        void Show()
        {
            if (!Hidden)
            {
                return;
            }
            if (!FinishedAnimation)
            {
                return;
            }
            Hidden = false;
            FinishedAnimation = false;
            Timer = 0.0f;
            CurrentAnimation = AnimationType::Enter;
        }

        void Hide()
        {
            if (Hidden)
            {
                return;
            }
            if (!FinishedAnimation)
            {
                return;
            }
            Hidden = true;
            FinishedAnimation = false;
            Timer = 0.0f;
            CurrentAnimation = AnimationType::Exit;
        }

        void Update(float DeltaTime);

        void Render() override;
    };
}
