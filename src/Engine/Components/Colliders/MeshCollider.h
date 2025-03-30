#ifndef MESH_COLLIDER_H
#define MESH_COLLIDER_H

#include "Collider.h"
#include "../../../Models/Mesh.h"
using namespace Engine;

class MeshCollider : public Collider
{
public:
    MeshCollider(Transform* transform, bool isTrigger = false, float scale = 1.0f);
    virtual bool CheckCollision(const Collider& other) override;
    Engine::Mesh* mesh_p;
    float xScale;
    float yScale;
    float zScale;
};

#endif // MESH_COLLIDER_H
