//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    // Initialize HUD elements
    AddText("Tempo", Vector2(mGame->GetWindowWidth() - 150.0f, HUD_POS_Y), Vector2(CHAR_WIDTH * 4, WORD_HEIGHT), POINT_SIZE);
    mTimeText = AddText("400", Vector2(mGame->GetWindowWidth() - 150.0f + CHAR_WIDTH, HUD_POS_Y + WORD_OFFSET), Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);

    // AddText("World", Vector2(mGame->GetWindowWidth() - 300.0f, HUD_POS_Y), Vector2(CHAR_WIDTH * 5, WORD_HEIGHT), POINT_SIZE);
    mLevelName = AddText("Nível 1", Vector2(mGame->GetWindowWidth() - 300.0f + CHAR_WIDTH, HUD_POS_Y ), Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);

    AddText("Nerdovaldo", Vector2(40.0f, HUD_POS_Y), Vector2(CHAR_WIDTH * 10, WORD_HEIGHT), POINT_SIZE);
    mPowerUp = AddText("Invencibilidade desativada", Vector2(40.0f, HUD_POS_Y + 50), Vector2(CHAR_WIDTH * 26, WORD_HEIGHT), POINT_SIZE);
    // mScoreCounter = AddText("000000", Vector2(40.0f, HUD_POS_Y + WORD_OFFSET), Vector2(CHAR_WIDTH * 6, WORD_HEIGHT), POINT_SIZE);
    for (int i = 0; i < mLivesCount; i++) {
        UIImage* heart = AddImage("../Assets/Sprites/Misc/Heart.png",
                           Vector2(40.0 + CHAR_WIDTH * i, HUD_POS_Y + WORD_HEIGHT + 10),
                           Vector2(CHAR_WIDTH, CHAR_WIDTH));
        mHearts.emplace_back(heart);
    }
}

HUD::~HUD()
{

}

void HUD::SetTime(int time)
{
    mTimeText->SetText(std::to_string(time));

    // Reinsert position and size based on the number of digits
    int numDigits = std::to_string(time).length();
    mTimeText->SetPosition(Vector2(mGame->GetWindowWidth() - 150.0f + (4 - numDigits) * CHAR_WIDTH, HUD_POS_Y + WORD_OFFSET));
    mTimeText->SetSize(Vector2(CHAR_WIDTH * numDigits, WORD_HEIGHT));
}

void HUD::SetLevelName(const std::string &levelName)
{
    mLevelName->SetText(levelName);
}

void HUD::SetLifeCount(int lifeCount)
{


    for (int i = lifeCount; i < mHearts.size(); ++i) {
        mHearts[i]->SetSize(Vector2(0,0));
    }

    mLivesCount = lifeCount;
}

void HUD::SetPowerUp(bool hasPowerUp) {
    if (hasPowerUp) {
        mPowerUp->SetText("Invencibilidade ativada!");
    }else {
        mPowerUp->SetText("");
    }

}
