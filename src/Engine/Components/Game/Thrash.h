#pragma once

#include "../Component.h"
#include "Serialization/SerializationUtility.h"
#include "imgui.h"

enum ThrashSize
{
    Small = 1,
    Medium = 5,
    Large = 10
};

class Thrash : public Engine::Component
{
    ThrashSize size;

public:
    Thrash() = default;
    ~Thrash() override = default;
    ThrashSize GetSize() const { return size; }
    void SetSize(ThrashSize newSize) { size = newSize; }
    void Start() override {}
    void OnDestroy() override {}

    SERIALIZATION_EXPORT_CLASS(Thrash);

    #if EDITOR
    void DrawImGui() override;
#endif
};
