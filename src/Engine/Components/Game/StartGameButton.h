#pragma once

#include "Engine/Components/Component.h"
#include "Engine/Components/Colliders/BoxCollider.h"
#include "Events/TAction.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    class StartGameButton : public Component
    {
    private:
        BoxCollider* collider = nullptr;
        Events::TAction<Collider*> StartGameAction = Events::TAction<Collider*>(this, &StartGameButton::OnStartGame);
    public:
        StartGameButton() = default;
        ~StartGameButton() override = default;
        void OnStartGame(Collider* collider);
        void Start() override;

        SERIALIZATION_EXPORT_CLASS(StartGameButton);

#if EDITOR
        void DrawImGui() override { ImGui::Text("Has StartGameButton component"); };
#endif
    };

} // namespace Engine
