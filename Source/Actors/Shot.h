//
// Created by Pc on 28/06/2025.
//

#pragma once
#include "Actor.h"

class DrawSpriteComponent;
class DrawAnimatedComponent;
class DrawPolygonComponent;
class CircleColliderComponent;

class Shot : public Actor {
public:
    Shot(Game* game, const Vector2& velocity,ColliderLayer layer);
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

private:
    RigidBodyComponent* mRigidBodyComponent;
    AABBColliderComponent* mColliderComponent;
    DrawAnimatedComponent* mDrawComponent;
};
