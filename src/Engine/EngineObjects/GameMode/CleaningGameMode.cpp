#include "CleaningGameMode.h"

#include "Engine/EngineObjects/Scene/Scene.h"

namespace Engine
{
    CleaningGameMode::CleaningGameMode() :
        BloodManager()
    {

    }

    CleaningGameMode::~CleaningGameMode()
    {
        delete BloodManager;
    }

    void CleaningGameMode::Start()
    {
        BloodManager = new class BloodManager(GetScene()->GetBounds());
    }

    void CleaningGameMode::Update(float DeltaTime)
    {
    }
}
