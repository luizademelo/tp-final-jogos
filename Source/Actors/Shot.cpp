//
// Created by Pc on 28/06/2025.
//
#include "Shot.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Shot::Shot(Game* game, const Vector2& velocity)
    : Actor(game)
{

    mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Shots/ToiletPaper/texture.png", "../Assets/Sprites/Shots/ToiletPaper/texture.json");
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mRigidBodyComponent->SetVelocity(velocity);
    mColliderComponent = new AABBColliderComponent(this, 0, 0,
                                                   Game::TILE_SIZE, Game::TILE_SIZE,
                                                   ColliderLayer::Enemy);
    mDrawComponent->AddAnimation("Normal", {0});
    mDrawComponent->SetAnimation("Normal");
    mDrawComponent->SetAnimFPS(5.0f);
}

void Shot::OnUpdate(float deltaTime)
{
    if (GetPosition().x < 0 || GetPosition().x > GetGame()->GetWindowWidth() ||
        GetPosition().y < 0 || GetPosition().y > GetGame()->GetWindowHeight())
    {
        mState = ActorState::Destroy;
    }

}