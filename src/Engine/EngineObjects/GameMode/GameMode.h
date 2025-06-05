#pragma once
#include "Engine/EngineObjects/Entity.h"
#include "Factory/GameModeRaii.h" // Used in Macro

#define SERIALIZATION_EXPORT_GAME_MODE(__CLASS__)\
public:\
inline static const std::string TypeName = std::string(#__CLASS__);\
\
[[nodiscard]] std::string GetType() const override\
{\
return TypeName;\
}\
\
private:\
static inline const GameModeRaii<__CLASS__> RaiiHandle = GameModeRaii<__CLASS__>(#__CLASS__);

namespace Engine
{

    class GameMode : public Entity
    {
    public:
        virtual void Start() = 0;

        virtual void Update(float DeltaTime) = 0;
    };

}
