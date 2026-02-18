#include "resources.h"
#include "types.h"
#include "gameContext.h"
#include "game.h"
#include "resourceManager.h"
#include "database.h"
#include <wingdi.h>



void DrawGameOverScreen(HDC hdc, RECT rect)
{
    int w = rect.right;
    int h = rect.bottom;

    HDC overlayDC = CreateCompatibleDC(hdc);
    HBITMAP overlayBmp = CreateCompatibleBitmap(hdc, w, h);
    HBITMAP oldBmp = (HBITMAP)SelectObject(overlayDC, overlayBmp);

    FillRect(overlayDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // alpha blend
    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 160;
    bf.AlphaFormat = 0;

    AlphaBlend(
        hdc,
        0, 0, w, h,
        overlayDC,
        0, 0, w, h,
        bf
    );

    SelectObject(hdc, gRes.hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    const char* text;
    if(gGame.currentLevel == 7 && gGame.gameState.isLevelCleared)
    text = "GAME PASSED! :)";
    else
        text = "GAME OVER";
    SIZE sz;

    GetTextExtentPoint32(hdc, text, strlen(text), &sz);

    TextOut(
        hdc,
        (w - sz.cx) / 2,
        h / 4,
        text,
        strlen(text)
    );

    gGame.restartButtonInfo.x = w/2 - gGame.restartButtonInfo.width - 10;
    gGame.restartButtonInfo.y = h/3;
    gGame.homeButtonInfo.x = w/2 + 10;
    gGame.homeButtonInfo.y = h/3;

    DrawButton(hdc,gRes.restartButton, gRes.restartButtonMask, gGame.restartButtonInfo);
    DrawButton(hdc, gRes.homeButton, gRes.homeButtonMask, gGame.homeButtonInfo);

    // cleanup
    SelectObject(overlayDC, oldBmp);
    DeleteObject(overlayBmp);
    DeleteDC(overlayDC);
}

void SaveFinalScore() {
    char playerName[32];
    const char* gameMode;
    int finalScore = gGame.totalScore;

    if (gGame.gameState.isMultiplayer) {
        snprintf(playerName, sizeof(playerName), "%s's Team", gGame.playerName);
        gameMode = "MultiPlayer";
    } else {
        strncpy(playerName, gGame.playerName, sizeof(playerName));
        playerName[sizeof(playerName)-1] = '\0';
        gameMode = "SinglePlayer";
    }

    if (!SaveScore(playerName, finalScore, gGame.currentLevel, gameMode)) {
        OutputDebugStringA("Failed to save score to database!\n");
    }
}
