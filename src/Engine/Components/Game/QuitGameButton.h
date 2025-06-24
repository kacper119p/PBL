#pragma once

#include "Engine/Components/Colliders/BoxCollider.h"
#include "Engine/Components/Component.h"
#include "Events/TAction.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    class QuitGameButton : public Component
    {
    private:
        BoxCollider* collider = nullptr;
        Events::TAction<Collider*> QuitGameAction = Events::TAction<Collider*>(this, &QuitGameButton::OnQuitGame);

    public:
        QuitGameButton() = default;
        ~QuitGameButton() override = default;
        void OnQuitGame(Collider* collider);
        void Start() override;

        SERIALIZATION_EXPORT_CLASS(QuitGameButton);

#if EDITOR
        void DrawImGui() override { ImGui::Text("Has QuitGameButton component"); };
#endif
    };

} // namespace Engine
