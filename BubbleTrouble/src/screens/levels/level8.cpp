#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <stdio.h>

// Stubovi su aktivni na pocetku levela; nestaju cim se prvi balon unisti
static bool s_pillarsActive = true;

// Poziva se iz Update() svaki frame dok je level 8 aktivan
void UpdateLevel8Pillars() {
    if (!s_pillarsActive) return;

    // Level pocinje sa 3 balona (levelScore = 0). Cim score poraste,
    // znaci da je barem jedan balon unisten — ukloni oba stuba odmah.
    if (CURRENT_LEVEL.levelScore > 0) {
        s_pillarsActive = false;
        CURRENT_LEVEL.pillar1.width  = 0;
        CURRENT_LEVEL.pillar1.height = 0;
        CURRENT_LEVEL.pillar2.width  = 0;
        CURRENT_LEVEL.pillar2.height = 0;
    }
}

void RenderLevel8(HDC hdcBuffer, RECT rect){
    SelectObject(gRes.hdcMem, gRes.hBgLevel2);

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    SetStretchBltMode(hdcBuffer, HALFTONE);
    SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
    StretchBlt(
        hdcBuffer, bgX, bgY, bgW, bgH,
        gRes.hdcMem, 0, 0,
        CURRENT_LEVEL.backgroundInfo.width,
        CURRENT_LEVEL.backgroundInfo.height,
        SRCCOPY
    );


    if(CURRENT_LEVEL.pillar1.width > 0){
        SelectObject(gRes.hdcMem, gRes.longWall);
        SetStretchBltMode(hdcBuffer, HALFTONE);
        SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
        StretchBlt(
            hdcBuffer,
            CURRENT_LEVEL.pillar1.x,
            CURRENT_LEVEL.pillar1.y,
            CURRENT_LEVEL.pillar1.width,
            CURRENT_LEVEL.pillar1.height,
            gRes.hdcMem,
            0, 0,
            CURRENT_LEVEL.pillar1.originalWidth,
            CURRENT_LEVEL.pillar1.originalHeight,
            SRCCOPY
        );
    }

    // === SECOND PILLAR (RIGHT) ===
    if(CURRENT_LEVEL.pillar2.width > 0){
        SelectObject(gRes.hdcMem, gRes.longWall);
        SetStretchBltMode(hdcBuffer, HALFTONE);
        SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
        StretchBlt(
            hdcBuffer,
            CURRENT_LEVEL.pillar2.x,
            CURRENT_LEVEL.pillar2.y,
            CURRENT_LEVEL.pillar2.width,
            CURRENT_LEVEL.pillar2.height,
            gRes.hdcMem,
            0, 0,
            CURRENT_LEVEL.pillar2.originalWidth,
            CURRENT_LEVEL.pillar2.originalHeight,
            SRCCOPY
        );
    }

    // === BALLOONS ===
    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (CURRENT_LEVEL.balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &CURRENT_LEVEL.balloons[i]);
        }
    }

    DrawPowerups(hdcBuffer, rect);
}

void InitLevel8(HWND hwnd){
    RECT rect;
    GetClientRect(hwnd, &rect);

    // === LOAD BACKGROUND INFO ===
    if(gRes.hBgLevel2){
        BITMAP bm;
        GetObject(gRes.hBgLevel2, sizeof(BITMAP), &bm);
        CURRENT_LEVEL.backgroundInfo.width = bm.bmWidth;
        CURRENT_LEVEL.backgroundInfo.height = bm.bmHeight;
    }

    // === SETUP PILLARS ===
    BITMAP bmp;
    GetObject(gRes.longWall, sizeof(BITMAP), &bmp);

    // Pillar 1 (Left divider)
    CURRENT_LEVEL.pillar1.originalWidth  = bmp.bmWidth;
    CURRENT_LEVEL.pillar1.originalHeight = bmp.bmHeight;
    CURRENT_LEVEL.pillar1.width          = bmp.bmWidth;
    CURRENT_LEVEL.pillar1.height         = bmp.bmHeight;

    // Pillar 2 (Right divider)
    CURRENT_LEVEL.pillar2.originalWidth  = bmp.bmWidth;
    CURRENT_LEVEL.pillar2.originalHeight = bmp.bmHeight;
    CURRENT_LEVEL.pillar2.width          = bmp.bmWidth;
    CURRENT_LEVEL.pillar2.height         = bmp.bmHeight;

    RecalculateLevel8Layout(hwnd);

    // Reset flag i score za stub-mehaniku
    s_pillarsActive = true;
    CURRENT_LEVEL.levelScore = 0;

    // === CLEAR ALL BALLOONS ===
    for(int i = 0; i < MAX_BALLOONS; i++)
        CURRENT_LEVEL.balloons[i].active = false;

    // === CREATE 3 BALLOONS - ONE PER SECTION ===
    // Layout: [  Section1(25%)  |P1|  Section2(50%)  |P2|  Section3(25%)  ]
    // Pillars are at 25% and 75% so the middle section is wider.

    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;

    // Pillar positions at 25% and 75% of bgW
    int pillar1CenterX = gGame.leftWall.width + bgW / 4;
    int pillar2CenterX = gGame.leftWall.width + 3 * bgW / 4;

    // Section 1 (Left) - Large red balloon
    float section1CenterX = gGame.leftWall.width + (float)(bgW / 4) / 2.0f;
    InitBalloon(0, section1CenterX, 120.0f, 80.0f, -3.0f, RGB(220, 30, 30));   // Red

    // Section 2 (Middle) - Medium yellow balloon
    float section2CenterX = (float)(pillar1CenterX + pillar2CenterX) / 2.0f;
    InitBalloon(1, section2CenterX, 150.0f, 55.0f,  2.5f, RGB(255, 210, 0));   // Yellow

    // Section 3 (Right) - Medium-large orange balloon
    float section3CenterX = pillar2CenterX + (float)(bgW / 4) / 2.0f;
    InitBalloon(2, section3CenterX, 130.0f, 65.0f, -2.8f, RGB(255, 130, 0));   // Orange

    // === SPAWN BOTH PLAYERS IN THE CENTER SECTION ===
    // Player 1 - slightly left of center
    int centerX = rect.right / 2;
    gGame.hero.x = centerX - gGame.hero.width - 20;
    gGame.hero.y = rect.bottom - gGame.floorWall.height - gGame.hero.height;
    gGame.hero.floorY = 0;
    gGame.hero.isOnLadder = false;

    // Player 2 - slightly right of center (only matters in multiplayer, no harm in singleplayer)
    if(gGame.gameState.isMultiplayer) {
        gGame.hero2.x = centerX + 20;
        gGame.hero2.y = rect.bottom - gGame.floorWall.height - gGame.hero2.height;
        gGame.hero2.floorY = 0;
        gGame.hero2.isOnLadder = false;
    }

    InitPowerupSystemForLevel();

}

void RecalculateLevel8Layout(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    // Pillars at 25% and 75% — gives a wide middle section and narrow outer sections
    // === PILLAR 1 (divides section 1 and 2, at 25% of bgW) ===
    CURRENT_LEVEL.pillar1.x      = gGame.leftWall.width + bgW / 4 - CURRENT_LEVEL.pillar1.width / 2;
    CURRENT_LEVEL.pillar1.y      = 0;
    CURRENT_LEVEL.pillar1.height = bgH;

    // === PILLAR 2 (divides section 2 and 3, at 75% of bgW) ===
    CURRENT_LEVEL.pillar2.x      = gGame.leftWall.width + 3 * bgW / 4 - CURRENT_LEVEL.pillar2.width / 2;
    CURRENT_LEVEL.pillar2.y      = 0;
    CURRENT_LEVEL.pillar2.height = bgH;
}
