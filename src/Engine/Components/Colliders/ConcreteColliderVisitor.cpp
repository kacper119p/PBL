#pragma once

#include "ColliderVisitor.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "SphereCollider.h"
#include "SpatialPartitioning.h"
#include "spdlog/spdlog.h"

// TODO: remove when spatial fully implemented
#include "Engine/EngineObjects/Scene/Scene.h"

/////////////////////////////////////////////////////////////////////////////////////////
//
//
//          POMYSL MIALAM TAKI ZEBY ZROBIC CONCRETECOLLIDERVISITOR DLA KAZDEGO COLLIDERA
//          KAZDY COLLIDER MUSIALBY MIEC PRZYPISANA REFKE DO ZEWNETRZNEGO SPATIALA
//          CZYLI SPATIAL BEDZIE TAKIM SINGLETONOWYM MANAGEREM SIATKOWANIA POZYCJI
//          I NA TEJ PODSTAWIE BEDZIE WYKONYWANY W COLLIDER->UPDATE() MANAGECOLLISIONS()
//          A W MANAGECOLLISIONS() BEDZIE POBIERANIE POTENCJALNYCH KOLIZJI ZE SPATIALA
//          I PODWOJNYM FOREM PRZELECENIE WSZYSTKICH I WYKRYCIE KOLIZJI
//
//
///////////////////////////////////////////////////////////////////////////////////////


namespace Engine
{

    ConcreteColliderVisitor::ConcreteColliderVisitor() :
        currentCollider(nullptr), collisionDetected(false), spatialPartitioning(nullptr)
    {
    }

    ConcreteColliderVisitor::ConcreteColliderVisitor(SpatialPartitioning* partitioning, Collider* collider) :
        currentCollider(collider), collisionDetected(false), spatialPartitioning(partitioning)
    {
    }
    bool ConcreteColliderVisitor::CheckBoxBoxCollision(const BoxCollider& box1, const BoxCollider& box2)
        {
            const glm::mat4& transform1 = box1.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& transform2 = box2.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 box1Min = transform1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 box1Max = transform1 * glm::vec4(box1.GetWidth(), box1.GetHeight(), box1.GetDepth(), 1.0f);

            glm::vec3 box2Min = transform2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 box2Max = transform2 * glm::vec4(box2.GetWidth(), box2.GetHeight(), box2.GetDepth(), 1.0f);

            return !(box1Max.x < box2Min.x || box1Min.x > box2Max.x || box1Max.y < box2Min.y || box1Min.y > box2Max.y ||
                     box1Max.z < box2Min.z || box1Min.z > box2Max.z);
        }

    bool ConcreteColliderVisitor::CheckBoxSphereCollision(const BoxCollider& box, const SphereCollider& sphere)
        {
            const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 sphereCenter = sphereTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            glm::vec3 boxCenter = boxTransform *
                                  glm::vec4(box.GetWidth() / 2.0f, box.GetHeight() / 2.0f, box.GetDepth() / 2.0f, 1.0f);
            glm::vec3 boxHalfExtents = glm::vec3(box.GetWidth() / 2.0f, box.GetHeight() / 2.0f, box.GetDepth() / 2.0f);

            glm::vec3 xAxis = glm::normalize(glm::vec3(boxTransform[0])); 
            glm::vec3 yAxis = glm::normalize(glm::vec3(boxTransform[1])); 
            glm::vec3 zAxis = glm::normalize(glm::vec3(boxTransform[2])); 

            glm::vec3 delta = sphereCenter - boxCenter;

            glm::vec3 closestPoint = boxCenter;

            float distanceX = glm::dot(delta, xAxis);
            float distanceY = glm::dot(delta, yAxis);
            float distanceZ = glm::dot(delta, zAxis);

            distanceX = glm::clamp(distanceX, -boxHalfExtents.x, boxHalfExtents.x);
            distanceY = glm::clamp(distanceY, -boxHalfExtents.y, boxHalfExtents.y);
            distanceZ = glm::clamp(distanceZ, -boxHalfExtents.z, boxHalfExtents.z);

            closestPoint += distanceX * xAxis;
            closestPoint += distanceY * yAxis;
            closestPoint += distanceZ * zAxis;

            float distanceSquared = glm::dot(closestPoint - sphereCenter, closestPoint - sphereCenter);


            return distanceSquared <= (sphere.GetRadius() * sphere.GetRadius());
        }

    bool ConcreteColliderVisitor::CheckBoxCapsuleCollision(const BoxCollider& box, const CapsuleCollider& capsule)
        {
            const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 boxMin = boxTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 boxMax = boxTransform * glm::vec4(box.GetWidth(), box.GetHeight(), box.GetDepth(), 1.0f);

            glm::vec3 capsuleStart = capsuleTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 capsuleEnd = capsuleTransform * glm::vec4(0.0f, capsule.GetHeight(), 0.0f, 1.0f);
            float capsuleRadius = capsule.GetRadius();

            glm::vec3 closestPointOnAABB = glm::clamp(capsuleStart, boxMin, boxMax);

            glm::vec3 capsuleAxis = capsuleEnd - capsuleStart;
            float capsuleAxisLengthSquared = glm::dot(capsuleAxis, capsuleAxis);

            if (capsuleAxisLengthSquared > 0.0f)
            {
                float t = glm::dot(closestPointOnAABB - capsuleStart, capsuleAxis) / capsuleAxisLengthSquared;
                t = glm::clamp(t, 0.0f, 1.0f);
                glm::vec3 closestPointOnCapsule = capsuleStart + t * capsuleAxis;

                float distanceSquared = glm::dot(closestPointOnCapsule - closestPointOnAABB,
                                                 closestPointOnCapsule - closestPointOnAABB);

                return distanceSquared <= (capsuleRadius * capsuleRadius);
            }

            float distanceSquared = glm::dot(capsuleStart - closestPointOnAABB, capsuleStart - closestPointOnAABB);

            return distanceSquared <= (capsuleRadius * capsuleRadius);
        }

    bool ConcreteColliderVisitor::CheckBoxMeshCollision(const BoxCollider& box, const MeshCollider& mesh)
        {
            const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& meshTransform = mesh.GetTransform()->GetLocalToWorldMatrix();

            // Oblicz AABB dla BoxCollider
            glm::vec3 boxMin = boxTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 boxMax = boxTransform * glm::vec4(box.GetWidth(), box.GetHeight(), box.GetDepth(), 1.0f);

            // Pobierz AABB dla MeshCollider
            Models::AABBox3 meshAABB = mesh.GetMesh()->GetAabBox();
            glm::vec3 meshMin = meshTransform * glm::vec4(meshAABB.min, 1.0f);
            glm::vec3 meshMax = meshTransform * glm::vec4(meshAABB.max, 1.0f);

            // Sprawdź kolizję AABB
            if (boxMax.x < meshMin.x || boxMin.x > meshMax.x || boxMax.y < meshMin.y || boxMin.y > meshMax.y ||
                boxMax.z < meshMin.z || boxMin.z > meshMax.z)
            {
                return false; // Brak kolizji
            }

            // Sprawdź kolizję wierzchołków
            const auto& vertices = mesh.GetMesh()->VerticesData;
            for (const auto& vertex : vertices)
            {
                glm::vec3 worldVertex = meshTransform * glm::vec4(vertex.Position, 1.0f);
                if (worldVertex.x >= boxMin.x && worldVertex.x <= boxMax.x && worldVertex.y >= boxMin.y &&
                    worldVertex.y <= boxMax.y && worldVertex.z >= boxMin.z && worldVertex.z <= boxMax.z)
                {
                    return true; // Kolizja wykryta
                }
            }

            return false; // Brak kolizji
        }

    bool ConcreteColliderVisitor::CheckSphereSphereCollision(const SphereCollider& sphere1,
                                                                 const SphereCollider& sphere2)
        {
            float distance =
                    glm::distance(sphere1.GetTransform()->GetPosition(), sphere2.GetTransform()->GetPosition());
            return distance <= (sphere1.GetRadius() + sphere2.GetRadius());
        }

    bool ConcreteColliderVisitor::CheckSphereMeshCollision(const SphereCollider& sphere, const MeshCollider& mesh)
        {
            const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& meshTransform = mesh.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 sphereCenter = sphereTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            float sphereRadius = sphere.GetRadius();

            // Pobierz AABB dla MeshCollider
            Models::AABBox3 meshAABB = mesh.GetMesh()->GetAabBox();
            glm::vec3 meshMin = meshTransform * glm::vec4(meshAABB.min, 1.0f);
            glm::vec3 meshMax = meshTransform * glm::vec4(meshAABB.max, 1.0f);

            // Sprawdź kolizję AABB
            if (sphereCenter.x + sphereRadius < meshMin.x || sphereCenter.x - sphereRadius > meshMax.x ||
                sphereCenter.y + sphereRadius < meshMin.y || sphereCenter.y - sphereRadius > meshMax.y ||
                sphereCenter.z + sphereRadius < meshMin.z || sphereCenter.z - sphereRadius > meshMax.z)
            {
                return false; // Brak kolizji
            }

            // Sprawdź kolizję wierzchołków
            const auto& vertices = mesh.GetMesh()->VerticesData;
            for (const auto& vertex : vertices)
            {
                glm::vec3 worldVertex = meshTransform * glm::vec4(vertex.Position, 1.0f);

                float distanceSquared = glm::dot(sphereCenter - worldVertex, sphereCenter - worldVertex);

                if (distanceSquared <= (sphereRadius * sphereRadius))
                {
                    return true; // Kolizja wykryta
                }
            }

            return false; // Brak kolizji
        }

    bool ConcreteColliderVisitor::CheckCapsuleSphereCollision(const CapsuleCollider& capsule,
                                                                  const SphereCollider& sphere)
        {
            const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 capsuleStart = capsuleTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 capsuleEnd = capsuleTransform * glm::vec4(0.0f, capsule.GetHeight(), 0.0f, 1.0f);
            float capsuleRadius = capsule.GetRadius();

            glm::vec3 sphereCenter = sphereTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            float sphereRadius = sphere.GetRadius();

            glm::vec3 capsuleAxis = capsuleEnd - capsuleStart;
            float capsuleAxisLengthSquared = glm::dot(capsuleAxis, capsuleAxis);

            float t = glm::dot(sphereCenter - capsuleStart, capsuleAxis) / capsuleAxisLengthSquared;
            t = glm::clamp(t, 0.0f, 1.0f);
            glm::vec3 closestPointOnCapsule = capsuleStart + t * capsuleAxis;

            float distanceSquared =
                    glm::dot(closestPointOnCapsule - sphereCenter, closestPointOnCapsule - sphereCenter);

            float combinedRadius = capsuleRadius + sphereRadius;
            return distanceSquared <= (combinedRadius * combinedRadius);
        }

    bool ConcreteColliderVisitor::CheckCapsuleCapsuleCollision(const CapsuleCollider& capsule1,
                                                                   const CapsuleCollider& capsule2)
        {
            const glm::mat4& transform1 = capsule1.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& transform2 = capsule2.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 capsule1Start = transform1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 capsule1End = transform1 * glm::vec4(0.0f, capsule1.GetHeight(), 0.0f, 1.0f);
            float capsule1Radius = capsule1.GetRadius();

            glm::vec3 capsule2Start = transform2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 capsule2End = transform2 * glm::vec4(0.0f, capsule2.GetHeight(), 0.0f, 1.0f);
            float capsule2Radius = capsule2.GetRadius();

            glm::vec3 u = capsule1End - capsule1Start;
            glm::vec3 v = capsule2End - capsule2Start;
            glm::vec3 w = capsule1Start - capsule2Start;

            float a = glm::dot(u, u);
            float b = glm::dot(u, v);
            float c = glm::dot(v, v);
            float d = glm::dot(u, w);
            float e = glm::dot(v, w);

            float denominator = a * c - b * b;

            float s, t;
            if (denominator > 1e-6f)
            {
                s = (b * e - c * d) / denominator;
                t = (a * e - b * d) / denominator;
            }
            else
            {
                s = 0.0f;
                t = (e > 0.0f ? 1.0f : 0.0f);
            }

            s = glm::clamp(s, 0.0f, 1.0f);
            t = glm::clamp(t, 0.0f, 1.0f);

            glm::vec3 closestPointOnCapsule1 = capsule1Start + s * u;
            glm::vec3 closestPointOnCapsule2 = capsule2Start + t * v;

            float distanceSquared = glm::dot(closestPointOnCapsule1 - closestPointOnCapsule2,
                                             closestPointOnCapsule1 - closestPointOnCapsule2);


            float combinedRadius = capsule1Radius + capsule2Radius;
            return distanceSquared <= (combinedRadius * combinedRadius);
        }

    bool ConcreteColliderVisitor::CheckCapsuleMeshCollision(const CapsuleCollider& capsule,
                                                                const MeshCollider& mesh)
        {
            const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& meshTransform = mesh.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 capsuleStart = capsuleTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 capsuleEnd = capsuleTransform * glm::vec4(0.0f, capsule.GetHeight(), 0.0f, 1.0f);
            float capsuleRadius = capsule.GetRadius();

            const auto& vertices = mesh.GetMesh()->VerticesData;
            for (const auto& vertex : vertices)
            {
                glm::vec3 worldVertex = meshTransform * glm::vec4(vertex.Position, 1.0f);

                glm::vec3 capsuleAxis = capsuleEnd - capsuleStart;
                float capsuleAxisLengthSquared = glm::dot(capsuleAxis, capsuleAxis);

                float t = glm::dot(worldVertex - capsuleStart, capsuleAxis) / capsuleAxisLengthSquared;
                t = glm::clamp(t, 0.0f, 1.0f);
                glm::vec3 closestPointOnCapsule = capsuleStart + t * capsuleAxis;

                float distanceSquared =
                        glm::dot(closestPointOnCapsule - worldVertex, closestPointOnCapsule - worldVertex);

                if (distanceSquared <= (capsuleRadius * capsuleRadius))
                {
                    return true; // Kolizja wykryta
                }
            }

            return false; // Brak kolizji
        }

    bool ConcreteColliderVisitor::CheckMeshMeshCollision(const MeshCollider& mesh1, const MeshCollider& mesh2)
        {
            const glm::mat4& transform1 = mesh1.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& transform2 = mesh2.GetTransform()->GetLocalToWorldMatrix();

            // Pobierz AABB dla obu siatek
            Models::AABBox3 aabb1 = mesh1.GetMesh()->GetAabBox();
            Models::AABBox3 aabb2 = mesh2.GetMesh()->GetAabBox();

            glm::vec3 min1 = transform1 * glm::vec4(aabb1.min, 1.0f);
            glm::vec3 max1 = transform1 * glm::vec4(aabb1.max, 1.0f);
            glm::vec3 min2 = transform2 * glm::vec4(aabb2.min, 1.0f);
            glm::vec3 max2 = transform2 * glm::vec4(aabb2.max, 1.0f);

            // Sprawdź kolizję AABB
            if (max1.x < min2.x || min1.x > max2.x || max1.y < min2.y || min1.y > max2.y || max1.z < min2.z ||
                min1.z > max2.z)
            {
                return false; // Brak kolizji
            }

            // Sprawdź kolizję wierzchołków
            const auto& vertices1 = mesh1.GetMesh()->VerticesData;
            const auto& vertices2 = mesh2.GetMesh()->VerticesData;

            for (const auto& vertex1 : vertices1)
            {
                glm::vec3 worldVertex1 = transform1 * glm::vec4(vertex1.Position, 1.0f);

                for (const auto& vertex2 : vertices2)
                {
                    glm::vec3 worldVertex2 = transform2 * glm::vec4(vertex2.Position, 1.0f);

                    if (glm::dot(worldVertex1 - worldVertex2, worldVertex1 - worldVertex2) < 1e-6f)

                    {
                        return true; // Kolizja wykryta
                    }
                }
            }

            return false; // Brak kolizji
        }

    glm::vec3 ConcreteColliderVisitor::GetSeparationBoxBox(const BoxCollider& box1, const BoxCollider& box2)
    {
        const glm::mat4& transform1 = box1.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& transform2 = box2.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 box1Min = transform1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 box1Max = transform1 * glm::vec4(box1.GetWidth(), box1.GetHeight(), box1.GetDepth(), 1.0f);

        glm::vec3 box2Min = transform2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 box2Max = transform2 * glm::vec4(box2.GetWidth(), box2.GetHeight(), box2.GetDepth(), 1.0f);

        glm::vec3 overlapMin = glm::max(box1Min, box2Min);
        glm::vec3 overlapMax = glm::min(box1Max, box2Max);

        if (overlapMin.x > overlapMax.x || overlapMin.y > overlapMax.y || overlapMin.z > overlapMax.z)
        {
            return glm::vec3(0.0f);
        }

        glm::vec3 overlap = overlapMax - overlapMin;

        if (overlap.x < overlap.y && overlap.x < overlap.z)
        {
            return glm::vec3(overlap.x * (box1Min.x < box2Min.x ? -1.0f : 1.0f), 0.0f, 0.0f);
        }
        else if (overlap.y < overlap.z)
        {
            return glm::vec3(0.0f, overlap.y * (box1Min.y < box2Min.y ? -1.0f : 1.0f), 0.0f);
        }
        else
        {
            return glm::vec3(0.0f, 0.0f, overlap.z * (box1Min.z < box2Min.z ? -1.0f : 1.0f));
        }

        return glm::vec3(0.0f);
    }

    glm::vec3 ConcreteColliderVisitor::GetSeparationBoxSphere(const BoxCollider& box, const SphereCollider& sphere)
    {
        const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 sphereCenter = sphereTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        glm::vec3 boxCenter =
                boxTransform * glm::vec4(box.GetWidth() / 2.0f, box.GetHeight() / 2.0f, box.GetDepth() / 2.0f, 1.0f);
        glm::vec3 boxHalfExtents = glm::vec3(box.GetWidth() / 2.0f, box.GetHeight() / 2.0f, box.GetDepth() / 2.0f);

        glm::vec3 delta = sphereCenter - boxCenter;

        glm::vec3 closestPoint = glm::clamp(delta, -boxHalfExtents, boxHalfExtents) + boxCenter;

        glm::vec3 separation = sphereCenter - closestPoint;
        float distance = glm::length(separation);

        if (distance < sphere.GetRadius())
        {
            return glm::normalize(separation) * (sphere.GetRadius() - distance);
        }

        return glm::vec3(0.0f);
    }

    glm::vec3 ConcreteColliderVisitor::GetSeparationBoxCapsule(const BoxCollider& box, const CapsuleCollider& capsule)
    {
        glm::vec3 boxCenter = box.GetTransform()->GetPositionWorldSpace();
        glm::vec3 capsuleCenter = capsule.GetTransform()->GetPositionWorldSpace();

        glm::vec3 separation = capsuleCenter - boxCenter;

        float boxHalfWidth = box.GetWidth() / 2.0f;
        float boxHalfHeight = box.GetHeight() / 2.0f;
        float boxHalfDepth = box.GetDepth() / 2.0f;

        float capsuleRadius = capsule.GetRadius();
        float capsuleHeight = capsule.GetHeight();

        separation.x = glm::max(separation.x - (boxHalfWidth + capsuleRadius), 0.0f);
        separation.y = glm::max(separation.y - (boxHalfHeight + capsuleHeight / 2.0f), 0.0f);
        separation.z = glm::max(separation.z - (boxHalfDepth + capsuleRadius), 0.0f);

        return separation;
    }

    glm::vec3 ConcreteColliderVisitor::GetSeparationSphereSphere(const SphereCollider& sphere1,
                                                                 const SphereCollider& sphere2)
    {
        glm::vec3 center1 = sphere1.GetTransform()->GetPosition();
        glm::vec3 center2 = sphere2.GetTransform()->GetPosition();

        glm::vec3 delta = center2 - center1;
        float distance = glm::length(delta);
        float radiusSum = sphere1.GetRadius() + sphere2.GetRadius();

        if (distance < radiusSum)
        {
            return glm::normalize(delta) * (radiusSum - distance);
        }

        return glm::vec3(0.0f);
    }

    glm::vec3 ConcreteColliderVisitor::GetSeparationSphereCapsule(const SphereCollider& sphere,
                                                                  const CapsuleCollider& capsule)
    {
        const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 sphereCenter = sphereTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        float sphereRadius = sphere.GetRadius();

        glm::vec3 capsuleStartLocal(0.0f, -capsule.GetHeight() / 2.0f, 0.0f);
        glm::vec3 capsuleEndLocal(0.0f, capsule.GetHeight() / 2.0f, 0.0f);

        glm::vec3 capsuleStart = glm::vec3(capsuleTransform * glm::vec4(capsuleStartLocal, 1.0f));
        glm::vec3 capsuleEnd = glm::vec3(capsuleTransform * glm::vec4(capsuleEndLocal, 1.0f));
        float capsuleRadius = capsule.GetRadius();

        glm::vec3 capsuleAxis = capsuleEnd - capsuleStart;
        float capsuleAxisLengthSquared = glm::dot(capsuleAxis, capsuleAxis);

        float t = glm::dot(sphereCenter - capsuleStart, capsuleAxis) / capsuleAxisLengthSquared;
        t = glm::clamp(t, 0.0f, 1.0f);
        glm::vec3 closestPointOnCapsule = capsuleStart + t * capsuleAxis;

        glm::vec3 separation = sphereCenter - closestPointOnCapsule;
        float distance = glm::length(separation);
        float combinedRadius = sphereRadius + capsuleRadius;

        if (distance < combinedRadius)
        {
            return glm::normalize(separation) * (combinedRadius - distance);
        }

        return glm::vec3(0.0f);
    }

    glm::vec3 ConcreteColliderVisitor::GetSeparationCapsuleCapsule(const CapsuleCollider& capsule1,
                                                                   const CapsuleCollider& capsule2)
    {
        const glm::mat4& transform1 = capsule1.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& transform2 = capsule2.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 capsule1Start = transform1 * glm::vec4(0.0f, -capsule1.GetHeight() / 2.0f, 0.0f, 1.0f);
        glm::vec3 capsule1End = transform1 * glm::vec4(0.0f, capsule1.GetHeight() / 2.0f, 0.0f, 1.0f);

        glm::vec3 capsule2Start = transform2 * glm::vec4(0.0f, -capsule2.GetHeight() / 2.0f, 0.0f, 1.0f);
        glm::vec3 capsule2End = transform2 * glm::vec4(0.0f, capsule2.GetHeight() / 2.0f, 0.0f, 1.0f);

        glm::vec3 capsule1Axis = capsule1End - capsule1Start;
        glm::vec3 capsule2Axis = capsule2End - capsule2Start;

        float capsule1AxisLengthSquared = glm::dot(capsule1Axis, capsule1Axis);
        float capsule2AxisLengthSquared = glm::dot(capsule2Axis, capsule2Axis);

        glm::vec3 w = capsule1Start - capsule2Start;

        float a = capsule1AxisLengthSquared;
        float b = glm::dot(capsule1Axis, capsule2Axis);
        float c = capsule2AxisLengthSquared;
        float d = glm::dot(capsule1Axis, w);
        float e = glm::dot(capsule2Axis, w);

        float denominator = a * c - b * b;

        float s, t;
        if (denominator > 1e-6f)
        {
            s = (b * e - c * d) / denominator;
            t = (a * e - b * d) / denominator;
        }
        else 
        {
            s = 0.0f;
            t = glm::clamp(e / c, 0.0f, 1.0f);
        }

        s = glm::clamp(s, 0.0f, 1.0f);
        t = glm::clamp(t, 0.0f, 1.0f);

        glm::vec3 closestPointOnCapsule1 = capsule1Start + s * capsule1Axis;
        glm::vec3 closestPointOnCapsule2 = capsule2Start + t * capsule2Axis;

        glm::vec3 separation = closestPointOnCapsule2 - closestPointOnCapsule1;
        float distance = glm::length(separation);
        float combinedRadius = capsule1.GetRadius() + capsule2.GetRadius();

        if (distance < combinedRadius)
        {
            return glm::normalize(separation) * (combinedRadius - distance);
        }

        return glm::vec3(0.0f);
    }

    void ConcreteColliderVisitor::ResolveCollisionBox(BoxCollider& box)
        {
            if (this->currentCollider == nullptr || this->currentCollider->IsStatic())
                return;

            switch (this->currentCollider->colliderType)
            {
                case BOX:
                {
                    BoxCollider thisBox = BoxCollider();
                    thisBox = static_cast<BoxCollider&>(*this->currentCollider);
                    collisionDetected = CheckBoxBoxCollision(box, thisBox);
                    thisBox.isColliding = box.isColliding = collisionDetected;
                    if (collisionDetected)
                    {
                        glm::vec3 separation = GetSeparationBoxBox(box, thisBox);
                        box.GetTransform()->SetPosition(box.GetTransform()->GetPosition() + separation);
                        thisBox.GetTransform()->SetPosition(thisBox.GetTransform()->GetPosition() - separation);
                    }
                    // TODO: emit collision event
                    break;
                }
                case SPHERE:
                {
                    SphereCollider sphere = SphereCollider();
                    sphere = static_cast<SphereCollider&>(*this->currentCollider);
                    collisionDetected = CheckBoxSphereCollision(box, sphere);
                    box.isColliding = sphere.isColliding = collisionDetected;
                    if (collisionDetected)
                    {
                        glm::vec3 separation = GetSeparationBoxSphere(box, sphere);
                        box.GetTransform()->SetPosition(box.GetTransform()->GetPosition() + separation);
                        sphere.GetTransform()->SetPosition(sphere.GetTransform()->GetPosition() - separation);
                    }
                    // TODO: emit collision event
                    break;
                }
                case CAPSULE:
                {
                    CapsuleCollider capsule = CapsuleCollider();
                    capsule = static_cast<CapsuleCollider&>(*this->currentCollider);
                    collisionDetected = CheckBoxCapsuleCollision(box, capsule);
                    if (collisionDetected)
                    {
                        glm::vec3 separation = GetSeparationBoxCapsule(box, capsule);
                        box.GetTransform()->SetPosition(box.GetTransform()->GetPosition() + separation);
                        capsule.GetTransform()->SetPosition(capsule.GetTransform()->GetPosition() - separation);
                    }
                    // TODO: emit collision event
                    break;
                }
                case MESH:
                {
                    /*auto& mesh = static_cast<MeshCollider&>(*currentCollider);
                    collisionDetected = CheckBoxMeshCollision(box, mesh);*/
                    // TODO: emit collision event
                    break;
                }
            }
        }

    void ConcreteColliderVisitor::ResolveCollisionSphere(SphereCollider& sphere)
        {
            if (this->currentCollider == nullptr || this->currentCollider->IsStatic())
                return;

            switch (this->currentCollider->colliderType)
            {
                case BOX:
                {
                    BoxCollider box = BoxCollider();
                    box = static_cast<BoxCollider&>(*this->currentCollider);
                    collisionDetected = CheckBoxSphereCollision(box, sphere);
                    if (collisionDetected)
                    {
                        glm::vec3 separation = GetSeparationBoxSphere(box, sphere);
                        box.GetTransform()->SetPosition(box.GetTransform()->GetPosition() + separation);
                        sphere.GetTransform()->SetPosition(sphere.GetTransform()->GetPosition() - separation);
                    }
                    // TODO: emit collision event
                    break;
                }
                case SPHERE:
                {
                    SphereCollider otherSphere = SphereCollider();
                    otherSphere = SphereCollider();
                    otherSphere = static_cast<SphereCollider&>(*this->currentCollider);
                    collisionDetected = CheckSphereSphereCollision(sphere, otherSphere);
                    if (collisionDetected)
                    {
                        glm::vec3 separation = GetSeparationSphereSphere(sphere, otherSphere);
                        sphere.GetTransform()->SetPosition(sphere.GetTransform()->GetPosition() + separation);
                        otherSphere.GetTransform()->SetPosition(otherSphere.GetTransform()->GetPosition() - separation);
                    }
                    // TODO: emit collision event
                    break;
                }
                case CAPSULE:
                {
                    CapsuleCollider capsule = CapsuleCollider();
                    capsule = static_cast<CapsuleCollider&>(*this->currentCollider);
                    collisionDetected =
                            CheckCapsuleSphereCollision(capsule, sphere);
                    if (collisionDetected)
                    {
                        glm::vec3 separation = GetSeparationSphereCapsule(sphere, capsule);
                        sphere.GetTransform()->SetPosition(sphere.GetTransform()->GetPosition() + separation);
                        capsule.GetTransform()->SetPosition(capsule.GetTransform()->GetPosition() - separation);
                    }
                    // TODO: emit collision event
                    break;
                }
                case MESH:
                {
                    /*auto& mesh = static_cast<MeshCollider&>(*currentCollider);
                    collisionDetected = CheckSphereMeshCollision(sphere, mesh);*/
                    // TODO: emit collision event
                    break;
                }
            }
        }

    void ConcreteColliderVisitor::ResolveCollisionCapsule(CapsuleCollider& capsule)
        {
            if (this->currentCollider == nullptr || this->currentCollider->IsStatic())
                return;

            switch (this->currentCollider->colliderType)
            {
                case BOX:
                {
                    BoxCollider box = BoxCollider();
                    box = static_cast<BoxCollider&>(*this->currentCollider);
                    collisionDetected = CheckBoxCapsuleCollision(box, capsule);
                    if (collisionDetected)
                    {
                        glm::vec3 separation = GetSeparationBoxCapsule(box, capsule);
                        box.GetTransform()->SetPosition(box.GetTransform()->GetPosition() + separation);
                        capsule.GetTransform()->SetPosition(capsule.GetTransform()->GetPosition() - separation);
                    }
                    // TODO: emit collision event
                    break;
                }
                case SPHERE:
                {
                    SphereCollider sphere = SphereCollider();
                    sphere = static_cast<SphereCollider&>(*this->currentCollider);
                    collisionDetected =
                            CheckCapsuleSphereCollision(capsule, sphere);
                    if (collisionDetected)
                    {
                        glm::vec3 separation = GetSeparationSphereCapsule(sphere, capsule);
                        sphere.GetTransform()->SetPosition(sphere.GetTransform()->GetPosition() + separation);
                        capsule.GetTransform()->SetPosition(capsule.GetTransform()->GetPosition() - separation);
                    }
                    // TODO: emit collision event
                    break;
                }
                case CAPSULE:
                {
                    CapsuleCollider otherCapsule = CapsuleCollider();
                    otherCapsule = static_cast<CapsuleCollider&>(*this->currentCollider);

                    collisionDetected =
                            CheckCapsuleCapsuleCollision(capsule, otherCapsule);
                    if (collisionDetected)
                    {
                        glm::vec3 separation = GetSeparationCapsuleCapsule(capsule, otherCapsule);
                        capsule.GetTransform()->SetPosition(capsule.GetTransform()->GetPosition() + separation);
                        otherCapsule.GetTransform()->SetPosition(otherCapsule.GetTransform()->GetPosition() -
                                                                 separation);
                    }
                    // TODO: emit collision event
                    break;
                }
                case MESH:
                {
                    //auto& mesh = static_cast<MeshCollider&>(*currentCollider);
                    //collisionDetected = CheckCapsuleMeshCollision(capsule, mesh);
                    // TODO: emit collision event
                    break;
                }
            }
        }

    void ConcreteColliderVisitor::ResolveCollisionMesh(MeshCollider& mesh)
        {
            if (!currentCollider)
                return;

            switch (currentCollider->colliderType)
            {
                case BOX:
                {
                    auto& box = static_cast<BoxCollider&>(*currentCollider);
                    collisionDetected = CheckBoxMeshCollision(box, mesh);
                    // TODO: emit collision event
                    break;
                }
                case SPHERE:
                {
                    auto& sphere = static_cast<SphereCollider&>(*currentCollider);
                    collisionDetected = CheckSphereMeshCollision(sphere, mesh);
                    // TODO: emit collision event
                    break;
                }
                case CAPSULE:
                {
                    auto& capsule = static_cast<CapsuleCollider&>(*currentCollider);
                    collisionDetected = CheckCapsuleMeshCollision(capsule, mesh);
                    // TODO: emit collision event
                    break;
                }
                case MESH:
                {
                    auto& otherMesh = static_cast<MeshCollider&>(*currentCollider);
                    collisionDetected = CheckMeshMeshCollision(mesh, otherMesh);
                    // TODO: emit collision event
                    break;
                }
            }
        }

    void ConcreteColliderVisitor::ManageCollisions(std::vector<Collider*> colliders) 
        { 
        spdlog::info("Managing collisions");

            if (!currentCollider || currentCollider->IsStatic())
                return;

            for (auto* collider : colliders)
            {
                if (collider == this->currentCollider)
                    continue;

                collider->AcceptCollision(*this);
            }
            
        }
} // namespace Engine
