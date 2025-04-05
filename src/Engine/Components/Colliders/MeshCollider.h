#ifndef MESH_COLLIDER_H
#define MESH_COLLIDER_H

#include "Collider.h"
#include "Models/Mesh.h"
using namespace Engine;

class MeshCollider : public Collider
{
public:
    float _scale;
    Mesh* _mesh_p;

    MeshCollider(Transform* transform, bool isTrigger = false, float scale = 1.0f, Mesh* mesh_p);

    virtual bool CheckCollision(const Collider& other) override;
};

#endif // MESH_COLLIDER_H
