//
// Created by Lucas N. Ferreira on 30/09/23.
//

#pragma once

#include "Actor.h"
#include "LifeHeart.h"

class Enemy : public Actor
{
public:
    explicit Enemy(Game* game, float forwardSpeed = 100.0f, float deathTime = 0.5f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void ShootProjectile();
    void Kill() override;
    void BumpKill(const float bumpForce = 300.0f);

private:
    bool mIsDying;
    float mForwardSpeed;
    float mDyingTimer;
    float mShootTimer = 2.0f; // Time between shots
    float mShootCooldown = 2.0f;


    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    LifeHeart* mLifeBar;
};
