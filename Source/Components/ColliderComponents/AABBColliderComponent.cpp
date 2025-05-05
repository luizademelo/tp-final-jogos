//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
        ColliderLayer layer, bool isStatic, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(Vector2((float)dx, (float)dy))
        ,mIsStatic(isStatic)
        ,mWidth(w)
        ,mHeight(h)
        ,mLayer(layer)
{
    mOwner->GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
    mOwner->GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.1 (1 linha): Calcule (e retorne) o ponto mínimo dessa AABB. A variável `mOffset`
    //  define a posição da AABB com relação a posição do objeto dono do componente. Portanto,
    //  basta somar a posição do objeto dono do componente a esse deslocamento.
    return mOwner->GetPosition() + mOffset;
}

Vector2 AABBColliderComponent::GetMax() const
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.2 (1 linha): Calcule (e retorne) o ponto máximo dessa AABB. As variáveis membro
    //  `mWidth` e `mHeight` definem a altura e a largura da AABB, respectivamente. Portanto,
    //  basta somar a largura à coordenada x e a altura à coordenada y do ponto mínimo da AABB
    //  (utilize o método `GetMin` implementado na etapa anterior).
    return GetMin() + Vector2((float)mWidth, (float)mHeight);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 2 (~5 linhas): Verifique se esta AABB está colidindo com a AABB b passada como parâmetro.
    //  Retorne verdadeiro se estiver e falso caso contrário. Utilize os métodos `GetMin` e `GetMax`
    //  para acessar os pontos de mínimo e máximo das duas AABBs. É importante destacar que quando os valores
    //  foram iguais, as AABBs NÃO estão colidindo. Portanto, utilize o operador < para verificar
    //  se as AABBs estão colidindo.
    return (GetMin().x < b.GetMax().x && GetMax().x > b.GetMin().x &&
            GetMin().y < b.GetMax().y && GetMax().y > b.GetMin().y);
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent* b) const
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 3.1 (2 linhas): Calcule as interseções verticais (top e down) entre as duas AABBs.
    float top = GetMin().y - b->GetMax().y; // Top
    float down = GetMax().y - b->GetMin().y; // Down

    // TODO 3.2 (1 linha): Encontre e retorne a interseção com menor valor absoluto.
    return (Math::Abs(top) < Math::Abs(down)) ? top : down;
}

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent* b) const
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 3.3 (2 linhas): Calcule as interseções horizontais (right e left) entre as duas AABBs.
    float right = GetMax().x - b->GetMin().x; // Right
    float left = GetMin().x - b->GetMax().x; // Left

    // TODO 3.4 (1 linha): Encontre e retorne a interseção com menor valor absoluto.
    return (Math::Abs(left) < Math::Abs(right)) ? left : right;
}

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent *rigidBody)
{
    // --------------
    // TODO - PARTE 3
    // --------------
    if (mIsStatic) return false;

    auto colliders = mOwner->GetGame()->GetColliders();

    // TODO 4.1: Ordene os colliders de acordo com a direção do movimento horizontal do objeto dono
    //  do componente. Se o objeto dono do componente estiver se movendo para a direita, ordene os colliders
    //  de forma que os colliders mais próximos do ponto máximo da AABB do objeto dono do componente
    //  venham primeiro. Se o objeto dono do componente estiver se movendo para a esquerda, ordene os colliders
    //  de forma que os colliders mais próximos do ponto mínimo da AABB do objeto dono do componente
    //  venham primeiro. Se o objeto dono do componente não estiver se movendo horizontalmente, não faça nada,
    //  apenas retore 0.0f. Utilize o método `std::sort` e o método `GetMin` e `GetMax` para acessar os pontos mínimo
    //  e máximo da AABB do objeto dono do componente.

    if (rigidBody->GetVelocity().x > .0f) {
        std::sort(colliders.begin(), colliders.end(),
                  [this](const AABBColliderComponent* a, const AABBColliderComponent* b) {
                      return GetMax().x - a->GetMin().x < GetMax().x - b->GetMin().x;
                  });
    }
    else if (rigidBody->GetVelocity().x < .0f) {
        std::sort(colliders.begin(), colliders.end(),
                  [this](const AABBColliderComponent* a, const AABBColliderComponent* b) {
                      return GetMin().x - a->GetMax().x < GetMin().x - b->GetMax().x;
                  });
    }
    else return 0.0f;

    // TODO 4.2: Percorra todos os colliders e verifique se o objeto dono do componente
    //  está colidindo com algum deles. Antes dessa verificação, verifique se o collider está habilitado
    //  e se não é o próprio collider do objeto dono do componente. Em ambos os casos, ignore o collider e
    //  continue a verificação. Se o objeto dono do componente estiver colidindo horizontalmente com algum collider,
    //  resolva a colisão horizontal e retorne o valor da interseção horizontal mínima. Utilize o método
    //  `GetMinHorizontalOverlap` para calcular a interseção horizontal mínima. Utilize o método
    //  `ResolveHorizontalCollisions` para resolver a colisão horizontal. Utilize o método `OnHorizontalCollision`
    //  do objeto dono do componente para notificar a colisão ao Actor dono desse componente.
    for (auto& collider : colliders) {
        if (collider == this) continue;
        if (!collider->IsEnabled()) continue;

        if (Intersect(*collider)) {
            float minHorizontalOverlap = GetMinHorizontalOverlap(collider);
            ResolveHorizontalCollisions(rigidBody, minHorizontalOverlap);

            mOwner->OnHorizontalCollision(minHorizontalOverlap, collider);
            return minHorizontalOverlap;
        }
    }

    return 0.0f;
}

float AABBColliderComponent::DetectVertialCollision(RigidBodyComponent *rigidBody) {
    // --------------
    // TODO - PARTE 3
    // --------------
    if (mIsStatic) return false;

    auto colliders = mOwner->GetGame()->GetColliders();

    // TODO 4.3: Ordene os colliders de acordo com a direção do movimento vertical do objeto dono
    //  do componente. Se o objeto dono do componente estiver se movendo para baixo, ordene os colliders
    //  de forma que os colliders mais próximos do ponto máximo da AABB do objeto dono do componente
    //  venham primeiro. Se o objeto dono do componente estiver se movendo para cima, ordene os colliders
    //  de forma que os colliders mais próximos do ponto mínimo da AABB do objeto dono do componente venham primeiro.
    //  Se o objeto dono do componente não estiver se movendo verticalmente, não faça nada, apenas retore 0.0f.
    //  Utilize o método `std::sort` e o método `GetMin` e `GetMax` para acessar os pontos mínimo e máximo da AABB
    //  do objeto dono do componente.
    if(rigidBody->GetVelocity().y > .0f) {
        std::sort(colliders.begin(), colliders.end(),
                  [this](const AABBColliderComponent* a, const AABBColliderComponent* b) {
                      return GetMax().y - a->GetMin().y < GetMax().y - b->GetMin().y;
                  });
    }
    else if (rigidBody->GetVelocity().y < .0f) {
        std::sort(colliders.begin(), colliders.end(),
                  [this](const AABBColliderComponent* a, const AABBColliderComponent* b) {
                      return GetMin().y - a->GetMax().y < GetMin().y - b->GetMax().y;
                  });
    }
    else return 0.0f;

    // TODO 4.4: Percorra todos os colliders e verifique se o objeto dono do componente
    //  está colidindo com algum deles. Antes dessa verificação, verifique se o collider está habilitado
    //  e se não é o próprio collider do objeto dono do componente. Em ambos os casos, ignore o collider e
    //  continue a verificação. Se o objeto dono do componente estiver colidindo verticalmente com algum collider,
    //  resolva a colisão vertical e retorne o valor da interseção vertical mínima. Utilize o método
    //  `GetMinVerticalOverlap` para calcular a interseção vertical mínima. Utilize o método
    //  `ResolveVerticalCollisions` para resolver a colisão vertical. Utilize o método `OnVerticalCollision`
    //  do objeto dono do componente para notificar a colisão ao Actor dono desse componente.

    for (auto& collider : colliders) {
        if (collider == this) continue;
        if (!collider->IsEnabled()) continue;

        if (Intersect(*collider)) {
            float minVerticalOverlap = GetMinVerticalOverlap(collider);
            ResolveVerticalCollisions(rigidBody, minVerticalOverlap);

            // Callback only for closest (first) collision
            mOwner->OnVerticalCollision(minVerticalOverlap, collider);
            return minVerticalOverlap;
        }
    }

    return 0.0f;
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap)
{   // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 5.1 (2 linhas): Aplique a resolução de colisão horizontal. Para isso, mova o objeto dono do componente
    //  para o lado oposto da colisão. Para isso, subtraia o valor da interseção horizontal mínima da posição do
    //  objeto dono do componente. Utilize o método `SetPosition` do objeto dono do componente. Altere também a
    //  velocidade horizontal do objeto dono do componente para 0.0f. Utilize o método `SetVelocity`.
    mOwner->SetPosition(mOwner->GetPosition() - Vector2(minXOverlap, 0.0f));
    rigidBody->SetVelocity(Vector2(0.f, rigidBody->GetVelocity().y));

}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 5.2 (2 linhas): Aplique a resolução de colisão vertical. Para isso, mova o objeto dono do componente
    //  para o lado oposto da colisão. Para isso, subtraia o valor da interseção vertical mínima da posição do
    //  objeto dono do componente. Utilize o método `SetPosition` do objeto dono do componente. Altere também a
    //  velocidade vertical do objeto dono do componente para 0.0f. Utilize o método `SetVelocity`.
    mOwner->SetPosition(mOwner->GetPosition() - Vector2(0.0f, minYOverlap));
    rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0.f));

    // TODO 5.3 (1 linha): Verifique se a colisão vertical ocorreu por cima, ou seja, se o valor da interseção
    //  vertical mínima é maior que 0.0f. Se sim, chame o método `SetOnGround` do objeto dono do componente.
    if (minYOverlap > .0f) {
        mOwner->SetOnGround();
    }
}