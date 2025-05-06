#include <glm/glm.hpp>
#include <iostream>
#include "Engine/Components/Component.h"
#include "Engine/Components/Transform.h"
#include "Engine/Input/InputManager.h"
#include "../Interfaces/IUpdateable.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Entity.h"
#include "../../EngineObjects/UpdateManager.h"

namespace Engine
{
    class MovementComponent : public Component, public IUpdateable
    {
    private:
        float Speed;

    public:
        MovementComponent(float speed = 1.0f) : Speed(speed)
        {
            UpdateManager::GetInstance()->RegisterComponent(this);
        }
        void Start() override {}
        void Update(float deltaTime) override;
        void OnDestroy() override {}


        SERIALIZATION_EXPORT_CLASS(MovementComponent);


        #if EDITOR
        void DrawImGui() override
        {
            // Implementacja GUI dla edytora (opcjonalna)
            std::cout << "MovementComponent: Speed = " << Speed << std::endl;
        }
        #endif
    };
} // namespace Engine
