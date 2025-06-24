#include "Player.h"

#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/Components/Camera/CameraFollow.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    Player::~Player()
    {
        UpdateManager::GetInstance()->UnregisterPlayer(this);
        CameraFollow::GetInstance().SetTarget(nullptr);
    }

    void Player::Start()
    {
        UpdateManager::GetInstance()->RegisterPlayer(this);
        CameraFollow::GetInstance().SetTarget(this);
    }

}
