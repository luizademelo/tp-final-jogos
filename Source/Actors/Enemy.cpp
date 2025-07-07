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

Enemy::Enemy(Game* game, EnemyType type, float forwardSpeed, float deathTime)
        : Actor(game)
        , mDyingTimer(deathTime)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mDirectionChangeTimer(0.0f)
        , mType(type)  
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mColliderComponent = new AABBColliderComponent(this, 
                        Game::TILE_SIZE * 0.75f,
                        28,
                        Game::TILE_SIZE - 5,
                        (Game::TILE_SIZE * 2) - 18,
                      ColliderLayer::Enemy);
    mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));
    
    // Selecionar a textura com base no tipo
    std::string texturePath;
    std::string jsonPath;
    
    switch(type) {
        case ENEMY_TYPE1:
            texturePath = "../Assets/Sprites/Enemy/texture.png";
            jsonPath = "../Assets/Sprites/Enemy/texture.json";
            break;
        case ENEMY_TYPE2:
            texturePath = "../Assets/Sprites/Enemy2/texture.png";
            jsonPath = "../Assets/Sprites/Enemy2/texture.json";
            break;
        case ENEMY_TYPE3:
            texturePath = "../Assets/Sprites/Enemy3/texture.png";
            jsonPath = "../Assets/Sprites/Enemy3/texture.json";
            break;
        case ENEMY_TYPE4:
            texturePath = "../Assets/Sprites/Enemy4/texture.png";
            jsonPath = "../Assets/Sprites/Enemy4/texture.json";
            break;
    }
    
    mDrawComponent = new DrawAnimatedComponent(this, texturePath, jsonPath);

    // Configurações básicas para todos os tipos
    mDrawComponent->AddAnimation("Dead", {16});
    mDrawComponent->AddAnimation("Idle", {5});
    
    // Configurações específicas por tipo
    ConfigureForType(type);
    
    SetRotation(Math::Pi);
}

void Enemy::ConfigureForType(EnemyType type) {
    switch(type) {
        case ENEMY_TYPE1:
            // Inimigo padrão (nível 1)
            mDrawComponent->AddAnimation("walk", {4,5,6,7});
            mDrawComponent->SetAnimation("walk");
            mDrawComponent->SetAnimFPS(10.0f);
            mDrawComponent->SetScale(3.0f);
            mShootCooldown = 2.0f;
            break;
            
        case ENEMY_TYPE2:
            // Inimigo mais rápido (nível 2)
            mDrawComponent->AddAnimation("walk", {4,5,6,7});
            mDrawComponent->SetAnimation("walk");
            mDrawComponent->SetAnimFPS(12.0f); // Animação mais rápida
            mDrawComponent->SetScale(3.0f);
            mForwardSpeed = 150.0f; // Movimentação mais rápida
            mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));
            mShootCooldown = 1.8f; // Atira um pouco mais rápido
            break;
            
        case ENEMY_TYPE3:
            // Inimigo mais forte (nível 3)
            mDrawComponent->AddAnimation("walk", {4,5,6,7});
            mDrawComponent->SetAnimation("walk");
            mDrawComponent->SetAnimFPS(10.0f);
            mDrawComponent->SetScale(3.5f); // Maior
            mShootCooldown = 1.5f; // Atira mais rápido
            break;
            
        case ENEMY_TYPE4:
            // Inimigo final (nível 4)
            mDrawComponent->AddAnimation("walk", {4,5,6,7});
            mDrawComponent->SetAnimation("walk");
            mDrawComponent->SetAnimFPS(14.0f); // Animação mais rápida
            mDrawComponent->SetScale(4.0f); // Muito maior
            mForwardSpeed = 180.0f; // Mais rápido
            mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));
            mShootCooldown = 1.0f; // Atira muito mais rápido
            break;
    }
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

    // Atualizar timer de mudança de direção
    if (mDirectionChangeTimer > 0.0f) {
        mDirectionChangeTimer -= deltaTime;
    }

    if (GetPosition().y > GetGame()->GetWindowHeight())
    {
        mState = ActorState::Destroy;
    }
}

void Enemy::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying || mDirectionChangeTimer > 0.0f) return;

    if (other->GetLayer() == ColliderLayer::Blocks)
    {
        bool wasGoingRight = (mRigidBodyComponent->GetVelocity().x > 0);
        
        if (wasGoingRight) {
            mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, mRigidBodyComponent->GetVelocity().y));
            SetRotation(Math::Pi);
        } else {
            mRigidBodyComponent->SetVelocity(Vector2(mForwardSpeed, mRigidBodyComponent->GetVelocity().y));
            SetRotation(0.0f);
        }
        
        mDirectionChangeTimer = 0.1f;
    }

    else if (other->GetLayer() == ColliderLayer::PlayerShoot) {
        this->Kill();
    }

}

void Enemy::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{

}

void Enemy::ShootProjectile()
{
    Vector2 velocity = Vector2(-1400.0f, 0.0f); // Shoots left
    if (mRigidBodyComponent->GetVelocity().x > 0.0f) {
        velocity.x *= -1;
    }
    auto shot = new Shot(GetGame(), velocity, ColliderLayer::EnemyShoot, "../Assets/Sprites/Shots/Email/texture.png", "../Assets/Sprites/Shots/Email/texture.json");

    Vector2 pos = Vector2(GetPosition().x,  GetGame()->GetWindowHeight() - (3 * Game::TILE_SIZE));
    shot->SetPosition(pos);
    shot->GetComponent<DrawAnimatedComponent>()->SetScale(0.5f);
}

