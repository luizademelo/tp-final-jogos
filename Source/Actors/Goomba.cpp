//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Goomba.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Goomba::Goomba(Game* game, float forwardSpeed, float deathTime)
        : Actor(game)
        , mDyingTimer(deathTime)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
{
    // --------------
    // TODO - PARTE 5
    // --------------

    // TODO 2.1 (2 linhas): Crie o `RigidBodyComponent` com massa `1.0f` e coeficiente de atrito `0.0`
    //  (basta omitir esse parâmetro para inicializa-lo com zero). Altere a velocidade inicial horizontal do
    //  goomba para `mForwardSpeed`.
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));

    // TODO 2.2 (1 linha): Crie o `AABBColliderComponent` com dimensões `(0,0,Game::TILE_SIZE,Game::TILE_SIZE)` e
    //  tipo de colisão `ColliderLayer::Enemy`.
    mColliderComponent = new AABBColliderComponent(this, 0, 0,
                                                   Game::TILE_SIZE, Game::TILE_SIZE,
                                                   ColliderLayer::Enemy);

    // TODO 2.3 (1 linha): Crie o componente `DrawAnimatedComponent` passando os caminhos da imagem (.png) e
    //  dos dados (.json) do sprite sheet do goomba que você criou com o FreeTexturePacker.
    //  - 2.3.1 (2 linha): Utilize a função `AddAnimation` para adicionar as animações "dead" e "walk".
    //  - 2.3.2 (1 linha): Utilize a função `SetAnimation` para definir a animação inicial como "walk".
    //  - 2.3.3 (1 linha): Utilize a função `SetAnimFPS` para definir a taxa de atualização de quadros da animação
    //    para 5.0f.
    mDrawComponent = new DrawAnimatedComponent(this,
                                                  "../Assets/Sprites/Goomba/Goomba.png",
                                                  "../Assets/Sprites/Goomba/Goomba.json");

    mDrawComponent->AddAnimation("dead", {0});
    mDrawComponent->AddAnimation("walk", {1, 2});
    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(5.0f);
}

void Goomba::Kill()
{
    // --------------
    // TODO - PARTE 5
    // --------------

    // TODO 3 (~4 linhas): altere a animação para "dead" e o valor da variável mIsDying para verdadeiro.
    //  Além disso, desabilite os componentes `mRigidBodyComponent` e  `mColliderComponent`
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Goomba::OnUpdate(float deltaTime)
{
    // --------------
    // TODO - PARTE 5
    // --------------

    // TODO 4.1 (~4 linhas): Verifique se a variável `mIsDying` é verdadeira. Se for, decremente o cronômetro
    //  `mDyingTimer` pelo delta time. Quando esse cronômetro chegar a zero, altere o estado do goomba para
    //  `ActorState::Destroy`
    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f) {
            mState = ActorState::Destroy;
        }
    }

    // TODO 4.2 (~2 linhas): Verifique se a posição vertical do goomba é maior do que o tamanho da tela. Se for,
    //  altere o estado do goomba para `ActorState::Destroy`
    if (GetPosition().y > GetGame()->GetWindowHeight()) {
        mState = ActorState::Destroy;
    }
}

void Goomba::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    // --------------
    // TODO - PARTE 5
    // --------------

    // TODO 5.1 (~4 linhas): Verifique se a colisão ocorreu  com um objeto to tipo
    //  `CollisionSide::Blocks` ou `CollisionSide::Enemy`. Se sim, altere a velocidade do goomba para
    //  que ele se mova para o lado oposto com velocidade `mForwardSpeed`.
    if ((other->GetLayer() == ColliderLayer::Blocks || other->GetLayer() == ColliderLayer::Enemy)) {
        if (minOverlap > 0) {
            mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));
        }
        else {
            mRigidBodyComponent->SetVelocity(Vector2(mForwardSpeed, 0.0f));
        }
    }

    //  TODO 5.2: Verifique se a colisão ocorreu com um objeto do tipo `CollisionSide::Player`.
    //   Se sim, mate o jogador com o método `Kill()` do jogador.
    if (other->GetLayer() == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    }
}

void Goomba::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{

}
