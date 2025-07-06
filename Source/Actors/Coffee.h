//
// Created by Pc on 06/07/2025.
//

#pragma once

#include "Actor.h"
#include <string>

class Coffee : public Actor {
public:
    explicit Coffee(Game* game, const bool isStatic = true);
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
private:
    class DrawAnimatedComponent *mDrawComponent ;
    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    ColliderLayer mColliderLayer;
};