#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <algorithm>
#include <stdio.h>
#include <string>
#include "multiplayer.h"



void RenderStaticUI(HDC hdc, RECT rect) {
    if (CURRENT_LEVEL.hStaticCache == NULL  || CURRENT_LEVEL.staticRedraw) {

        if (CURRENT_LEVEL.hdcCache) DeleteDC(CURRENT_LEVEL.hdcCache);
        if (CURRENT_LEVEL.hStaticCache) DeleteObject(CURRENT_LEVEL.hStaticCache);

        CURRENT_LEVEL.hdcCache = CreateCompatibleDC(hdc);
        CURRENT_LEVEL.hStaticCache = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        SelectObject(CURRENT_LEVEL.hdcCache, CURRENT_LEVEL.hStaticCache);

        RenderWalls(CURRENT_LEVEL.hdcCache, rect);

        // === RENDER PLACEHOLDER ZA LEVEL ===
        SelectObject(gRes.hdcMem, gRes.levelPlaceholderWhite);

        std::string levelText = "LEVEL " + std::to_string(gGame.currentLevel+1);
        HFONT oldFont = (HFONT)SelectObject(CURRENT_LEVEL.hdcCache, gRes.hFont);
        SetBkMode(CURRENT_LEVEL.hdcCache, TRANSPARENT);
        SetTextColor(CURRENT_LEVEL.hdcCache, RGB(60, 60, 60));

        SIZE textSize;
        GetTextExtentPoint32(CURRENT_LEVEL.hdcCache, levelText.c_str(), levelText.length(), &textSize);

        int barHeight = 25;
        int paddingX = 30;
        int boxW = textSize.cx + paddingX;
        int boxH = textSize.cy + paddingX;

        int placeholderX = (rect.right / 2) - (boxW / 2);
        int floorTop = rect.bottom - gGame.floorWall.height + barHeight;
        int placeholderY = floorTop + (gGame.floorWall.height - boxH) / 2;
        SelectObject(gRes.hdcMem, gRes.levelPlaceholderWhite);

        SetStretchBltMode(CURRENT_LEVEL.hdcCache, COLORONCOLOR);
        StretchBlt(CURRENT_LEVEL.hdcCache, placeholderX, placeholderY, boxW, boxH, gRes.hdcMem, 0, 0,
                   gGame.levelPlaceholderInfo.width, gGame.levelPlaceholderInfo.height, SRCAND);

        SelectObject(gRes.hdcMem, gRes.levelPlaceholderBlack);

        StretchBlt(CURRENT_LEVEL.hdcCache, placeholderX, placeholderY, boxW, boxH, gRes.hdcMem, 0, 0,
                   gGame.levelPlaceholderInfo.width, gGame.levelPlaceholderInfo.height, SRCPAINT);

        TEXTMETRIC tm;
        GetTextMetrics(CURRENT_LEVEL.hdcCache, &tm);
        int textX = placeholderX + (boxW - textSize.cx) / 2;
        int textY = placeholderY + (boxH - tm.tmHeight) / 2;
        TextOut(CURRENT_LEVEL.hdcCache, textX, textY, levelText.c_str(), levelText.length());

        SelectObject(CURRENT_LEVEL.hdcCache, oldFont);


        // === PLAYER PLACEHOLDERS ===
        int heartsTop = rect.bottom - gGame.floorWall.height + barHeight*2 + 10;
        int playerY = heartsTop + gGame.heartBgInfo.height + 10;

        if (gGame.gameState.isMultiplayer) {
            std::string p2Text = "PLAYER 2";

            SIZE p2Size;
            HFONT hTempFont = (HFONT)SelectObject(CURRENT_LEVEL.hdcCache, gRes.hFont);
            GetTextExtentPoint32(CURRENT_LEVEL.hdcCache, p2Text.c_str(), p2Text.length(), &p2Size);
            SelectObject(CURRENT_LEVEL.hdcCache, hTempFont);

            int p2Padding = 30;
            int p2BoxW = p2Size.cx + p2Padding;

            int p2X = rect.right - gGame.rightWall.width - p2BoxW;

            DrawPlayerPlaceholder(CURRENT_LEVEL.hdcCache, rect, p2Text, p2X, playerY,
                                 RGB(0, 0, 180), gGame.heartBgInfo.height + 6);
        }

        gGame.playerHolderInfo.x = gGame.leftWall.width;
        gGame.playerHolderInfo.y = playerY;

        DrawPlayerPlaceholder(CURRENT_LEVEL.hdcCache, rect, "PLAYER 1", gGame.leftWall.width, playerY,
                             RGB(180, 0, 0), gGame.heartBgInfo.height + 6);


        CURRENT_LEVEL.staticRedraw = false;
    }

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, CURRENT_LEVEL.hdcCache, 0, 0, SRCCOPY);
}

void RenderWalls(HDC hdc, RECT rect){
    int tileW = gGame.floorWall.width;
    int tileH = gGame.leftWall.height;

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    SetStretchBltMode(hdc, HALFTONE);
    SetBrushOrgEx(hdc, 0, 0, NULL);
    SelectObject(gRes.hdcMem, gRes.wall);
    StretchBlt(hdc, 0, 0, rect.right, rect.bottom, gRes.hdcMem, 0, 0, tileW, tileH, SRCCOPY);

    // BIJELI OKVIR
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Rectangle(
        hdc,
        bgX - 1,
        bgY,
        bgX + bgW + 2,
        bgY + bgH + 3
    );

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);

}


void RenderDynamicGameUI(HDC hdc, RECT rect)
{
    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    // PAUSE BUTTON //
    gGame.pauseButtonInfo.x = bgX + 15;
    DrawButton(hdc, gRes.pauseButton, gRes.pauseButtonMask, gGame.pauseButtonInfo);

    // SOUND BUTTON //
    gGame.soundButtonInfo.x = gGame.pauseButtonInfo.x + gGame.pauseButtonInfo.width + 15;
    DrawButton(hdc, gRes.soundButton, gRes.soundButtonMask, gGame.soundButtonInfo);

    // === TIME BAR ===
    int barHeight = 25;
    int barGap = 8;
    int barY = bgY + bgH + barGap;
    int barX = bgX;
    int maxBarWidth = bgW;

    // Pozadina trake
    HBRUSH hDarkBrush = CreateSolidBrush(RGB(100, 100, 100));
    RECT barBgRect = { barX, barY, barX + maxBarWidth, barY + barHeight };
    FillRect(hdc, &barBgRect, hDarkBrush);
    DeleteObject(hDarkBrush);

    // Ograničenje vremena na 0
    if (CURRENT_LEVEL.timeLeft < 0)
        CURRENT_LEVEL.timeLeft = 0;

    int currentWidth = (int)((CURRENT_LEVEL.timeLeft / maxTime) * maxBarWidth);

    HBRUSH hRedBrush = CreateSolidBrush(RGB(220, 0, 0));
    RECT barRect = { barX, barY, barX + currentWidth, barY + barHeight };
    FillRect(hdc, &barRect, hRedBrush);
    DeleteObject(hRedBrush);

    HBRUSH hLightRedBrush = CreateSolidBrush(RGB(255, 60, 60));
    RECT topBar = { barX, barY, barX + currentWidth, barY + 5 };
    FillRect(hdc, &topBar, hLightRedBrush);
    DeleteObject(hLightRedBrush);

    HPEN hWhitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hWhitePen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, barX, barY, barX + maxBarWidth, barY + barHeight);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hWhitePen);

    // === HEARTS & SCORE ===
    DrawHeartsAndScore(hdc, rect, barHeight);

    std::string levelText = "LEVEL " + std::to_string(gGame.currentLevel+1);

    SIZE textSize;
    GetTextExtentPoint32(
        hdc,
        levelText.c_str(),
        levelText.length(),
        &textSize
    );

    // Padding oko teksta
    int paddingX = 40;
    int paddingY = 20;

    int boxW = textSize.cx + paddingX * 2;
    int placeholderX = (rect.right / 2) - (boxW / 2);

    // === RENDER BAKLJI ===
    float torchScale = 1.3f;

    int torchW = (int)(gGame.torchInfo.width  * torchScale);
    int torchH = (int)(gGame.torchInfo.height * torchScale);

    int torchX1 = gGame.leftWall.width/2 - torchW/2;
    int torchX2 = 3*gGame.leftWall.width/2 + bgW -  torchW/2;
    int torchY = bgH / 2 - torchH / 2;

    int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;

    // Lijeva baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(
        hdc,
        torchX1, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCPAINT
    );
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(
        hdc,
        torchX1, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCAND
    );
    // Desna baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(
        hdc,
        torchX2, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCPAINT
    );
    SelectObject(gRes.hdcMem, gRes.torch);
   StretchBlt(
        hdc,
        torchX2, torchY,
        torchW, torchH,
        gRes.hdcMem,
        torchSrcX, torchSrcY,
        gGame.torchInfo.width,
        gGame.torchInfo.height,
        SRCAND
    );

    // === HERO (PLAYER 1) ===
    if (gGame.player1Stats.lives > 0) {
        RenderHero(
            hdc,
            &gGame.hero,
            gRes.character,
            gRes.characterMask,
            rect,
            true,
            1.2f
        );
    }

    // === HARPOON ===
    if (gGame.harpoon.isActive) {
        int visible = std::min(gGame.harpoon.length, gGame.harpoon.height);
        int y = rect.bottom - gGame.floorWall.height - visible;

        SelectObject(gRes.hdcMem, gRes.arrowMask);
        StretchBlt(hdc, gGame.harpoon.x, y,
                   gGame.harpoon.width, visible,
                   gRes.hdcMem, 0, 0,
                   gGame.harpoon.width, visible, SRCPAINT);

        SelectObject(gRes.hdcMem, gRes.arrow);
        StretchBlt(hdc, gGame.harpoon.x, y,
                   gGame.harpoon.width, visible,
                   gRes.hdcMem, 0, 0,
                   gGame.harpoon.width, visible, SRCAND);
    }


    // === PLAYER 2 (MULTIPLAYER) ===
   if(gGame.gameState.isMultiplayer && gGame.player2Stats.lives > 0) {
        RenderHero(
            hdc,
            &gGame.hero2,
            gRes.hero2,
            gRes.hero2Mask,
            rect,
            true,
            1.2f
        );

        // Render harpoon2
        if(gGame.harpoon2.isActive) {
            int visible = std::min(gGame.harpoon2.length, gGame.harpoon2.height);
            int y = rect.bottom - gGame.floorWall.height - visible;

            SelectObject(gRes.hdcMem, gRes.arrowMask);
            StretchBlt(hdc, gGame.harpoon2.x, y,
                       gGame.harpoon2.width, visible,
                       gRes.hdcMem, 0, 0,
                       gGame.harpoon2.width, visible, SRCPAINT);

            SelectObject(gRes.hdcMem, gRes.arrow);
            StretchBlt(hdc, gGame.harpoon2.x, y,
                       gGame.harpoon2.width, visible,
                       gRes.hdcMem, 0, 0,
                       gGame.harpoon2.width, visible, SRCAND);
        }
    }

    // === OVERLAY TEXT ===
    // PROMJENA: Level limit je sada 5 (uključuje Level 5)
    if (gGame.gameState.isLevelCleared && gGame.currentLevel < 5)
        DrawLevelPassedScreen(hdc, rect);
    // PROMJENA: Kraj igre nakon levela 6
    else if (gGame.gameState.isGameOver || gGame.currentLevel >= 5 && gGame.gameState.isLevelCleared)
        DrawGameOverScreen(hdc, rect);
    else if(gGame.gameState.currentMode == GAME_MODE_PAUSE ){
        DrawPausedScreen(hdc, rect);
    }

}

void DrawHeartsAndScore(HDC hdc, RECT rect, int padding) {
    int gap = 30; // Razmak između srca

    // ==========================================
    // LIJEVA STRANA - PLAYER 1 (ili Single Player)
    // ==========================================

    int p1Lives = gGame.player1Stats.lives;
    int p1Score = gGame.gameState.isMultiplayer ? gGame.player1Stats.displayScore : gGame.displayScore;
    HeartAnim* p1HeartsAnim = gGame.gameState.isMultiplayer ? gGame.player1Stats.hearts : gGame.hearts;

    int startX1 = gGame.leftWall.width;
    int startY = rect.bottom - gGame.floorWall.height + padding + gGame.heartInfo.height;

    for (int i = 0; i < 5; i++) {
        int x = startX1 + i * (gGame.heartInfo.width + gap);
        int y = startY;

        // === P1 BACKGROUND ===
        HBITMAP oldBmp = (HBITMAP)SelectObject(gRes.hdcMem, gRes.heartBkgMask);
        BitBlt(hdc, x, y, gGame.heartBgInfo.width, gGame.heartBgInfo.height, gRes.hdcMem, 0, 0, SRCPAINT);
        SelectObject(gRes.hdcMem, gRes.heartBkg);
        BitBlt(hdc, x, y, gGame.heartBgInfo.width, gGame.heartBgInfo.height, gRes.hdcMem, 0, 0, SRCAND);

        // === P1 BORDER ===
        SelectObject(gRes.hdcMem, gRes.heartBorderMask);
        BitBlt(hdc, x, y, gGame.heartBorderInfo.width, gGame.heartBorderInfo.height, gRes.hdcMem, 0, 0, SRCPAINT);
        SelectObject(gRes.hdcMem, gRes.heartBorder);
        BitBlt(hdc, x, y, gGame.heartBorderInfo.width, gGame.heartBorderInfo.height, gRes.hdcMem, 0, 0, SRCAND);

        // === P1 FILL ===
        if (i < p1Lives) {
            int srcX = p1HeartsAnim[i].currentFrame * gGame.heartInfo.width;
            SelectObject(gRes.hdcMem, gRes.heartMask);
            BitBlt(hdc, x, y, gGame.heartInfo.width, gGame.heartInfo.height, gRes.hdcMem, srcX, 0, SRCPAINT);
            SelectObject(gRes.hdcMem, gRes.heart);
            BitBlt(hdc, x, y, gGame.heartInfo.width, gGame.heartInfo.height, gRes.hdcMem, srcX, 0, SRCAND);
        }
        SelectObject(gRes.hdcMem, oldBmp); // Vrati stari objekt
    }

    // === SCORE ZA PLAYER 1 ===
    // Layout: [PLAYER 1] [SCORE]
    int scoreGap = 10;
    int scoreX = gGame.playerHolderInfo.x + gGame.playerHolderInfo.width + scoreGap;
    int scoreY = gGame.playerHolderInfo.y + 5;

    // Dimenzije score boxa
    int boxW = 100; // Fiksna minimalna širina za urednost
    int scoreBoxH = gGame.playerHolderInfo.height - 8;

    // Iscrtaj holder
    SelectObject(gRes.hdcMem, gRes.scoreHolder);
    SetStretchBltMode(hdc, COLORONCOLOR);
    StretchBlt(hdc, scoreX, scoreY, boxW, scoreBoxH, gRes.hdcMem, 0, 0, gGame.scoreHolderInfo.width, gGame.scoreHolderInfo.height, SRCCOPY);

    // Okvir
    HPEN hDarkPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hDarkPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RoundRect(hdc, scoreX - 2, scoreY - 2, scoreX + boxW + 2, scoreY + scoreBoxH + 2, 4, 4);
    SelectObject(hdc, hOldPen);
    DeleteObject(hDarkPen);

    // Ispis broja
    HFONT oldFont = (HFONT)SelectObject(hdc, gRes.hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(70, 70, 70));
    char valueText[32];
    sprintf(valueText, "%d", p1Score);
    SIZE textSize;
    GetTextExtentPoint32(hdc, valueText, lstrlen(valueText), &textSize);
    TextOut(hdc, scoreX + 10, scoreY + scoreBoxH/2 - textSize.cy/2, valueText, strlen(valueText));


    // ==========================================
    // DESNA STRANA - PLAYER 2 (Samo Multiplayer)
    // ==========================================
    if (gGame.gameState.isMultiplayer) {
        int p2Lives = gGame.player2Stats.lives;
        int p2Score = gGame.player2Stats.displayScore;
        HeartAnim* p2HeartsAnim = gGame.player2Stats.hearts;

        // Srca počinju od desnog zida
        int startX2 = rect.right - gGame.rightWall.width - gGame.heartBgInfo.width;

        for (int i = 0; i < 5; i++) {
            int x = startX2 - i * (gGame.heartInfo.width + gap);
            int y = startY;

            // === P2 BACKGROUND ===
            HBITMAP oldBmp2 = (HBITMAP)SelectObject(gRes.hdcMem, gRes.heartBkgMask);
            BitBlt(hdc, x, y, gGame.heartBgInfo.width, gGame.heartBgInfo.height, gRes.hdcMem, 0, 0, SRCPAINT);
            SelectObject(gRes.hdcMem, gRes.heartBkg);
            BitBlt(hdc, x, y, gGame.heartBgInfo.width, gGame.heartBgInfo.height, gRes.hdcMem, 0, 0, SRCAND);

            // === P2 BORDER ===
            SelectObject(gRes.hdcMem, gRes.heartBorderMask);
            BitBlt(hdc, x, y, gGame.heartBorderInfo.width, gGame.heartBorderInfo.height, gRes.hdcMem, 0, 0, SRCPAINT);
            SelectObject(gRes.hdcMem, gRes.heartBorder);
            BitBlt(hdc, x, y, gGame.heartBorderInfo.width, gGame.heartBorderInfo.height, gRes.hdcMem, 0, 0, SRCAND);

            // === P2 FILL ===
            if (i < p2Lives) {
                int srcX = p2HeartsAnim[i].currentFrame * gGame.heartInfo.width;
                SelectObject(gRes.hdcMem, gRes.heartMask);
                BitBlt(hdc, x, y, gGame.heartInfo.width, gGame.heartInfo.height, gRes.hdcMem, srcX, 0, SRCPAINT);
                SelectObject(gRes.hdcMem, gRes.heart);
                BitBlt(hdc, x, y, gGame.heartInfo.width, gGame.heartInfo.height, gRes.hdcMem, srcX, 0, SRCAND);
            }
            SelectObject(gRes.hdcMem, oldBmp2);
        }

        // === P2 SCORE (Uz Player 2 ime) ===
        // Layout: [SCORE] [PLAYER 2]

        // 1. Izračunaj gdje počinje "PLAYER 2" okvir (da budemo simetrični s P1)
        char p2NameText[] = "PLAYER 2";
        SIZE p2NameSize;
        GetTextExtentPoint32(hdc, p2NameText, strlen(p2NameText), &p2NameSize);
        int p2NameWidth = p2NameSize.cx + 30; // 30 je padding iz RenderStaticUI

        // X koordinata gdje počinje "PLAYER 2" okvir (desno poravnato uz zid)
        int p2NameX = rect.right - gGame.rightWall.width - p2NameWidth;

        // 2. Postavi Score Box LIJEVO od imena
        int p2ScoreX = p2NameX - boxW - scoreGap;

        // Iscrtaj holder za P2
        SelectObject(gRes.hdcMem, gRes.scoreHolder);
        StretchBlt(hdc, p2ScoreX, scoreY, boxW, scoreBoxH, gRes.hdcMem, 0, 0, gGame.scoreHolderInfo.width, gGame.scoreHolderInfo.height, SRCCOPY);

        // Okvir za P2
        HPEN hDarkPen2 = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
        HPEN hOldPen2 = (HPEN)SelectObject(hdc, hDarkPen2);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        RoundRect(hdc, p2ScoreX - 2, scoreY - 2, p2ScoreX + boxW + 2, scoreY + scoreBoxH + 2, 4, 4);
        SelectObject(hdc, hOldPen2);
        DeleteObject(hDarkPen2);

        // Tekst za P2
        sprintf(valueText, "%d", p2Score);
        GetTextExtentPoint32(hdc, valueText, lstrlen(valueText), &textSize);
        TextOut(hdc, p2ScoreX + 10, scoreY + scoreBoxH/2 - textSize.cy/2, valueText, strlen(valueText));
    }

    SelectObject(hdc, oldFont);
}

void DrawPlayerPlaceholder(
    HDC hdc,
    RECT rect,
    const std::string& text,
    int x,
    int y,
    COLORREF textColor,
    int boxHeight
) {
    HFONT oldFont = (HFONT)SelectObject(hdc, gRes.hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, textColor);

    SIZE textSize;
    GetTextExtentPoint32(
        hdc,
        text.c_str(),
        text.length(),
        &textSize
    );

    int paddingX = 30;
    int boxW = textSize.cx + paddingX;
    int boxH = boxHeight + 10;

    gGame.playerHolderInfo.x = x;
    gGame.playerHolderInfo.y = y;
    gGame.playerHolderInfo.width = boxW;
    gGame.playerHolderInfo.height = boxH;

    SelectObject(gRes.hdcMem, gRes.levelPlaceholderWhite);
    SetStretchBltMode(hdc, COLORONCOLOR);
    StretchBlt(
        hdc,
        x, y,
        boxW, boxH,
        gRes.hdcMem,
        0, 0,
        gGame.levelPlaceholderInfo.width,
        gGame.levelPlaceholderInfo.height,
        SRCAND
    );

    SelectObject(gRes.hdcMem, gRes.levelPlaceholderBlack);
    StretchBlt(
        hdc,
        x, y,
        boxW, boxH,
        gRes.hdcMem,
        0, 0,
        gGame.levelPlaceholderInfo.width,
        gGame.levelPlaceholderInfo.height,
        SRCPAINT
    );

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);

    int textX = x + (boxW - textSize.cx) / 2;
    int textY = y + (boxH - tm.tmHeight) / 2;

    TextOut(
        hdc,
        textX,
        textY,
        text.c_str(),
        text.length()
    );

    SelectObject(hdc, oldFont);
}
// ============ UI HELPER FUNCTIONS ============

// Moved from gameOver.cpp
void DrawButton(HDC hdc, HBITMAP bmp, HBITMAP mask, Button& button)
{
    // maska
    SelectObject(gRes.hdcMem, mask);
    BitBlt(hdc, button.x, button.y, button.width, button.height, gRes.hdcMem, 0, 0, SRCPAINT);

    // bitmap
    SelectObject(gRes.hdcMem, bmp);
    BitBlt(hdc, button.x, button.y, button.width, button.height, gRes.hdcMem, 0, 0, SRCAND);

    // SOUND OFF
    if (bmp == gRes.soundButton && !gGame.settingsState.soundState.soundEffectsOn)
    {
        HRGN rgn = CreateEllipticRgn(
            button.x,
            button.y,
            button.x + button.width,
            button.y + button.height
        );

        SelectClipRgn(hdc, rgn);

        BLENDFUNCTION bf{};
        bf.BlendOp = AC_SRC_OVER;
        bf.SourceConstantAlpha = 80;

        HDC overlayDC = CreateCompatibleDC(hdc);
        HBITMAP overlayBmp = CreateCompatibleBitmap(hdc, button.width, button.height);
        HBITMAP oldBmp = (HBITMAP)SelectObject(overlayDC, overlayBmp);

        RECT r{ 0, 0, button.width, button.height };
        FillRect(overlayDC, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));

        AlphaBlend(
            hdc,
            button.x, button.y,
            button.width, button.height,
            overlayDC,
            0, 0,
            button.width, button.height,
            bf
        );

        SelectObject(overlayDC, oldBmp);
        SelectClipRgn(hdc, NULL);
        DeleteObject(rgn);
        DeleteObject(overlayBmp);
        DeleteDC(overlayDC);
    }

    // HOVER
    if (button.isHover)
    {
        HRGN rgn = CreateEllipticRgn(
            button.x,
            button.y,
            button.x + button.width,
            button.y + button.height
        );

        SelectClipRgn(hdc, rgn);

        HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
        BLENDFUNCTION bf{};
        bf.BlendOp = AC_SRC_OVER;
        bf.SourceConstantAlpha = 60;

        HDC overlayDC = CreateCompatibleDC(hdc);
        HBITMAP overlayBmp = CreateCompatibleBitmap(hdc, button.width, button.height);
        HBITMAP oldBmp = (HBITMAP)SelectObject(overlayDC, overlayBmp);

        RECT r = { 0, 0, button.width, button.height };
        FillRect(overlayDC, &r, brush);

        AlphaBlend(
            hdc,
            button.x, button.y,
            button.width, button.height,
            overlayDC,
            0, 0,
            button.width, button.height,
            bf
        );

        SelectObject(overlayDC, oldBmp);
        SelectClipRgn(hdc, NULL);
        DeleteObject(rgn);
        DeleteObject(brush);
        DeleteObject(overlayBmp);
        DeleteDC(overlayDC);
    }
}


