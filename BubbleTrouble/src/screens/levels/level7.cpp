#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <stdio.h>

// ============================================================
//  RENDER
// ============================================================
void RenderLevel7(HDC hdcBuffer, RECT rect)
{
    // === POZADINA ===
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

    // === HORIZONTALNA PLATFORMA (cuvamo u door fieldu) ===
    if (CURRENT_LEVEL.door.width > 0) {
        SelectObject(gRes.hdcMem, gRes.longWall);
        SetStretchBltMode(hdcBuffer, HALFTONE);
        SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
        StretchBlt(
            hdcBuffer,
            CURRENT_LEVEL.door.x,  CURRENT_LEVEL.door.y,
            CURRENT_LEVEL.door.width, CURRENT_LEVEL.door.height,
            gRes.hdcMem, 0, 0,
            CURRENT_LEVEL.door.originalWidth,
            CURRENT_LEVEL.door.originalHeight,
            SRCCOPY
        );
    }

    // === VERTIKALNI STUB 1 (longWall — lijevi cetvrt) ===
    if (CURRENT_LEVEL.longWall.width > 0) {
        SelectObject(gRes.hdcMem, gRes.longWall);
        SetStretchBltMode(hdcBuffer, HALFTONE);
        SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
        StretchBlt(
            hdcBuffer,
            CURRENT_LEVEL.longWall.x, CURRENT_LEVEL.longWall.y,
            CURRENT_LEVEL.longWall.width, CURRENT_LEVEL.longWall.height,
            gRes.hdcMem, 0, 0,
            CURRENT_LEVEL.longWall.originalWidth,
            CURRENT_LEVEL.longWall.originalHeight,
            SRCCOPY
        );
    }

    // === VERTIKALNI STUB 2 (pillar1 — centar) ===
    if (CURRENT_LEVEL.pillar1.width > 0) {
        SelectObject(gRes.hdcMem, gRes.longWall);
        SetStretchBltMode(hdcBuffer, HALFTONE);
        SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
        StretchBlt(
            hdcBuffer,
            CURRENT_LEVEL.pillar1.x, CURRENT_LEVEL.pillar1.y,
            CURRENT_LEVEL.pillar1.width, CURRENT_LEVEL.pillar1.height,
            gRes.hdcMem, 0, 0,
            CURRENT_LEVEL.pillar1.originalWidth,
            CURRENT_LEVEL.pillar1.originalHeight,
            SRCCOPY
        );
    }

    // === VERTIKALNI STUB 3 (pillar2 — desni cetvrt) ===
    if (CURRENT_LEVEL.pillar2.width > 0) {
        SelectObject(gRes.hdcMem, gRes.longWall);
        SetStretchBltMode(hdcBuffer, HALFTONE);
        SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
        StretchBlt(
            hdcBuffer,
            CURRENT_LEVEL.pillar2.x, CURRENT_LEVEL.pillar2.y,
            CURRENT_LEVEL.pillar2.width, CURRENT_LEVEL.pillar2.height,
            gRes.hdcMem, 0, 0,
            CURRENT_LEVEL.pillar2.originalWidth,
            CURRENT_LEVEL.pillar2.originalHeight,
            SRCCOPY
        );
    }

    // === MERDEVINE (desna strana, TransparentBlt - bijela pozadina) ===
    if (CURRENT_LEVEL.ladder.width > 0 && gRes.ladder) {
        HDC hdcLadder = CreateCompatibleDC(hdcBuffer);
        HBITMAP hOld  = (HBITMAP)SelectObject(hdcLadder, gRes.ladder);

        TransparentBlt(
            hdcBuffer,
            CURRENT_LEVEL.ladder.x, CURRENT_LEVEL.ladder.y,
            CURRENT_LEVEL.ladder.width, CURRENT_LEVEL.ladder.height,
            hdcLadder,
            0, 0, CURRENT_LEVEL.ladderOrigW, CURRENT_LEVEL.ladderOrigH,
            RGB(255, 255, 255)
        );

        SelectObject(hdcLadder, hOld);
        DeleteDC(hdcLadder);
    }

    // === BALONI ===
    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (CURRENT_LEVEL.balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &CURRENT_LEVEL.balloons[i]);
        }
    }

    DrawPowerups(hdcBuffer, rect);
}

// ============================================================
//  RECALCULATE (poziva se i kod resize prozora)
// ============================================================
void RecalculateLevel7Layout(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    int bgX  = gGame.leftWall.width;
    int bgW  = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH  = rect.bottom - gGame.floorWall.height;

    // ---- HORIZONTALNA PLATFORMA ----
    // Postavlja se na ~42% visine igraceg prostora
    int platformH = 18;
    int platformY = (int)(bgH * 0.42f);

    BITMAP bmpWall;
    GetObject(gRes.longWall, sizeof(BITMAP), &bmpWall);

    CURRENT_LEVEL.door.x              = bgX;
    CURRENT_LEVEL.door.y              = platformY;
    CURRENT_LEVEL.door.width          = bgW;
    CURRENT_LEVEL.door.height         = platformH;
    CURRENT_LEVEL.door.originalWidth  = bmpWall.bmWidth;
    CURRENT_LEVEL.door.originalHeight = bmpWall.bmHeight;
    CURRENT_LEVEL.door.active         = true;  // omogucava koliziju balona

    // ---- VERTIKALNI STUBOVI (gornji sprat, od vrha do platforme) ----
    int pillarW = bmpWall.bmWidth;   // prirodna sirina bitmape
    int upperH  = platformY;         // visina gornjeg sprata

    // Stub 1: na 1/4 sirine
    CURRENT_LEVEL.longWall.x              = bgX + bgW / 4 - pillarW / 2;
    CURRENT_LEVEL.longWall.y              = 0;
    CURRENT_LEVEL.longWall.width          = pillarW;
    CURRENT_LEVEL.longWall.height         = upperH;
    CURRENT_LEVEL.longWall.originalWidth  = bmpWall.bmWidth;
    CURRENT_LEVEL.longWall.originalHeight = bmpWall.bmHeight;

    // Stub 2: na 2/4 (centar)
    CURRENT_LEVEL.pillar1.x              = bgX + bgW / 2 - pillarW / 2;
    CURRENT_LEVEL.pillar1.y              = 0;
    CURRENT_LEVEL.pillar1.width          = pillarW;
    CURRENT_LEVEL.pillar1.height         = upperH;
    CURRENT_LEVEL.pillar1.originalWidth  = bmpWall.bmWidth;
    CURRENT_LEVEL.pillar1.originalHeight = bmpWall.bmHeight;

    // Stub 3: na 3/4 sirine
    CURRENT_LEVEL.pillar2.x              = bgX + 3 * bgW / 4 - pillarW / 2;
    CURRENT_LEVEL.pillar2.y              = 0;
    CURRENT_LEVEL.pillar2.width          = pillarW;
    CURRENT_LEVEL.pillar2.height         = upperH;
    CURRENT_LEVEL.pillar2.originalWidth  = bmpWall.bmWidth;
    CURRENT_LEVEL.pillar2.originalHeight = bmpWall.bmHeight;

    // ---- MERDEVINE (desna strana, od platforme do poda) ----
    if (gRes.ladder) {
        int ladderDisplayW = 52;
        int ladderDisplayH = bgH - platformY - platformH;

        CURRENT_LEVEL.ladder.x      = rect.right - gGame.rightWall.width - ladderDisplayW - 4;
        CURRENT_LEVEL.ladder.y      = platformY + platformH;
        CURRENT_LEVEL.ladder.width  = ladderDisplayW;
        CURRENT_LEVEL.ladder.height = ladderDisplayH;
    }
}

// ============================================================
//  INIT
// ============================================================
void InitLevel7(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    // === POZADINA INFO ===
    if (gRes.hBgLevel2) {
        BITMAP bm;
        GetObject(gRes.hBgLevel2, sizeof(BITMAP), &bm);
        CURRENT_LEVEL.backgroundInfo.width  = bm.bmWidth;
        CURRENT_LEVEL.backgroundInfo.height = bm.bmHeight;
    }

    // === UCITAJ ORIGINALNE DIMENZIJE MERDEVINA ===
    if (gRes.ladder) {
        BITMAP bmpLadder;
        GetObject(gRes.ladder, sizeof(BITMAP), &bmpLadder);
        CURRENT_LEVEL.ladderOrigW = bmpLadder.bmWidth;
        CURRENT_LEVEL.ladderOrigH = bmpLadder.bmHeight;
    }

    // === POSTAVI PLATFORMU, STUBOVE I MERDEVINE ===
    RecalculateLevel7Layout(hwnd);

    // === OCISTI SVE BALONE ===
    for (int i = 0; i < MAX_BALLOONS; i++)
        CURRENT_LEVEL.balloons[i].active = false;

    int bgX = gGame.leftWall.width;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    int platformY = CURRENT_LEVEL.door.y;
    int platformH = CURRENT_LEVEL.door.height;

    // -------------------------------------------------------
    // GORNJI SPRAT: 4 odjeljka (3 stuba = 4 komore)
    // Svaki odjeljak ima jedan mali balon (radius 18 — jednim
    // pogotkom puca, ne dijeli se jer je 18/2 = 9 < MIN_RADIUS)
    // -------------------------------------------------------
    int sectionW   = bgW / 4;
    float upperMid = (float)platformY * 0.35f;  // Y spustanja u gornji dio

    // Odjeljak 1 (lijevi kraj)
    InitBalloon(0,
        bgX + sectionW * 0 + sectionW / 2.0f,
        upperMid, 18.0f, 1.8f, RGB(255, 255, 0));   // zuti

    // Odjeljak 2
    InitBalloon(1,
        bgX + sectionW * 1 + sectionW / 2.0f,
        upperMid * 0.8f, 18.0f, -1.8f, RGB(0, 210, 0));  // zeleni

    // Odjeljak 3
    InitBalloon(2,
        bgX + sectionW * 2 + sectionW / 2.0f,
        upperMid, 18.0f, 1.8f, RGB(255, 255, 0));   // zuti

    // Odjeljak 4 (desni kraj)
    InitBalloon(3,
        bgX + sectionW * 3 + sectionW / 2.0f,
        upperMid * 0.8f, 18.0f, -1.8f, RGB(0, 210, 0));  // zeleni

    // -------------------------------------------------------
    // DONJI SPRAT: jedan ogromni balon u centru
    // radius 65 → dijeli se na 32.5 → 16.25 → pop (3 nivoa)
    // Platforma djeluje kao plafon, max 4 aktivna u isto vrijeme
    // -------------------------------------------------------
    float lowerCenterX = bgX + bgW / 2.0f;
    float lowerCenterY = (float)(platformY + platformH)
                       + (bgH - platformY - platformH) * 0.40f;

    InitBalloon(4, lowerCenterX, lowerCenterY,
                65.0f, 2.2f, RGB(180, 0, 220));   // ljubicast

    // === HERO POCETNA POZICIJA — gornji sprat ===
    {
        int upperFloorY = CURRENT_LEVEL.door.y - gGame.hero.height;
        gGame.hero.x      = gGame.leftWall.width + 50;  // npr. isto kao u ostalim levelima
        gGame.hero.y      = upperFloorY;
        gGame.hero.floorY = upperFloorY;
        gGame.hero.isOnLadder = false;
    }

    // === POWERUP SISTEM ===
    InitPowerupSystemForLevel();

    // === MULTIPLAYER — pocetne pozicije ===
    if (gGame.gameState.isMultiplayer) {
        gGame.hero.x  = gGame.leftWall.width + 50;
        gGame.hero2.x = gGame.leftWall.width + 130;

        RECT r2;
        GetClientRect(hwnd, &r2);
        gGame.hero2.y = r2.bottom - gGame.floorWall.height - gGame.hero2.height;
    }
}
