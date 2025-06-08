#include "Player.h"

#include "Engine/EngineObjects/UpdateManager.h"

namespace Engine
{
    Player::~Player()
    {
        UpdateManager::GetInstance()->UnregisterPlayer(this);
    }

    void Player::Start()
    {
        UpdateManager::GetInstance()->RegisterPlayer(this);
    }

}
