#pragma once
#include "Engine/EngineObjects/Entity.h"
#include "Factory/PlayerRaii.h" // Used in Macro

#define SERIALIZATION_EXPORT_PLAYER(__CLASS__)\
public:\
inline static const std::string TypeName = std::string(#__CLASS__);\
\
[[nodiscard]] std::string GetType() const override\
{\
return TypeName;\
}\
\
private:\
static inline const PlayerRaii<__CLASS__> RaiiHandle = PlayerRaii<__CLASS__>(#__CLASS__);

namespace Engine
{
    class Player : public Entity
    {

    };
} // Engine
