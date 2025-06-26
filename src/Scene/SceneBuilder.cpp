#include "SceneBuilder.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>
#include "Utility/TextureUtilities.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Components/Renderers/AnimatedModelRenderer.h"
#include "Engine/Gui/LightsGui.h"
#include "Engine/Components/Colliders/Collider.h"
#include "Engine/Components/BaseMovement/MovementComponent.h"
#include "Engine/EngineObjects/Scene/SceneManager.h"
#include "tracy/Tracy.hpp"


namespace Scene
{
    bool shouldContinue = true;

    void SceneBuilder::Build(Engine::Scene*& Scene)
    {
        ZoneScoped;
        Scene = new class Engine::Scene();
        Engine::SceneManager::LoadScene("./res/scenes/Gameplay1.lvl", Scene);
        // TODO: remove when no longer needed

#if !EDITOR

#endif
    }
} // namespace Scene
