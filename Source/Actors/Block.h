//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "Actor.h"
#include <string>

enum BlockType {
    BRICK,
    NORMAL
};
class Block : public Actor
{
public:
    explicit Block(Game* game, const std::string &texturePath);
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent *other) override;
    class RigidBodyComponent* mRigidBodyComponent;
    BlockType type = BlockType::NORMAL;
    void OnUpdate(float deltaTime) override;
    Vector2 mOriginalPosition;
};
