//
// Created by Pc on 28/06/2025.
//

#pragma once
#include "Actor.h"
#include <string>

class DrawSpriteComponent;
class DrawAnimatedComponent;
class DrawPolygonComponent;
class CircleColliderComponent;

class Shot : public Actor {
public:
    Shot(Game* game, const Vector2& velocity, ColliderLayer layer, std::string spriteSheetPath, std::string spriteSheetData);
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void Destroy();

private:
    RigidBodyComponent* mRigidBodyComponent;
    AABBColliderComponent* mColliderComponent;
    DrawAnimatedComponent* mDrawComponent;
    ColliderLayer mColliderLayer;

    float mLivenessTime = 3;
    float mTimer = 0;
};
