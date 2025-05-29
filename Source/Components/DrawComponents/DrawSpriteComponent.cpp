//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawSpriteComponent.h"

#include <iostream>
#include <ostream>

#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawSpriteComponent::DrawSpriteComponent(class Actor* owner, const std::string &texturePath, const int width, const int height, const int drawOrder)
        :DrawComponent(owner, drawOrder)
        ,mWidth(width)
        ,mHeight(height)
{
    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 1.1 (1 linhas): Utilize a função LoadTexture da classe Game para criar uma textura a partir da
    //  imagem `texturePath` passada como parâmetro no construtor. Armazene o ponteiro retornada (SDLTexture*)
    //  na variável membro 'mSpriteSheetSurface'.
    mSpriteSheetSurface = GetGame()->LoadTexture(texturePath);
}

void DrawSpriteComponent::Draw(SDL_Renderer *renderer)
{
    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 1.2 (~5 linhas): Utilize a função SDL_RenderCopyEx para desenhar a textura armazenada
    //  no atributo mSpriteSheetSurface. Você terá que criar um SDL_Rect para definir a região
    //  da tela onde será desenhado o sprite. Para que o objeto seja desenhado em relação a posição da câmera,
    //  subtraia a posição da câmera da posição do objeto. Além disso, você terá que criar uma flag do tipo
    //  SDL_RendererFlip para definir se o sprite será desenhado virado à direita ou à
    //  esquerda. A orientação do sprite (esquerda ou direita) depende da rotação do objeto dono do sprite.
    //  Se a rotação for zero, o sprite deve ser desenhado virado à direita. Se for igual a MAth::Pi, deve
    //  ser desenhado à esquerda.

    SDL_Rect dstRect;
    dstRect.w = mWidth;
    dstRect.h = mHeight;

    Vector2 screenPos = mOwner->GetPosition() - GetGame()->GetCameraPos();
    dstRect.x = static_cast<int>(screenPos.x);
    dstRect.y = static_cast<int>(screenPos.y);
    // std::cout << dstRect.x << " " << dstRect.y << std::endl;

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mOwner->GetRotation() == Math::Pi) {
        flip = SDL_FLIP_HORIZONTAL;
    }

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, nullptr, &dstRect, 0, nullptr, flip);

}