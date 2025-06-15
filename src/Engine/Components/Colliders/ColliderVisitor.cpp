#pragma once

#include "ColliderVisitor.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Engine/Components/Physics/Rigidbody.h"
#include "Engine/EngineObjects/Entity.h"
#include "SpatialPartitioning.h"
#include "SphereCollider.h"
#include "spdlog/spdlog.h"

namespace Engine
{

    ColliderVisitor::ColliderVisitor() :
        currentCollider(nullptr), collisionDetected(false), spatialPartitioning(&SpatialPartitioning::GetInstance())
    {
        result.collisionNormal = glm::vec3(0.0f);
        result.collisionPoint = glm::vec3(0.0f);
        result.penetrationDepth = 0.0f;
        result.hasCollision = false;
    }

    bool OverlapOnAxis(const glm::vec3& axis, const glm::vec3& toCenter, const glm::vec3& aX, const glm::vec3& aY,
                       const glm::vec3& aZ, const glm::vec3& aHalf, const glm::vec3& bX, const glm::vec3& bY,
                       const glm::vec3& bZ, const glm::vec3& bHalf)
    {
        if (glm::length2(axis) < 1e-6f)
            return true;
        float projectionA = std::abs(glm::dot(axis, aX)) * aHalf.x + std::abs(glm::dot(axis, aY)) * aHalf.y +
                            std::abs(glm::dot(axis, aZ)) * aHalf.z;

        float projectionB = std::abs(glm::dot(axis, bX)) * bHalf.x + std::abs(glm::dot(axis, bY)) * bHalf.y +
                            std::abs(glm::dot(axis, bZ)) * bHalf.z;

        float distance = std::abs(glm::dot(toCenter, axis));

        return distance <= (projectionA + projectionB);
    }

    CollisionResult ColliderVisitor::CheckBoxBoxCollision(const BoxCollider& box1, const BoxCollider& box2)
    {
        CollisionResult result;

        result.hasCollision = false;
        result.ownerCollider = &box1;
        result.otherCollider = &box2;

        const glm::mat4& t1 = box1.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& t2 = box2.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 half1 = glm::vec3(box1.GetWidth(), box1.GetHeight(), box1.GetDepth()) * 0.5f;
        glm::vec3 half2 = glm::vec3(box2.GetWidth(), box2.GetHeight(), box2.GetDepth()) * 0.5f;

        glm::vec3 center1 = glm::vec3(t1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec3 center2 = glm::vec3(t2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glm::vec3 aX = glm::normalize(glm::vec3(t1[0]));
        glm::vec3 aY = glm::normalize(glm::vec3(t1[1]));
        glm::vec3 aZ = glm::normalize(glm::vec3(t1[2]));

        glm::vec3 bX = glm::normalize(glm::vec3(t2[0]));
        glm::vec3 bY = glm::normalize(glm::vec3(t2[1]));
        glm::vec3 bZ = glm::normalize(glm::vec3(t2[2]));

        glm::vec3 toCenter = center2 - center1;

        glm::vec3 axes[] = {aX,
                            aY,
                            aZ,
                            bX,
                            bY,
                            bZ,
                            glm::cross(aX, bX),
                            glm::cross(aX, bY),
                            glm::cross(aX, bZ),
                            glm::cross(aY, bX),
                            glm::cross(aY, bY),
                            glm::cross(aY, bZ),
                            glm::cross(aZ, bX),
                            glm::cross(aZ, bY),
                            glm::cross(aZ, bZ)};

        float minPenetration = std::numeric_limits<float>::max();
        glm::vec3 collisionNormal(0.0f);

        for (const glm::vec3& axis : axes)
        {
            if (glm::length2(axis) < 1e-6f)
                continue;

            glm::vec3 normAxis = glm::normalize(axis);
            if (!OverlapOnAxis(normAxis, toCenter, aX, aY, aZ, half1, bX, bY, bZ, half2))
            {
                result.hasCollision = false;
                return result;
            }

            float projectedRadius1 = glm::abs(glm::dot(normAxis, aX)) * half1.x +
                                     glm::abs(glm::dot(normAxis, aY)) * half1.y +
                                     glm::abs(glm::dot(normAxis, aZ)) * half1.z;

            float projectedRadius2 = glm::abs(glm::dot(normAxis, bX)) * half2.x +
                                     glm::abs(glm::dot(normAxis, bY)) * half2.y +
                                     glm::abs(glm::dot(normAxis, bZ)) * half2.z;

            float distance = glm::abs(glm::dot(toCenter, normAxis));
            float penetration = projectedRadius1 + projectedRadius2 - distance;

            if (abs(penetration) < abs(minPenetration))
            {
                minPenetration = penetration;
                collisionNormal = normAxis;
                if (glm::dot(toCenter, collisionNormal) < 0.0f)
                    collisionNormal = -collisionNormal;
            }
        }

        result.hasCollision = true;
        result.penetrationDepth = minPenetration;
        result.collisionNormal = collisionNormal;

        result.collisionPoint =
                center1 + collisionNormal * (glm::dot(toCenter, collisionNormal) - minPenetration * 0.5f);

        return result;
    }

    CollisionResult ColliderVisitor::CheckBoxSphereCollision(const BoxCollider& box, const SphereCollider& sphere)
    {
        CollisionResult result;

        result.hasCollision = false;
        result.ownerCollider = &box;
        result.otherCollider = &sphere;

        const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 sphereCenter = glm::vec3(sphereTransform * glm::vec4(0, 0, 0, 1));

        glm::vec3 xAxis = glm::normalize(glm::vec3(boxTransform[0]));
        glm::vec3 yAxis = glm::normalize(glm::vec3(boxTransform[1]));
        glm::vec3 zAxis = glm::normalize(glm::vec3(boxTransform[2]));

        float scaleX = glm::length(glm::vec3(boxTransform[0]));
        float scaleY = glm::length(glm::vec3(boxTransform[1]));
        float scaleZ = glm::length(glm::vec3(boxTransform[2]));

        glm::vec3 halfExtents = glm::vec3(box.GetWidth() * 0.5f * scaleX, box.GetHeight() * 0.5f * scaleY,
                                          box.GetDepth() * 0.5f * scaleZ);

        glm::vec3 boxCenter = glm::vec3(boxTransform * glm::vec4(0, 0, 0, 1));

        glm::vec3 delta = sphereCenter - boxCenter;

        float distanceX = glm::dot(delta, xAxis);
        float distanceY = glm::dot(delta, yAxis);
        float distanceZ = glm::dot(delta, zAxis);

        distanceX = glm::clamp(distanceX, -halfExtents.x, halfExtents.x);
        distanceY = glm::clamp(distanceY, -halfExtents.y, halfExtents.y);
        distanceZ = glm::clamp(distanceZ, -halfExtents.z, halfExtents.z);

        glm::vec3 closestPoint = boxCenter + distanceX * xAxis + distanceY * yAxis + distanceZ * zAxis;

        float distanceSquared = glm::length2(closestPoint - sphereCenter);

        float scaleSphere = glm::length(glm::vec3(sphereTransform[0]));
        float worldRadius = sphere.GetRadius() * scaleSphere;

        if (distanceSquared <= (worldRadius * worldRadius))
        {
            result.hasCollision = true;

            glm::vec3 collisionNormal = sphereCenter - closestPoint;
            float dist = glm::length(collisionNormal);

            if (dist > 1e-6f)
                collisionNormal /= dist;
            else
                collisionNormal = glm::vec3(0, 1, 0);

            result.collisionNormal = collisionNormal;
            result.collisionPoint = closestPoint;
            result.penetrationDepth = worldRadius - dist;
        }

        return result;
    }

    CollisionResult ColliderVisitor::CheckBoxCapsuleCollision(const BoxCollider& box, const CapsuleCollider& capsule)
    {
        CollisionResult result;

        result.hasCollision = false;
        result.ownerCollider = &box;
        result.otherCollider = &capsule;

        const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 boxCenter = glm::vec3(boxTransform[3]);
        glm::vec3 xAxis = glm::normalize(glm::vec3(boxTransform[0]));
        glm::vec3 yAxis = glm::normalize(glm::vec3(boxTransform[1]));
        glm::vec3 zAxis = glm::normalize(glm::vec3(boxTransform[2]));

        float scaleX = glm::length(glm::vec3(boxTransform[0]));
        float scaleY = glm::length(glm::vec3(boxTransform[1]));
        float scaleZ = glm::length(glm::vec3(boxTransform[2]));

        glm::vec3 halfExtents = glm::vec3(box.GetWidth() * 0.5f * scaleX, box.GetHeight() * 0.5f * scaleY,
                                          box.GetDepth() * 0.5f * scaleZ);

        const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();
        glm::vec3 capsuleCenter = glm::vec3(capsuleTransform[3]);
        glm::vec3 capsuleUp = glm::normalize(glm::vec3(capsuleTransform * glm::vec4(0, 1, 0, 0)));

        float capsuleHalfHeight = capsule.GetHeight() * 0.5f;
        glm::vec3 capsuleStart = capsuleCenter - capsuleUp * capsuleHalfHeight;
        glm::vec3 capsuleEnd = capsuleCenter + capsuleUp * capsuleHalfHeight;
        float capsuleRadius = capsule.GetRadius();

        auto ClosestPointSegmentToOBB = [&](const glm::vec3& segStart,
                                            const glm::vec3& segEnd) -> std::pair<glm::vec3, glm::vec3>
        {
            float bestDistSq = std::numeric_limits<float>::max();
            glm::vec3 bestPointSeg, bestPointBox;

            const int samples = 16;
            for (int i = 0; i <= samples; ++i)
            {
                float t = static_cast<float>(i) / samples;
                glm::vec3 p = glm::mix(segStart, segEnd, t);
                glm::vec3 d = p - boxCenter;

                float dx = glm::clamp(glm::dot(d, xAxis), -halfExtents.x, halfExtents.x);
                float dy = glm::clamp(glm::dot(d, yAxis), -halfExtents.y, halfExtents.y);
                float dz = glm::clamp(glm::dot(d, zAxis), -halfExtents.z, halfExtents.z);

                glm::vec3 q = boxCenter + dx * xAxis + dy * yAxis + dz * zAxis;
                float distSq = glm::length2(p - q);

                if (distSq < bestDistSq)
                {
                    bestDistSq = distSq;
                    bestPointSeg = p;
                    bestPointBox = q;
                }
            }

            return {bestPointSeg, bestPointBox};
        };

        auto [capsuleP, boxP] = ClosestPointSegmentToOBB(capsuleStart, capsuleEnd);
        glm::vec3 delta = capsuleP - boxP;
        float distSq = glm::length2(delta);
        float radiusSq = capsuleRadius * capsuleRadius;

        if (distSq <= radiusSq)
        {
            result.hasCollision = true;

            float dist = sqrt(distSq);
            glm::vec3 collisionNormal;
            if (dist > 1e-6f)
                collisionNormal = delta / dist;
            else
                collisionNormal = glm::vec3(0, 1, 0);

            result.collisionNormal = collisionNormal;
            result.collisionPoint = boxP;
            result.penetrationDepth = capsuleRadius - dist;
        }

        return result;
    }

    CollisionResult ColliderVisitor::CheckSphereSphereCollision(const SphereCollider& sphere1,
                                                                const SphereCollider& sphere2)
    {
        CollisionResult result;

        result.hasCollision = false;
        result.ownerCollider = &sphere1;
        result.otherCollider = &sphere2;

        glm::vec3 center1 = sphere1.GetTransform()->GetPositionWorldSpace();
        glm::vec3 center2 = sphere2.GetTransform()->GetPositionWorldSpace();

        float radius1 = sphere1.GetRadius();
        float radius2 = sphere2.GetRadius();

        glm::vec3 delta = center2 - center1;
        float distance = glm::length(delta);
        float radiusSum = radius1 + radius2;

        if (distance <= radiusSum)
        {
            result.hasCollision = true;
            if (distance > 1e-6f)
            {
                result.collisionNormal = glm::normalize(delta);
                result.collisionPoint = center1 + result.collisionNormal * radius1;
            }
            else
            {
                result.collisionNormal = glm::vec3(1, 0, 0);
                result.collisionPoint = center1;
            }
            result.penetrationDepth = radiusSum - distance;
        }


        return result;
    }

    CollisionResult ColliderVisitor::CheckCapsuleSphereCollision(const CapsuleCollider& capsule,
                                                                 const SphereCollider& sphere)
    {
        CollisionResult result;

        result.hasCollision = false;
        result.ownerCollider = &capsule;
        result.otherCollider = &sphere;

        const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 capsuleOrigin = glm::vec3(capsuleTransform * glm::vec4(0, 0, 0, 1));
        glm::vec3 upDirection = glm::normalize(glm::vec3(capsuleTransform * glm::vec4(0, 1, 0, 0)));

        float cylinderHeight = capsule.GetHeight() - 2.0f * capsule.GetRadius();
        glm::vec3 capsuleStart = capsuleOrigin - 0.5f * cylinderHeight * upDirection;
        glm::vec3 capsuleEnd = capsuleOrigin + 0.5f * cylinderHeight * upDirection;

        glm::vec3 sphereCenter = glm::vec3(sphereTransform * glm::vec4(0, 0, 0, 1));

        float t = glm::dot(sphereCenter - capsuleStart, capsuleEnd - capsuleStart) /
                  glm::length2(capsuleEnd - capsuleStart);
        t = glm::clamp(t, 0.0f, 1.0f);
        glm::vec3 closestPoint = capsuleStart + t * (capsuleEnd - capsuleStart);

        float distanceSquared = glm::length2(sphereCenter - closestPoint);
        float radiusSum = capsule.GetRadius() + sphere.GetRadius();

        if (distanceSquared <= (radiusSum * radiusSum))
        {
            result.hasCollision = true;

            glm::vec3 dir = sphereCenter - closestPoint;
            if (glm::length2(dir) < 1e-6f)
            {
                dir = glm::vec3(1, 0, 0);
            }

            glm::vec3 collisionPointOnCapsuleSurface = closestPoint + glm::normalize(dir) * capsule.GetRadius();
            result.collisionPoint = collisionPointOnCapsuleSurface;
            result.collisionNormal = glm::normalize(sphereCenter - collisionPointOnCapsuleSurface);
            result.penetrationDepth = radiusSum - glm::sqrt(distanceSquared);
        }

        return result;
    }

    CollisionResult ColliderVisitor::CheckCapsuleCapsuleCollision(const CapsuleCollider& capsule1,
                                                                  const CapsuleCollider& capsule2)
    {
        CollisionResult result;

        result.hasCollision = false;
        result.ownerCollider = &capsule1;
        result.otherCollider = &capsule2;

        const glm::mat4& transform1 = capsule1.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& transform2 = capsule2.GetTransform()->GetLocalToWorldMatrix();

        float halfHeight1 = capsule1.GetHeight() * 0.5f;
        float halfHeight2 = capsule2.GetHeight() * 0.5f;

        float radius1 = capsule1.GetRadius();
        float radius2 = capsule2.GetRadius();

        glm::vec3 localA1(0.0f, -halfHeight1, 0.0f);
        glm::vec3 localB1(0.0f, +halfHeight1, 0.0f);

        glm::vec3 localA2(0.0f, -halfHeight2, 0.0f);
        glm::vec3 localB2(0.0f, +halfHeight2, 0.0f);

        glm::vec3 A1 = glm::vec3(transform1 * glm::vec4(localA1, 1.0f));
        glm::vec3 B1 = glm::vec3(transform1 * glm::vec4(localB1, 1.0f));

        glm::vec3 A2 = glm::vec3(transform2 * glm::vec4(localA2, 1.0f));
        glm::vec3 B2 = glm::vec3(transform2 * glm::vec4(localB2, 1.0f));

        glm::vec3 d1 = B1 - A1;
        glm::vec3 d2 = B2 - A2;
        glm::vec3 r = A1 - A2;

        float a = glm::dot(d1, d1);
        float e = glm::dot(d2, d2);
        float f = glm::dot(d2, r);

        float s, t;

        if (a <= 1e-6f && e <= 1e-6f)
        {
            s = t = 0.0f;
        }
        else if (a <= 1e-6f)
        {
            s = 0.0f;
            t = glm::clamp(f / e, 0.0f, 1.0f);
        }
        else
        {
            float c = glm::dot(d1, r);
            if (e <= 1e-6f)
            {
                t = 0.0f;
                s = glm::clamp(-c / a, 0.0f, 1.0f);
            }
            else
            {
                float b = glm::dot(d1, d2);
                float denom = a * e - b * b;

                if (denom != 0.0f)
                {
                    s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
                }
                else
                {
                    s = 0.0f;
                }

                t = (b * s + f) / e;

                if (t < 0.0f)
                {
                    t = 0.0f;
                    s = glm::clamp(-c / a, 0.0f, 1.0f);
                }
                else if (t > 1.0f)
                {
                    t = 1.0f;
                    s = glm::clamp((b - c) / a, 0.0f, 1.0f);
                }
            }
        }

        glm::vec3 closestPoint1 = A1 + d1 * s;
        glm::vec3 closestPoint2 = A2 + d2 * t;

        float distanceSquared = glm::length2(closestPoint1 - closestPoint2);
        float combinedRadius = radius1 + radius2;

        if (distanceSquared <= (combinedRadius * combinedRadius))
        {
            result.hasCollision = true;
            result.collisionPoint = (closestPoint1 + closestPoint2) * 0.5f;

            glm::vec3 normal = closestPoint1 - closestPoint2;
            float lengthSquared = glm::length2(normal);

            if (lengthSquared > 1e-6f)
                result.collisionNormal = glm::normalize(normal);
            else
                result.collisionNormal = glm::vec3(1.0f, 0.0f, 0.0f);

            result.penetrationDepth = combinedRadius - glm::sqrt(distanceSquared);
        }
        else
        {
            result.hasCollision = false;
        }

        return result;
    }

    inline glm::vec3 ColliderVisitor::GetSeparation()
    {
        if (!result.hasCollision)
            return glm::vec3(0.0f);

        if (!currentCollider || !currentCollider->GetTransform() || !result.otherCollider)
            return glm::vec3(0.0f);

        glm::vec3 gravityDir = glm::normalize(glm::vec3(0,-9.81f,0));

        float alignment = glm::dot(result.collisionNormal, gravityDir);

        const float verticalThreshold = 0.7f;

        glm::vec3 separation = result.collisionNormal * result.penetrationDepth;

        if (alignment > verticalThreshold)
        {
            return separation;
        }
        else if (alignment < -verticalThreshold)
        {
            return separation;
        }
        else
        {
            return 0.5f * separation;
        }
    }

    void ColliderVisitor::EmitCollision(Collider* const Collider) const { Collider->EmitCollision(currentCollider); }

    void ColliderVisitor::EmitTrigger(Collider* Collider) const { Collider->EmitTrigger(currentCollider); }

    void ColliderVisitor::ResolveCollisionBox(BoxCollider& box)
    {
        if (!currentCollider || this->currentCollider->IsStatic() ||
            (currentCollider->GetOwner()->GetComponent<Rigidbody>() &&
             currentCollider->GetOwner()->GetComponent<Rigidbody>()->mass == 0.0f))
            return;

        switch (this->currentCollider->colliderType)
        {
            case BOX:
            {
                auto* boxCollider = reinterpret_cast<BoxCollider*>(this->currentCollider);
                if (box.GetOwner() == boxCollider->GetOwner())
                    return;

                result = CheckBoxBoxCollision(box, *boxCollider);
                box.isColliding = boxCollider->isColliding = result.hasCollision;

                if (!result.hasCollision)
                    return;

                if (box.IsTrigger())
                {
                    EmitTrigger(&box);
                    return;
                }

                float penetrationDepth = result.penetrationDepth;
                glm::vec3 normal = result.collisionNormal;
                glm::vec3 contactPoint = result.collisionPoint;

                Engine::Rigidbody* thisRB = currentCollider->GetOwner()->GetComponent<Engine::Rigidbody>();
                Engine::Rigidbody* otherRB = box.GetOwner()->GetComponent<Engine::Rigidbody>();

                if (thisRB && otherRB)
                {
                    thisRB->OnCollision(otherRB, contactPoint, normal, penetrationDepth);
                }
                else if (thisRB)
                {
                    thisRB->OnCollisionStatic(contactPoint, normal);
                }

                glm::vec3 separation = GetSeparation();

               if (thisRB && otherRB)
                {
                    float invMassSum = thisRB->inverseMass + otherRB->inverseMass;
                    if (invMassSum > 0.0f)
                    {
                        glm::vec3 correctionThis = separation * (thisRB->inverseMass / invMassSum);

                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     correctionThis);
                    }
                }
                else if (thisRB)
                {
                    currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                 separation);
                }

                EmitCollision(&box);

                break;
            }

            case SPHERE:
            {
                auto* sphere = reinterpret_cast<SphereCollider*>(this->currentCollider);
                if (sphere->GetOwner() == box.GetOwner() || box.IsTrigger())
                    return;

                result = CheckBoxSphereCollision(box, *sphere);
                box.isColliding = sphere->isColliding = result.hasCollision;

                if (!result.hasCollision)
                    return;

                if (box.IsTrigger())
                {
                    EmitTrigger(&box);
                    return;
                }

                float penetrationDepth =
                        result.penetrationDepth;
                glm::vec3 normal = result.collisionNormal;
                glm::vec3 contactPoint = result.collisionPoint;

                Engine::Rigidbody* thisRB = currentCollider->GetOwner()->GetComponent<Engine::Rigidbody>();
                Engine::Rigidbody* otherRB = box.GetOwner()->GetComponent<Engine::Rigidbody>();

                if (thisRB && otherRB)
                {
                    thisRB->OnCollision(otherRB, contactPoint, normal, penetrationDepth);
                }
                else if (thisRB)
                {
                    thisRB->OnCollisionStatic(contactPoint, normal);
                }

                glm::vec3 separation = GetSeparation();

                if (thisRB && otherRB)
                {
                    float invMassSum = thisRB->inverseMass + otherRB->inverseMass;
                    if (invMassSum > 0.0f)
                    {
                        glm::vec3 correctionThis = separation * (thisRB->inverseMass / invMassSum);

                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     correctionThis);
                    }
                }
                else if (thisRB)
                {
                    currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                 separation);
                }

                EmitCollision(&box);

                break;
            }

            case CAPSULE:
            {
                auto* capsule = reinterpret_cast<CapsuleCollider*>(this->currentCollider);
                if (capsule->GetOwner() == box.GetOwner())
                    return;

                result = CheckBoxCapsuleCollision(box, *capsule);
                box.isColliding = capsule->isColliding = result.hasCollision;

                if (!result.hasCollision)
                    return;

                if (box.IsTrigger())
                {
                    EmitTrigger(&box);
                    return;
                }

                float penetrationDepth = result.penetrationDepth;
                glm::vec3 normal = result.collisionNormal;
                glm::vec3 contactPoint = result.collisionPoint;

                Engine::Rigidbody* thisRB = currentCollider->GetOwner()->GetComponent<Engine::Rigidbody>();
                Engine::Rigidbody* otherRB = box.GetOwner()->GetComponent<Engine::Rigidbody>();

                if (thisRB && otherRB)
                {
                    thisRB->OnCollision(otherRB, contactPoint, normal, penetrationDepth);
                }
                else if (thisRB)
                {
                    thisRB->OnCollisionStatic(contactPoint, normal);
                }

                glm::vec3 separation = GetSeparation();

                if (thisRB && otherRB)
                {
                    float invMassSum = thisRB->inverseMass + otherRB->inverseMass;
                    if (invMassSum > 0.0f)
                    {
                        glm::vec3 correctionThis = separation * (thisRB->inverseMass / invMassSum);

                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     correctionThis);
                    }
                }
                else if (thisRB)
                {
                    currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                 separation);
                }

                EmitCollision(&box);

                break;
            }
        }
    }

    void ColliderVisitor::ResolveCollisionSphere(SphereCollider& sphere)
    {
        if (!currentCollider || currentCollider->IsStatic() ||
            (currentCollider->GetOwner()->GetComponent<Rigidbody>() &&
             currentCollider->GetOwner()->GetComponent<Rigidbody>()->mass == 0.0f))
            return;

        switch (currentCollider->colliderType)
        {
            case BOX:
            {
                auto* box = reinterpret_cast<BoxCollider*>(currentCollider);
                if (box->GetOwner() == sphere.GetOwner())
                    return;

                result = CheckBoxSphereCollision(*box, sphere);
                box->isColliding = sphere.isColliding = result.hasCollision;

                if (!result.hasCollision)
                    return;

                if (sphere.IsTrigger())
                {
                    EmitTrigger(&sphere);
                    return;
                }

                float penetrationDepth = result.penetrationDepth;
                glm::vec3 normal = result.collisionNormal;
                glm::vec3 contactPoint = result.collisionPoint;

                Rigidbody* thisRB = currentCollider->GetOwner()->GetComponent<Rigidbody>();
                Rigidbody* otherRB = sphere.GetOwner()->GetComponent<Rigidbody>();

                if (thisRB && otherRB)
                {
                    thisRB->OnCollision(otherRB, contactPoint, normal, penetrationDepth);
                }
                else if (thisRB)
                {
                    thisRB->OnCollisionStatic(contactPoint, normal);
                }

                glm::vec3 separation = GetSeparation();

                if (thisRB && otherRB)
                {
                    float invMassSum = thisRB->inverseMass + otherRB->inverseMass;
                    if (invMassSum > 0.0f)
                    {
                        glm::vec3 correctionThis = separation * (thisRB->inverseMass / invMassSum);
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                     correctionThis);
                    }
                }
                else if (thisRB)
                {
                    currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                 separation);
                }

                EmitCollision(&sphere);
                break;
            }

            case SPHERE:
            {
                auto* otherSphere = reinterpret_cast<SphereCollider*>(currentCollider);
                if (otherSphere->GetOwner() == sphere.GetOwner())
                    return;

                result = CheckSphereSphereCollision(sphere, *otherSphere);
                otherSphere->isColliding = sphere.isColliding = result.hasCollision;

                if (!result.hasCollision)
                    return;

                if (sphere.IsTrigger())
                {
                    EmitTrigger(&sphere);
                    return;
                }

                float penetrationDepth = result.penetrationDepth;
                glm::vec3 normal = result.collisionNormal;
                glm::vec3 contactPoint = result.collisionPoint;

                Rigidbody* thisRB = currentCollider->GetOwner()->GetComponent<Rigidbody>();
                Rigidbody* otherRB = sphere.GetOwner()->GetComponent<Rigidbody>();

                if (thisRB && otherRB)
                {
                    thisRB->OnCollision(otherRB, contactPoint, normal, penetrationDepth);
                }
                else if (thisRB)
                {
                    thisRB->OnCollisionStatic(contactPoint, normal);
                }

                glm::vec3 separation = GetSeparation();

                if (thisRB && otherRB)
                {
                    float invMassSum = thisRB->inverseMass + otherRB->inverseMass;
                    if (invMassSum > 0.0f)
                    {
                        glm::vec3 correctionThis = separation * (thisRB->inverseMass / invMassSum);
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                     correctionThis);
                    }
                }
                else if (thisRB)
                {
                    currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                 separation);
                }

                EmitCollision(&sphere);
                break;
            }

            case CAPSULE:
            {
                auto* capsule = reinterpret_cast<CapsuleCollider*>(currentCollider);
                if (capsule->GetOwner() == sphere.GetOwner())
                    return;

                result = CheckCapsuleSphereCollision(*capsule, sphere);
                capsule->isColliding = sphere.isColliding = result.hasCollision;

                if (!result.hasCollision)
                    return;

                if (sphere.IsTrigger())
                {
                    EmitTrigger(&sphere);
                    return;
                }

                float penetrationDepth = result.penetrationDepth;
                glm::vec3 normal = result.collisionNormal;
                glm::vec3 contactPoint = result.collisionPoint;

                Rigidbody* thisRB = currentCollider->GetOwner()->GetComponent<Rigidbody>();
                Rigidbody* otherRB = sphere.GetOwner()->GetComponent<Rigidbody>();

                if (thisRB && otherRB)
                {
                    thisRB->OnCollision(otherRB, contactPoint, normal, penetrationDepth);
                }
                else if (thisRB)
                {
                    thisRB->OnCollisionStatic(contactPoint, normal);
                }

                glm::vec3 separation = GetSeparation();

                if (thisRB && otherRB)
                {
                    float invMassSum = thisRB->inverseMass + otherRB->inverseMass;
                    if (invMassSum > 0.0f)
                    {
                        glm::vec3 correctionThis = separation * (thisRB->inverseMass / invMassSum);
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                     correctionThis);
                    }
                }
                else if (thisRB)
                {
                    currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                 separation);
                }

                EmitCollision(&sphere);
                break;
            }
        }
    }

    void ColliderVisitor::ResolveCollisionCapsule(CapsuleCollider& capsule)
    {
        if (!currentCollider || currentCollider->IsStatic() ||
            (currentCollider->GetOwner()->GetComponent<Rigidbody>() &&
             currentCollider->GetOwner()->GetComponent<Rigidbody>()->mass == 0.0f))
            return;

        switch (currentCollider->colliderType)
        {
            case BOX:
            {
                auto* box = reinterpret_cast<BoxCollider*>(currentCollider);
                if (box->GetOwner() == capsule.GetOwner())
                    return;

                result = CheckBoxCapsuleCollision(*box, capsule);
                box->isColliding = capsule.isColliding = result.hasCollision;

                if (!result.hasCollision)
                    return;

                if (capsule.IsTrigger())
                {
                    EmitTrigger(&capsule);
                    return;
                }

                float penetrationDepth = result.penetrationDepth;
                glm::vec3 normal = result.collisionNormal;
                glm::vec3 contactPoint = result.collisionPoint;

                Rigidbody* thisRB = currentCollider->GetOwner()->GetComponent<Rigidbody>();
                Rigidbody* otherRB = capsule.GetOwner()->GetComponent<Rigidbody>();

                if (thisRB && otherRB)
                {
                    thisRB->OnCollision(otherRB, contactPoint, normal, penetrationDepth);
                }
                else if (thisRB)
                {
                    thisRB->OnCollisionStatic(contactPoint, normal);
                }

                glm::vec3 separation = GetSeparation();

                if (thisRB && otherRB)
                {
                    float invMassSum = thisRB->inverseMass + otherRB->inverseMass;
                    if (invMassSum > 0.0f)
                    {
                        glm::vec3 correctionThis = separation * (thisRB->inverseMass / invMassSum);
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                     correctionThis);
                    }
                }
                else if (thisRB)
                {
                    currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                 separation);
                }

                EmitCollision(&capsule);
                break;
            }

            case SPHERE:
            {
                auto* sphere = reinterpret_cast<SphereCollider*>(currentCollider);
                if (sphere->GetOwner() == capsule.GetOwner())
                    return;

                result = CheckCapsuleSphereCollision(capsule, *sphere);
                sphere->isColliding = capsule.isColliding = result.hasCollision;

                if (!result.hasCollision)
                    return;

                if (capsule.IsTrigger())
                {
                    EmitTrigger(&capsule);
                    return;
                }

                float penetrationDepth = result.penetrationDepth;
                glm::vec3 normal = result.collisionNormal;
                glm::vec3 contactPoint = result.collisionPoint;

                Rigidbody* thisRB = currentCollider->GetOwner()->GetComponent<Rigidbody>();
                Rigidbody* otherRB = capsule.GetOwner()->GetComponent<Rigidbody>();

                if (thisRB && otherRB)
                {
                    thisRB->OnCollision(otherRB, contactPoint, normal, penetrationDepth);
                }
                else if (thisRB)
                {
                    thisRB->OnCollisionStatic(contactPoint, normal);
                }

                glm::vec3 separation = GetSeparation();

                if (thisRB && otherRB)
                {
                    float invMassSum = thisRB->inverseMass + otherRB->inverseMass;
                    if (invMassSum > 0.0f)
                    {
                        glm::vec3 correctionThis = separation * (thisRB->inverseMass / invMassSum);
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     correctionThis);
                    }
                }
                else if (thisRB)
                {
                    currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                 separation);
                }

                EmitCollision(&capsule);
                break;
            }

            case CAPSULE:
            {
                auto* otherCapsule = reinterpret_cast<CapsuleCollider*>(currentCollider);
                if (otherCapsule->GetOwner() == capsule.GetOwner())
                    return;

                result = CheckCapsuleCapsuleCollision(capsule, *otherCapsule);
                otherCapsule->isColliding = capsule.isColliding = result.hasCollision;

                if (!result.hasCollision)
                    return;

                if (capsule.IsTrigger())
                {
                    EmitTrigger(&capsule);
                    return;
                }

                float penetrationDepth = result.penetrationDepth;
                glm::vec3 normal = result.collisionNormal;
                glm::vec3 contactPoint = result.collisionPoint;

                Rigidbody* thisRB = currentCollider->GetOwner()->GetComponent<Rigidbody>();
                Rigidbody* otherRB = capsule.GetOwner()->GetComponent<Rigidbody>();

                if (thisRB && otherRB)
                {
                    thisRB->OnCollision(otherRB, contactPoint, normal, penetrationDepth);
                }
                else if (thisRB)
                {
                    thisRB->OnCollisionStatic(contactPoint, normal);
                }

                glm::vec3 separation = GetSeparation();

                if (thisRB && otherRB)
                {
                    float invMassSum = thisRB->inverseMass + otherRB->inverseMass;
                    if (invMassSum > 0.0f)
                    {
                        glm::vec3 correctionThis = separation * (thisRB->inverseMass / invMassSum);
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                     correctionThis);
                    }
                }
                else if (thisRB)
                {
                    currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                 separation);
                }

                EmitCollision(&capsule);
                break;
            }
        }
    }

    void ColliderVisitor::SetCurrentCollider(Collider* collider) { this->currentCollider = collider; }

    void ColliderVisitor::ManageCollisions()
    {
        if (!currentCollider || currentCollider->IsStatic())
            return;

        for (auto* collider : spatialPartitioning->GetPotentialCollisions(currentCollider))
        {
            if (collider == this->currentCollider || collider->GetOwner() == nullptr)
                continue;

            collider->AcceptCollision(*this);
        }
    }

} // namespace Engine
