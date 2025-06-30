//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Enemy.h"

#include "Shot.h"
#include "../Game.h"
#include "../Math.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Random.h"

Enemy::Enemy(Game* game, float forwardSpeed, float deathTime)
        : Actor(game)
        , mDyingTimer(deathTime)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));

    mColliderComponent = new AABBColliderComponent(this, 0, 0,
                                                   Game::TILE_SIZE, Game::TILE_SIZE,
                                                   ColliderLayer::Enemy);

    mDrawComponent = new DrawAnimatedComponent(this,
                                                  "../Assets/Sprites/Enemy/texture.png",
                                                  "../Assets/Sprites/Enemy/texture.json");

    mDrawComponent->AddAnimation("Dead", {16});
    mDrawComponent->AddAnimation("Idle", {5});
    mDrawComponent->AddAnimation("walk", {4,5,6,7});
    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(5.0f);
    mDrawComponent->SetScale(1.5f);

    SetRotation(Math::Pi);
}

void Enemy::Kill()
{
    mGame->GetAudio()->PlaySound("DeadEnemy.mp3");
    mIsDying = true;
    mDrawComponent->SetAnimation("Dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Enemy::BumpKill(const float bumpForce)
{
    mDrawComponent->SetAnimation("Idle");
    mGame->GetAudio()->PlaySound("DeadEnemy.mp3");
    mRigidBodyComponent->SetVelocity(Vector2(bumpForce/2.0f, -bumpForce));
    mColliderComponent->SetEnabled(false);

    // Flip upside down (180 degrees)
    SetRotation(180);
}

void Enemy::OnUpdate(float deltaTime)
{
    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f) {
            mState = ActorState::Destroy;
        }
    }else {
          mShootTimer -= deltaTime;
        if (mShootTimer <= 0.0f) {
            ShootProjectile();
            mShootTimer = mShootCooldown;
        }
    }

    if (GetPosition().y > GetGame()->GetWindowHeight())
    {
        mState = ActorState::Destroy;
    }
}

void Enemy::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if ((other->GetLayer() == ColliderLayer::Blocks || other->GetLayer() == ColliderLayer::Enemy))
    {
        if (minOverlap > 0) {
            mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));
            SetRotation(Math::Pi);
        }
        else {
            mRigidBodyComponent->SetVelocity(Vector2(mForwardSpeed, 0.0f));
            SetRotation(0.0f);
        }
    }

    if (other->GetLayer() == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    }
}

void Enemy::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    }
}

void Enemy::ShootProjectile()
{
    Vector2 velocity = Vector2(-150.0f, 0.0f); // Shoots left
    if (mRigidBodyComponent->GetVelocity().x > 0.0f) {
        velocity.x = 150;
    }
    auto shot = new Shot(GetGame(), velocity, ColliderLayer::Enemy, "../Assets/Sprites/Shots/Email/texture.png", "../Assets/Sprites/Shots/Email/texture.json");
    Vector2 dir = mRigidBodyComponent->GetVelocity().x < 0 ? Vector2(-1, 0) : Vector2(1, 0);
    Vector2 pos = GetPosition() + dir * Game::TILE_SIZE * 0.5f;
    shot->SetPosition(pos);
    shot->GetComponent<DrawAnimatedComponent>()->SetScale(0.5f);
}

