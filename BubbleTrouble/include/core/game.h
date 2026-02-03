#pragma once
#include <windows.h>
#include "types.h"
#include <string>
// ================= SYSTEM =================
void LoadCustomFont();
void LoadBitmaps(HWND hwnd, HINSTANCE hInstance);
void RefreshSound();
// ================= INPUT ==================
void CheckInputs(HWND hwnd);
void CheckHeroDoorCollision();
void CheckHeroPillarCollision(StaticObject* pillar);
// ================= GAME LOOP ==============
void Update(HWND hwnd);
void RefreshScreen(HWND hwnd);

// ================= PHYSICS / HELPERS ======
float GetBounceSpeedForRadius(float radius);

// ================= BALLOONS ===============
void InitBalloon(int index, float x, float y, float radius, float speedX, COLORREF color);
void UpdateBalloons(HWND hwnd);
void SplitBalloon(int index);
void DrawBalloonGDI(HDC hdc, Balloon* b);

// ================= COLLISIONS =============
void CheckCollisions();

// ================= MENU ===================
void InitializeMenu(HWND hwnd);
void RenderMenu(HDC hdc, RECT rect);
void HandleMenuClick(HWND hwnd, int x, int y);
void HandleMenuMouseMove(HWND hwnd, int x, int y);

// ================= GAME STATE =============
void StartGame(HWND hwnd);
void ResetGame(HWND hwnd);
void ResetBetweenLevels(HWND hwnd);

// ================= UI / SCREENS ===========
void DrawGameOverScreen(HDC hdc, RECT rect);
void DrawLevelPassedScreen(HDC hdc, RECT rect);
void DrawHeartsAndScore(HDC hdc, RECT rect, int barHeight);
void DrawPlayerPlaceholder(
    HDC hdc,
    RECT rect,
    const std::string& text,
    int x,
    int y,
    COLORREF textColor,
    int boxHeight
);


void DrawButton(HDC hdc, HBITMAP bmp, HBITMAP mask, Button& info);
void CheckHover(Button& button, int x, int y);

void InitLevel(HWND hwnd);
void InitLevel2(HWND hwnd);
void InitLevel3(HWND hwnd);
void RecalculateLevel3Layout(HWND);
void RenderLevel1(HDC hdcBuffer, RECT rect);
void RenderLevel2(HDC hdcBuffer, RECT rect);
void RenderLevel3(HDC hdcBuffer, RECT rect);
void RenderDynamicGameUI(HDC hdcBuffer, RECT rect);
void RenderStaticUI(HDC hdc, RECT rect);
void RenderLevel(HDC, RECT);
void RenderWalls(HDC, RECT);
void RenderLevel4(HDC hdcBuffer, RECT rect);
void InitLevel4(HWND hwnd);
void RecalculateLevel4Layout(HWND hwnd);
void RenderLevel5(HDC hdcBuffer, RECT rect);
void InitLevel5(HWND hwnd);
void RenderLevel6(HDC hdcBuffer, RECT rect);
void InitLevel6(HWND hwnd);

// ================= TRANSITIONS ============
void StartWallTransition(HWND hwnd);
void UpdateWallTransition(HWND hwnd);
void DrawTransitionWalls(HDC hdc, RECT rect);

//================== UPDATE =================
void UpdateLayout(int oldW, int oldH, int newW, int newH);
