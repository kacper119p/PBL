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
        static constexpr float TimePerCharacter = 0.025f;

        std::queue<std::string> Lines;
        std::string CurrentText;

        Image* Image = nullptr;
        Text* Text = nullptr;

        float Timer = 0.0f;
        bool FinishedAnimation = true;

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
            FinishedAnimation = false;
            Timer = 0.0f;
            if (Lines.empty())
            {
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
            return FinishedAnimation && Lines.empty();
        }

        [[nodiscard]] bool IsAnimationFinished() const
        {
            return FinishedAnimation;
        }

        void Update(float DeltaTime);

        void Render() override;
    };
}
