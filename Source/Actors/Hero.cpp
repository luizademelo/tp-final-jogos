//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Hero.h"
#include "Block.h"
#include "Coffee.h"
#include "Shot.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Hero::Hero(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsOnStairs(false)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
        , mPoleSlideTimer(0.0f)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mColliderComponent = new AABBColliderComponent(this, 
                        Game::TILE_SIZE * 0.25f,  
                        Game::TILE_SIZE * 0.1f,   
                        Game::TILE_SIZE * 0.5f,   
                        Game::TILE_SIZE + int(Game::TILE_SIZE * mScale),
                        ColliderLayer::Player);

    mDrawComponent = new DrawAnimatedComponent(this,
                                              "../Assets/Sprites/Hero/texture.png",
                                              "../Assets/Sprites/Hero/texture.json");

    mDrawComponent->AddAnimation("Dead", {0});
    mDrawComponent->AddAnimation("idle", {7});
    mDrawComponent->AddAnimation("jump", {6});
    mDrawComponent->AddAnimation("run", {4, 6});
    mDrawComponent->AddAnimation("win", {7});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);
    mDrawComponent->SetScale(3.0f);
}

void Hero::OnProcessInput(const uint8_t* state)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    if (state[SDL_SCANCODE_D])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed);
        mRotation = 0.0f;
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_A])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * -mForwardSpeed);
        mRotation = Math::Pi;
        mIsRunning = true;
    }

    if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A])
    {
        mIsRunning = false;
    }

    if (state[SDL_SCANCODE_X] && mShootTimer <= 0.0f) {
        Shoot();
        mShootTimer = mShootCooldown;
    }

    if (state[SDL_SCANCODE_P]) {
        SDL_Log("x: %f, y: %f", GetPosition().x, GetPosition().y);
    }
}

void Hero::OnHandleKeyPress(const int key, const bool isPressed)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    // Jump
    if (key == SDLK_SPACE && isPressed && mIsOnGround)
    {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
        mIsOnGround = false;

        // Play jump sound
        mGame->GetAudio()->PlaySound("Jump.mp3");
    }
}

void Hero::OnUpdate(float deltaTime)
{
    
    if (mImmunityTimer > 0.0f) {
        mImmunityTimer -= deltaTime;
    }

    mShootTimer -= deltaTime;

    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f)
        {
            mState = ActorState::Destroy;
            mGame->SetGameScene(Game::GameScene::GameOver, 0.0f);
            mGame->SetGamePlayState(Game::GamePlayState::GameOver);
        }
        return; // evita qualquer outra lógica
    }

    if (mHasPowerUp) {
        mPowerUpTimer -= deltaTime;
        SDL_Log("Power up: %d", mPowerUpTimer);
        if (mPowerUpTimer <= 0.0f) {
            mHasPowerUp = false;
        }
    }

    // Check if Hero is off the ground
    if (mRigidBodyComponent && mRigidBodyComponent->GetVelocity().y != 0) {
        mIsOnGround = false;
    }

    // Kill Hero if he falls below the screen
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing && mPosition.y > mGame->GetWindowHeight())
    {
        Kill();
    }

    if (GetPosition().x >= GetGame()->GetStairsPosition()) {
        mGame->SetGamePlayState(Game::GamePlayState::Leaving);
    }

    if (mGame->GetGamePlayState() == Game::GamePlayState::Leaving)
    {
        // Stop Hero movement
        mState = ActorState::Destroy;

        // Get current level
        auto CurrentScene = GetGame()->GetGameScene();

        if (CurrentScene == Game::GameScene::Level1)
        {
            mGame->SetGameScene(Game::GameScene::Level2, 1.5f);
        }

        else if (CurrentScene == Game::GameScene::Level2)
        {
            mGame->SetGameScene(Game::GameScene::Level3, 1.5f);
        }

        else if (CurrentScene == Game::GameScene::Level3)
        {
            mGame->SetGameScene(Game::GameScene::Level4, 1.5f);
        }

        else if (CurrentScene == Game::GameScene::Level4)
        {
            mGame->SetGameScene(Game::GameScene::Victory, 1.5f);
        }

        return;
    }

    ManageAnimations();
}

void Hero::ManageAnimations()
{
    if(mIsDying)
    {
        mDrawComponent->SetAnimation("Dead");
    }
    else if(mIsOnStairs)
    {
        mDrawComponent->SetAnimation("win");
    }
    else if (mIsOnGround && mIsRunning)
    {
        mDrawComponent->SetAnimation("run");
    }
    else if (mIsOnGround && !mIsRunning)
    {
        mDrawComponent->SetAnimation("idle");
    }
    else if (!mIsOnGround)
    {
        mDrawComponent->SetAnimation("jump");
    }
}

void Hero::Kill()
{
    // Se tem power-up, apenas perde o power-up na primeira "morte"
    if (mHasPowerUp) {
        mHasPowerUp = false;
        SetImmunityTimer(1.0f);
        GetGame()->GetAudio()->PlaySound("Ouch.mp3");
        return;
    }
    
    // Se não tem power-up, perde vida
    mLivesCount--;
    
    if (mLivesCount > 0) {
        // Ainda tem vidas, apenas toca som e dá imunidade
        mGame->GetAudio()->PlaySound("Ouch.mp3");
        SetImmunityTimer(1.0f);
        return;
    }
    
    // Se chegou aqui, não tem mais vidas
    mIsDying = true;
    // mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    mDrawComponent->SetAnimation("Dead");

    // Disable collider and rigid body
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();
    mGame->GetAudio()->PlaySound("DeadHero.mp3");

    mGame->SetGameScene(Game::GameScene::GameOver, 2.0f);
    // if (mLivesCount <= 0)
    // {
    //     mGame->SetGameScene(Game::GameScene::GameOver, 2.0f);
    // }
    // else
    // {
    //     mGame->ResetGameScene(3.5f); // Reset the game scene after 3 seconds
    // }
}

void Hero::Win(AABBColliderComponent *poleCollider)
{
    mDrawComponent->SetAnimation("win");
    mGame->SetGamePlayState(Game::GamePlayState::LevelComplete);

    // Set mario velocity to go down
    mRigidBodyComponent->SetVelocity(Vector2::UnitY * 100.0f); // 100 pixels per second
    mRigidBodyComponent->SetApplyGravity(false);

    // Disable collider
    poleCollider->SetEnabled(false);

    // Adjust mario x position to grab the pole
    mPosition.Set(poleCollider->GetOwner()->GetPosition().x + Game::TILE_SIZE / 4.0f, mPosition.y);

    // Stop music
    mGame->GetAudio()->StopAllSounds();

    mPoleSlideTimer = POLE_SLIDE_TIME; // Start the pole slide timer
}

void Hero::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy && mImmunityTimer <= 0.0f) {
        // Se não tem power-up, perde vida
        if (!mHasPowerUp) {
            // Determinar direção do impulso (contrária à colisão)
            float knockbackDirection = (minOverlap > 0) ? -1.0f : 1.0f;
            float knockbackForce = 300.0f;

            // Aplicar impulso horizontal e pequeno pulo
            mRigidBodyComponent->SetVelocity(Vector2(knockbackDirection * knockbackForce, -100.0f));

            // Reproduzir som "ouch"
            GetGame()->GetAudio()->PlaySound("Ouch.mp3");
            
            // Reduzir vida
            mLivesCount--;
            
            // Definir período de imunidade
            SetImmunityTimer(1.0f);
            
            // Se ficou sem vidas, morre
            if (mLivesCount <= 0) {
                Kill();
            }
        }
        else {
            // Com power-up, apenas perde o power-up
            mHasPowerUp = false;
            SetImmunityTimer(1.0f);
            GetGame()->GetAudio()->PlaySound("Ouch.mp3");
        }
    }
    else if (other->GetLayer() == ColliderLayer::Pole)
    {
        mIsOnStairs = true;
        Win(other);
    }
    else if (other->GetLayer() == ColliderLayer::Coffee) {
        this->SetPowerUp();
        Coffee* coffee = static_cast<Coffee*>(other->GetOwner());
        coffee->SetState(ActorState::Destroy);
        mGame->GetAudio()->PlaySound("Collect.mp3");
    }
}

void Hero::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        // Verificar se o Hero está caindo (velocidade Y positiva) e o overlap indica que está por cima
        if (mRigidBodyComponent->GetVelocity().y > 0 && minOverlap > 0) {
            // Hero está pousando em cima do inimigo
            other->GetOwner()->Kill();
            // Dar um pequeno pulo após pisar no inimigo
            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, -200.0f));
            
            // Play stomp sound
            mGame->GetAudio()->PlaySound("Stomp.wav");
        }
        else {
            // Se não está pisando por cima, o herói deve morrer
            if (mImmunityTimer <= 0.0f) {
                Kill();
            }
        }
    }
    else if (other->GetLayer() == ColliderLayer::Blocks)
    {
        if (!mIsOnGround)
        {
            // Play bump sound
            mGame->GetAudio()->PlaySound("Bump.wav");

            // Cast actor to Block to call OnBump
            Block* block = static_cast<Block*>(other->GetOwner());
            block->OnBump();
        }
    }
    else if (other->GetLayer() == ColliderLayer::Coffee) {
        this->SetPowerUp();
        Coffee* coffee = static_cast<Coffee*>(other->GetOwner());
        coffee->SetState(ActorState::Destroy);
        mGame->GetAudio()->PlaySound("Collect.mp3");
    }
}

void Hero::Shoot() {
    Vector2 velocity = Vector2(100.0f, 0.0f); // Shoots right
    bool isLeft = mDrawComponent->GetOwner()->GetRotation() == Math::Pi ? true : false;
    if (isLeft) {
        velocity.x = -100;
    }
    auto shot = new Shot(GetGame(), velocity, ColliderLayer::Player, "../Assets/Sprites/Shots/PaperPlane/texture.png", "../Assets/Sprites/Shots/PaperPlane/texture.json");
    shot->SetScale(3.0f);
    if (isLeft) {
        shot->SetRotation(Math::Pi);
    }

    Vector2 dir = mDrawComponent->GetOwner()->GetRotation() == Math::Pi ? Vector2(-1, 0) : Vector2(1, 0);
    Vector2 pos = GetPosition() + dir * Game::TILE_SIZE ;
    shot->SetPosition(pos);
}

void Hero::SetPowerUp() {
    mPowerUpTimer = 5;
    mHasPowerUp = true;
}
