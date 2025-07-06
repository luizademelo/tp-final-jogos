//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "Actor.h"

class Hero : public Actor
{
public:
    explicit Hero(Game* game, float forwardSpeed = 3000.0f, float jumpSpeed = -750.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;
    void OnHandleKeyPress(const int key, const bool isPressed) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;
    void Win(AABBColliderComponent *poleCollider);
    void Shoot();
    int GetLivesCount(){return mLivesCount; }
    void SetPowerUp();
    bool HasPowerUp(){return mHasPowerUp; }

private:
    static const int POLE_SLIDE_TIME = 1; // Time in seconds to slide down the pole

    void ManageAnimations();

    float mForwardSpeed;
    float mJumpSpeed;
    float mPoleSlideTimer;
    bool mIsRunning;
    bool mIsOnStairs;
    bool mIsDying;
    float mShootCooldown = 0.5f;
    float mShootTimer = 0;
    int mLivesCount = 3;
    float mPowerUpTimer = 5;
    bool mHasPowerUp = false;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    class DrawSpriteComponent* mCircleDrawComponent;
};