//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Hero.h"
#include "Block.h"
#include "Shot.h"
#include "../Game.h"
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
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE * mScale ,Game::TILE_SIZE + int(Game::TILE_SIZE * mScale),
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
    mShootTimer -= deltaTime;

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
    mLivesCount -= 1;
    if (mLivesCount > 0) {
        mGame->GetAudio()->PlaySound("Ouch.mp3");
        return;
    }
    mIsDying = true;
    mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    mDrawComponent->SetAnimation("Dead");

    // Disable collider and rigid body
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();
    mGame->GetAudio()->PlaySound("DeadHero.mp3");

    if (mLivesCount <= 0)
    {
        mGame->SetGameScene(Game::GameScene::GameOver, 2.0f);
    }
    else
    {
        mGame->ResetGameScene(3.5f); // Reset the game scene after 3 seconds
    }
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
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        Kill();
    }
    else if (other->GetLayer() == ColliderLayer::Pole)
    {
        mIsOnStairs = true;
        Win(other);
    }
}

void Hero::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        other->GetOwner()->Kill();
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed / 2.5f));

        // Play jump sound
        mGame->GetAudio()->PlaySound("Stomp.wav");
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
}

void Hero::Shoot() {
    Vector2 velocity = Vector2(100.0f, 0.0f); // Shoots right
    if (mDrawComponent->GetOwner()->GetRotation() == Math::Pi) {
        velocity.x = -100;
    }
    auto shot = new Shot(GetGame(), velocity, ColliderLayer::Player, "../Assets/Sprites/Shots/Bullets/texture.png", "../Assets/Sprites/Shots/Bullets/texture.json");
    Vector2 dir = mDrawComponent->GetOwner()->GetRotation() == Math::Pi ? Vector2(-1, 0) : Vector2(1, 0);
    Vector2 pos = GetPosition() + dir * Game::TILE_SIZE ;
    // pos.y -= 20;
    shot->SetPosition(pos);
    shot->GetComponent<DrawAnimatedComponent>()->SetScale(0.5f);
}
