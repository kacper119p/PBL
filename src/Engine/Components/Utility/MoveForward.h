#pragma once
#include "Engine/Components/Updateable.h"

namespace Engine
{

    class MoveForward final : public Updateable
    {
    private:
        float Speed = 0.0f;

    public:
        [[nodiscard]] float GetSpeed() const
        {
            return Speed;
        }

        void SetSpeed(const float Speed)
        {
            this->Speed = Speed;
        }

    public:
        void Update(float DeltaTime) override;

#if EDITOR
        void DrawImGui() override;
#endif
        SERIALIZATION_EXPORT_CLASS(MoveForward)

    };

} // Engine
