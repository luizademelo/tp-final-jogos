#pragma once

#include "Actor.h"

class Enemy : public Actor
{
public:
    enum EnemyType {
        ENEMY_TYPE1, // Nível 1
        ENEMY_TYPE2, // Nível 2
        ENEMY_TYPE3, // Nível 3
        ENEMY_TYPE4  // Nível 4
    };
    
    explicit Enemy(Game* game, 
                  EnemyType type = ENEMY_TYPE1,
                  float forwardSpeed = 120.0f, 
                  float deathTime = 0.5f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void ShootProjectile();
    void Kill() override;
    void BumpKill(const float bumpForce = 300.0f);

private:
    void ConfigureForType(EnemyType type);
    
    bool mIsDying;
    float mForwardSpeed;
    float mDyingTimer;
    float mShootTimer = 2.0f; // Time between shots
    float mShootCooldown = 2.0f;
    float mDirectionChangeTimer;
    EnemyType mType;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};