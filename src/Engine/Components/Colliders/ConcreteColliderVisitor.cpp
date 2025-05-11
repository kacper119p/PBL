#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "ColliderVisitor.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "SpatialPartitioning.h"
#include "SphereCollider.h"
#include "spdlog/spdlog.h"

namespace Engine
{

    ConcreteColliderVisitor::ConcreteColliderVisitor() :
        currentCollider(nullptr), collisionDetected(false), spatialPartitioning(&SpatialPartitioning::GetInstance())
    {
        result.collisionNormal = glm::vec3(0.0f);
        result.collisionPoint = glm::vec3(0.0f);
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


    CollisionResult ConcreteColliderVisitor::CheckBoxBoxCollision(const BoxCollider& box1, const BoxCollider& box2)
    {
        CollisionResult result;

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
        glm::vec3 collisionNormal;

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

            float penetration =
                    glm::abs(glm::dot(toCenter, normAxis)) -
                    (glm::abs(glm::dot(normAxis, aX)) * half1.x + glm::abs(glm::dot(normAxis, aY)) * half1.y +
                     glm::abs(glm::dot(normAxis, aZ)) * half1.z + glm::abs(glm::dot(normAxis, bX)) * half2.x +
                     glm::abs(glm::dot(normAxis, bY)) * half2.y + glm::abs(glm::dot(normAxis, bZ)) * half2.z);

            if (penetration < minPenetration)
            {
                minPenetration = penetration;
                collisionNormal = normAxis;
            }
        }

        result.hasCollision = true;
        result.collisionNormal = collisionNormal;
        result.collisionPoint = center1 + collisionNormal * minPenetration;
        return result;
    }


    CollisionResult ConcreteColliderVisitor::CheckBoxSphereCollision(const BoxCollider& box,
                                                                     const SphereCollider& sphere)
    {
        CollisionResult result;

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
            result.collisionPoint = closestPoint;
            result.collisionNormal = glm::normalize(closestPoint - sphereCenter);
        }

        return result;
    }


    CollisionResult ConcreteColliderVisitor::CheckBoxCapsuleCollision(const BoxCollider& box,
                                                                      const CapsuleCollider& capsule)
    {
        CollisionResult result;
        result.hasCollision = false;

        const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 boxCenter = glm::vec3(boxTransform[3]);
        glm::vec3 xAxis = glm::normalize(glm::vec3(boxTransform[0]));
        glm::vec3 yAxis = glm::normalize(glm::vec3(boxTransform[1]));
        glm::vec3 zAxis = glm::normalize(glm::vec3(boxTransform[2]));

        float scaleX = glm::length(glm::vec3(boxTransform[0]));
        float scaleY = glm::length(glm::vec3(boxTransform[1]));
        float scaleZ = glm::length(glm::vec3(boxTransform[2]));

        glm::vec3 halfExtents = glm::vec3(box.GetWidth() * 0.5f * scaleX, box.GetHeight() * 0.5f * scaleY,
                                          box.GetDepth() * 0.5f * scaleZ);

        glm::vec3 capsuleCenter = glm::vec3(capsuleTransform[3]);
        glm::vec3 capsuleUp = glm::normalize(glm::vec3(capsuleTransform * glm::vec4(0, 1, 0, 0)));

        float capsuleHalfHeight = (capsule.GetHeight() * 0.5f);
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

        if (distSq <= capsuleRadius * capsuleRadius)
        {
            result.hasCollision = true;
            result.collisionPoint = boxP;
            result.collisionNormal = glm::length(delta) > 1e-6f ? glm::normalize(delta) : glm::vec3(0, 1, 0);
        }

        return result;
    }


    CollisionResult ConcreteColliderVisitor::CheckSphereSphereCollision(const SphereCollider& sphere1,
                                                                        const SphereCollider& sphere2)
    {
        CollisionResult result;

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
            result.collisionPoint = center1 + glm::normalize(delta) * radius1;
            result.collisionNormal = glm::normalize(delta);
        }

        return result;
    }


    CollisionResult ConcreteColliderVisitor::CheckCapsuleSphereCollision(const CapsuleCollider& capsule,
                                                                         const SphereCollider& sphere)
    {
        CollisionResult result;

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
            result.collisionPoint = closestPoint;
            result.collisionNormal = glm::normalize(closestPoint - sphereCenter);
        }

        return result;
    }


    CollisionResult ConcreteColliderVisitor::CheckCapsuleCapsuleCollision(const CapsuleCollider& capsule1,
                                                                          const CapsuleCollider& capsule2)
    {
        CollisionResult result;

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
            result.collisionNormal = glm::normalize(closestPoint1 - closestPoint2);
        }

        return result;
    }


    glm::vec3 ConcreteColliderVisitor::GetSeparationBoxBox(const BoxCollider& box1, const BoxCollider& box2)
    {
        const glm::mat4& transform1 = box1.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& transform2 = box2.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 halfExtents1 = glm::vec3(box1.GetWidth(), box1.GetHeight(), box1.GetDepth()) * 0.5f;
        glm::vec3 halfExtents2 = glm::vec3(box2.GetWidth(), box2.GetHeight(), box2.GetDepth()) * 0.5f;

        glm::vec3 center1 = glm::vec3(transform1[3]);
        glm::vec3 center2 = glm::vec3(transform2[3]);

        glm::mat3 rot1 = glm::mat3(transform1);
        glm::mat3 rot2 = glm::mat3(transform2);

        glm::vec3 axes1[3] = {rot1[0], rot1[1], rot1[2]};
        glm::vec3 axes2[3] = {rot2[0], rot2[1], rot2[2]};

        glm::vec3 delta = center2 - center1;

        float minPenetration = std::numeric_limits<float>::max();
        glm::vec3 smallestAxis = glm::vec3(0.0f);

        for (int i = 0; i < 3; ++i)
        {
            glm::vec3 axis = glm::normalize(axes1[i]);
            float proj1 = halfExtents1.x * glm::abs(glm::dot(axis, axes1[0])) +
                          halfExtents1.y * glm::abs(glm::dot(axis, axes1[1])) +
                          halfExtents1.z * glm::abs(glm::dot(axis, axes1[2]));

            float proj2 = halfExtents2.x * glm::abs(glm::dot(axis, axes2[0])) +
                          halfExtents2.y * glm::abs(glm::dot(axis, axes2[1])) +
                          halfExtents2.z * glm::abs(glm::dot(axis, axes2[2]));

            float distance = glm::abs(glm::dot(delta, axis));
            float overlap = proj1 + proj2 - distance;

            if (overlap < 0.0f)
                return glm::vec3(0.0f); // no collision

            if (overlap < minPenetration)
            {
                minPenetration = overlap;
                smallestAxis = axis * (glm::dot(delta, axis) < 0.0f ? -1.0f : 1.0f);
            }
        }

        for (int i = 0; i < 3; ++i)
        {
            glm::vec3 axis = glm::normalize(axes2[i]);
            float proj1 = halfExtents1.x * glm::abs(glm::dot(axis, axes1[0])) +
                          halfExtents1.y * glm::abs(glm::dot(axis, axes1[1])) +
                          halfExtents1.z * glm::abs(glm::dot(axis, axes1[2]));

            float proj2 = halfExtents2.x * glm::abs(glm::dot(axis, axes2[0])) +
                          halfExtents2.y * glm::abs(glm::dot(axis, axes2[1])) +
                          halfExtents2.z * glm::abs(glm::dot(axis, axes2[2]));

            float distance = glm::abs(glm::dot(delta, axis));
            float overlap = proj1 + proj2 - distance;

            if (overlap < 0.0f)
                return glm::vec3(0.0f); // no collision

            if (overlap < minPenetration)
            {
                minPenetration = overlap;
                smallestAxis = axis * (glm::dot(delta, axis) < 0.0f ? -1.0f : 1.0f);
            }
        }

        return smallestAxis * glm::max(0.0f, minPenetration);
    }


    glm::vec3 ConcreteColliderVisitor::GetSeparationBoxSphere(const BoxCollider& box, const SphereCollider& sphere)
    {
        const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 sphereCenter = glm::vec3(sphereTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec3 boxCenter = glm::vec3(boxTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glm::vec3 xAxis = glm::normalize(glm::vec3(boxTransform[0]));
        glm::vec3 yAxis = glm::normalize(glm::vec3(boxTransform[1]));
        glm::vec3 zAxis = glm::normalize(glm::vec3(boxTransform[2]));

        float scaleX = glm::length(glm::vec3(boxTransform[0]));
        float scaleY = glm::length(glm::vec3(boxTransform[1]));
        float scaleZ = glm::length(glm::vec3(boxTransform[2]));

        glm::vec3 halfExtents = glm::vec3(box.GetWidth() * 0.5f * scaleX, box.GetHeight() * 0.5f * scaleY,
                                          box.GetDepth() * 0.5f * scaleZ);

        glm::vec3 delta = sphereCenter - boxCenter;

        glm::vec3 axes[] = {xAxis, yAxis, zAxis};

        float minPenetration = std::numeric_limits<float>::max();
        glm::vec3 smallestAxis(0.0f);

        for (const glm::vec3& axis : axes)
        {
            float boxProjection = glm::abs(glm::dot(axis, xAxis)) * halfExtents.x +
                                  glm::abs(glm::dot(axis, yAxis)) * halfExtents.y +
                                  glm::abs(glm::dot(axis, zAxis)) * halfExtents.z;

            float sphereProjection = sphere.GetRadius();
            float distance = glm::abs(glm::dot(delta, axis));
            float overlap = boxProjection + sphereProjection - distance;

            if (overlap < 0.0f)
            {
                return glm::vec3(0.0f);
            }

            if (overlap < minPenetration)
            {
                minPenetration = overlap;
                smallestAxis = axis * (glm::dot(delta, axis) < 0.0f ? -1.0f : 1.0f);
            }
        }
        return smallestAxis * minPenetration;
    }


    glm::vec3 ConcreteColliderVisitor::GetSeparationBoxCapsule(const BoxCollider& box, const CapsuleCollider& capsule)
    {
        const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 boxCenter = glm::vec3(boxTransform[3]);
        glm::vec3 xAxis = glm::normalize(glm::vec3(boxTransform[0]));
        glm::vec3 yAxis = glm::normalize(glm::vec3(boxTransform[1]));
        glm::vec3 zAxis = glm::normalize(glm::vec3(boxTransform[2]));

        float scaleX = glm::length(glm::vec3(boxTransform[0]));
        float scaleY = glm::length(glm::vec3(boxTransform[1]));
        float scaleZ = glm::length(glm::vec3(boxTransform[2]));

        glm::vec3 halfExtents = glm::vec3(box.GetWidth() * 0.5f * scaleX, box.GetHeight() * 0.5f * scaleY,
                                          box.GetDepth() * 0.5f * scaleZ);

        glm::vec3 capsuleCenter = glm::vec3(capsuleTransform[3]);
        glm::vec3 capsuleUp = glm::normalize(glm::vec3(capsuleTransform * glm::vec4(0, 1, 0, 0)));

        float capsuleHalfHeight = (capsule.GetHeight() * 0.5f) - capsule.GetRadius();
        glm::vec3 capsuleStart = capsuleCenter - capsuleUp * capsuleHalfHeight;
        glm::vec3 capsuleEnd = capsuleCenter + capsuleUp * capsuleHalfHeight;

        glm::vec3 capsuleSegment = capsuleEnd - capsuleStart;
        float segmentLenSq = glm::length2(capsuleSegment);
        float t = 0.0f;

        if (segmentLenSq > 1e-6f)
        {
            t = glm::dot(boxCenter - capsuleStart, capsuleSegment) / segmentLenSq;
            t = glm::clamp(t, 0.0f, 1.0f);
        }

        glm::vec3 closestPointOnCapsule = capsuleStart + t * capsuleSegment;

        glm::vec3 d = closestPointOnCapsule - boxCenter;
        glm::vec3 closestPointOnBox = boxCenter;

        float distX = glm::clamp(glm::dot(d, xAxis), -halfExtents.x, halfExtents.x);
        float distY = glm::clamp(glm::dot(d, yAxis), -halfExtents.y, halfExtents.y);
        float distZ = glm::clamp(glm::dot(d, zAxis), -halfExtents.z, halfExtents.z);

        closestPointOnBox += distX * xAxis + distY * yAxis + distZ * zAxis;

        glm::vec3 separationVector = closestPointOnCapsule - closestPointOnBox;
        float distance = glm::length(separationVector);
        float radius = capsule.GetRadius();

        if (distance < radius && distance > 1e-6f)
        {
            float penetration = radius - distance;
            return glm::normalize(separationVector) * penetration;
        }

        return glm::vec3(0.0f);
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

        glm::vec3 sphereCenter = glm::vec3(sphereTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        float sphereRadius = sphere.GetRadius();

        glm::vec3 capsuleStart = glm::vec3(capsuleTransform * glm::vec4(0.0f, -capsule.GetHeight() / 2.0f, 0.0f, 1.0f));
        glm::vec3 capsuleEnd = glm::vec3(capsuleTransform * glm::vec4(0.0f, capsule.GetHeight() / 2.0f, 0.0f, 1.0f));
        float capsuleRadius = capsule.GetRadius();

        glm::vec3 capsuleAxis = capsuleEnd - capsuleStart;
        float capsuleAxisLengthSquared = glm::dot(capsuleAxis, capsuleAxis);

        float t = glm::dot(sphereCenter - capsuleStart, capsuleAxis) / capsuleAxisLengthSquared;
        t = glm::clamp(t, 0.0f, 1.0f);
        glm::vec3 closestPointOnCapsule = capsuleStart + t * capsuleAxis;

        glm::vec3 delta = sphereCenter - closestPointOnCapsule;
        float distance = glm::length(delta);
        float combinedRadius = sphereRadius + capsuleRadius;

        if (distance < combinedRadius)
        {
            float penetration = combinedRadius - distance;

            const float MaxPenetrationLimit = 0.1f;
            penetration = glm::min(penetration, MaxPenetrationLimit);

            return glm::normalize(delta) * penetration;
        }

        return glm::vec3(0.0f);
    }


    glm::vec3 ConcreteColliderVisitor::GetSeparationCapsuleCapsule(const CapsuleCollider& capsule1,
                                                                   const CapsuleCollider& capsule2)
    {
        const glm::mat4& transform1 = capsule1.GetTransform()->GetLocalToWorldMatrix();
        const glm::mat4& transform2 = capsule2.GetTransform()->GetLocalToWorldMatrix();

        glm::vec3 capsule1Start = glm::vec3(transform1 * glm::vec4(0.0f, -capsule1.GetHeight() / 2.0f, 0.0f, 1.0f));
        glm::vec3 capsule1End = glm::vec3(transform1 * glm::vec4(0.0f, capsule1.GetHeight() / 2.0f, 0.0f, 1.0f));

        glm::vec3 capsule2Start = glm::vec3(transform2 * glm::vec4(0.0f, -capsule2.GetHeight() / 2.0f, 0.0f, 1.0f));
        glm::vec3 capsule2End = glm::vec3(transform2 * glm::vec4(0.0f, capsule2.GetHeight() / 2.0f, 0.0f, 1.0f));

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

        glm::vec3 delta = closestPointOnCapsule2 - closestPointOnCapsule1;
        float distance = glm::length(delta);
        float combinedRadius = capsule1.GetRadius() + capsule2.GetRadius();

        if (distance < combinedRadius)
        {
            float penetration = combinedRadius - distance;

            const float MaxPenetrationLimit = 0.1f;
            penetration = glm::min(penetration, MaxPenetrationLimit);

            return glm::normalize(delta) * penetration;
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
                auto* boxCollider = reinterpret_cast<BoxCollider*>(this->currentCollider);
                if (box.GetOwner() == boxCollider->GetOwner())
                    return;

                result = CheckBoxBoxCollision(box, *boxCollider);

                if (result.hasCollision)
                {
                    box.isColliding = boxCollider->isColliding = result.hasCollision;
                    glm::vec3 separation = GetSeparationBoxBox(box, *boxCollider);

                    if (currentCollider->GetOwner()->GetComponent<Engine::RigidBody>())
                    {
                        currentCollider->GetOwner()->GetComponent<Engine::RigidBody>()->OnCollision(
                                result.collisionNormal, result.collisionPoint, 0.001f);
                    }
                    else
                    {
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     separation);
                    }
                }
                // TODO: emit collision event
                break;
            }
            case SPHERE:
            {
                auto* sphere = reinterpret_cast<SphereCollider*>(this->currentCollider);
                if (sphere->GetOwner() == box.GetOwner())
                    return;
                result = CheckBoxSphereCollision(box, *sphere);
                box.isColliding = sphere->isColliding = result.hasCollision;
                if (result.hasCollision)
                {
                    glm::vec3 separation = GetSeparationBoxSphere(box, *sphere);
                    if (currentCollider->GetOwner()->GetComponent<Engine::RigidBody>())
                    {
                        currentCollider->GetOwner()->GetComponent<Engine::RigidBody>()->OnCollision(
                                result.collisionNormal, result.collisionPoint, 0.001f);
                    }
                    else
                    {
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     separation);
                    }
                }
                // TODO: emit collision event
                break;
            }
            case CAPSULE:
            {
                auto* capsule = reinterpret_cast<CapsuleCollider*>(this->currentCollider);
                if (capsule->GetOwner() == box.GetOwner())
                    return;
                result = CheckBoxCapsuleCollision(box, *capsule);
                box.isColliding = capsule->isColliding = result.hasCollision;
                if (result.hasCollision)
                {
                    glm::vec3 separation = GetSeparationBoxCapsule(box, *capsule);
                    if (currentCollider->GetOwner()->GetComponent<Engine::RigidBody>())
                    {
                        currentCollider->GetOwner()->GetComponent<Engine::RigidBody>()->OnCollision(
                                result.collisionNormal, result.collisionPoint, 0.001f);
                    }
                    else
                    {
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     separation);
                    }
                }
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
                auto* box = reinterpret_cast<BoxCollider*>(this->currentCollider);
                if (box->GetOwner() == sphere.GetOwner())
                    return;
                result = CheckBoxSphereCollision(*box, sphere);
                box->isColliding = sphere.isColliding = result.hasCollision;
                if (result.hasCollision)
                {
                    glm::vec3 separation = GetSeparationBoxSphere(*box, sphere);
                    if (currentCollider->GetOwner()->GetComponent<Engine::RigidBody>())
                    {
                        currentCollider->GetOwner()->GetComponent<Engine::RigidBody>()->OnCollision(
                                result.collisionNormal, result.collisionPoint, 0.001f);
                    }
                    else
                    {
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                     separation);
                    }
                }
                // TODO: emit collision event
                break;
            }
            case SPHERE:
            {
                auto* otherSphere = reinterpret_cast<SphereCollider*>(this->currentCollider);
                if (otherSphere->GetOwner() == sphere.GetOwner())
                    return;
                result = CheckSphereSphereCollision(sphere, *otherSphere);
                otherSphere->isColliding = sphere.isColliding = result.hasCollision;
                if (result.hasCollision)
                {
                    glm::vec3 separation = GetSeparationSphereSphere(sphere, *otherSphere);
                    if (currentCollider->GetOwner()->GetComponent<Engine::RigidBody>())
                    {
                        currentCollider->GetOwner()->GetComponent<Engine::RigidBody>()->OnCollision(
                                result.collisionNormal, result.collisionPoint, 0.001f);
                    }
                    else
                    {
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     separation);
                    }
                }
                // TODO: emit collision event
                break;
            }
            case CAPSULE:
            {
                auto* capsule = reinterpret_cast<CapsuleCollider*>(this->currentCollider);
                if (capsule->GetOwner() == sphere.GetOwner())
                    return;
                result = CheckCapsuleSphereCollision(*capsule, sphere);
                capsule->isColliding = sphere.isColliding = result.hasCollision;
                if (result.hasCollision)
                {
                    glm::vec3 separation = GetSeparationSphereCapsule(sphere, *capsule);
                    if (currentCollider->GetOwner()->GetComponent<Engine::RigidBody>())
                    {
                        currentCollider->GetOwner()->GetComponent<Engine::RigidBody>()->OnCollision(
                                result.collisionNormal, result.collisionPoint, 0.001f);
                    }
                    else
                    {
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() -
                                                                     separation);
                    }
                }
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
                auto* box = reinterpret_cast<BoxCollider*>(this->currentCollider);
                if (box->GetOwner() == capsule.GetOwner())
                    return;
                result = CheckBoxCapsuleCollision(*box, capsule);
                box->isColliding = capsule.isColliding = result.hasCollision;
                if (result.hasCollision)
                {
                    glm::vec3 separation = GetSeparationBoxCapsule(*box, capsule);
                    if (currentCollider->GetOwner()->GetComponent<Engine::RigidBody>())
                    {
                        currentCollider->GetOwner()->GetComponent<Engine::RigidBody>()->OnCollision(
                                result.collisionNormal, result.collisionPoint, 0.001f);
                    }
                    else
                    {
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     separation);
                    }
                }
                // TODO: emit collision event
                break;
            }
            case SPHERE:
            {
                auto* sphere = reinterpret_cast<SphereCollider*>(this->currentCollider);
                if (sphere->GetOwner() == capsule.GetOwner())
                    return;
                result = CheckCapsuleSphereCollision(capsule, *sphere);
                sphere->isColliding = capsule.isColliding = result.hasCollision;
                if (result.hasCollision)
                {
                    glm::vec3 separation = GetSeparationSphereCapsule(*sphere, capsule);
                    if (currentCollider->GetOwner()->GetComponent<Engine::RigidBody>())
                    {
                        currentCollider->GetOwner()->GetComponent<Engine::RigidBody>()->OnCollision(
                                result.collisionNormal, result.collisionPoint, 0.001f);
                    }
                    else
                    {
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     separation);
                    }
                }
                // TODO: emit collision event
                break;
            }
            case CAPSULE:
            {
                auto* otherCapsule = reinterpret_cast<CapsuleCollider*>(this->currentCollider);
                if (otherCapsule->GetOwner() == capsule.GetOwner())
                    return;

                result = CheckCapsuleCapsuleCollision(capsule, *otherCapsule);
                otherCapsule->isColliding = capsule.isColliding = result.hasCollision;
                if (result.hasCollision)
                {
                    glm::vec3 separation = GetSeparationCapsuleCapsule(capsule, *otherCapsule);
                    if (currentCollider->GetOwner()->GetComponent<Engine::RigidBody>())
                    {
                        currentCollider->GetOwner()->GetComponent<Engine::RigidBody>()->OnCollision(
                                result.collisionNormal, result.collisionPoint, 0.001f);
                    }
                    else
                    {
                        currentCollider->GetTransform()->SetPosition(currentCollider->GetTransform()->GetPosition() +
                                                                     separation);
                    }
                }
                // TODO: emit collision event
                break;
            }
        }
    }


    void ConcreteColliderVisitor::SetCurrentCollider(Collider* collider) { this->currentCollider = collider; }


    void ConcreteColliderVisitor::ManageCollisions()
    {
        if (!currentCollider || currentCollider->IsStatic())
            return;

        for (auto* collider : spatialPartitioning->GetPotentialCollisions(currentCollider))
        {
            if (collider == this->currentCollider)
                continue;

            collider->AcceptCollision(*this);
        }
    }
} // namespace Engine
