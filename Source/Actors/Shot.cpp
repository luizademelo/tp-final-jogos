//
// Created by Pc on 28/06/2025.
//
#include "Shot.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Shot::Shot(Game* game, const Vector2& velocity, ColliderLayer layer, std::string spriteSheetPath, std::string spriteSheetData)
    : Actor(game)
{
    mColliderLayer = layer;
    mDrawComponent = new DrawAnimatedComponent(this, spriteSheetPath, spriteSheetData);
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mRigidBodyComponent->SetVelocity(velocity);
    mRigidBodyComponent->SetApplyGravity(false);
    mColliderComponent = mColliderLayer == ColliderLayer::PlayerShoot
                                        ? new AABBColliderComponent(this, 0, 0,36, 12, mColliderLayer)
                                        : new AABBColliderComponent(this, 3, 5,28, 16, mColliderLayer);
    mDrawComponent->AddAnimation("Normal", {0});
    mDrawComponent->SetAnimation("Normal");
    mDrawComponent->SetAnimFPS(5.0f);

}

void Shot::OnUpdate(float deltaTime)
{
    mTimer += deltaTime;
    if (mTimer > mLivenessTime) {
        mState = ActorState::Destroy;
    }
}

void Shot::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {

    this->Destroy();

    if (other->GetLayer() == ColliderLayer::Player || other->GetLayer() == ColliderLayer::Enemy) {
        other->GetOwner()->OnHorizontalCollision(minOverlap, mColliderComponent);
    }

    if (other->GetLayer() == ColliderLayer::PlayerShoot || other->GetLayer() == ColliderLayer::EnemyShoot) {
        auto shot = static_cast<Shot*>(other->GetOwner());
        shot->Destroy();
    }

}
void Shot::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {

}

void Shot::Destroy() {
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    mColliderComponent->SetEnabled(false);
    mGame->GetAudio()->PlaySound("wood.wav");
    mState = ActorState::Destroy;
}

