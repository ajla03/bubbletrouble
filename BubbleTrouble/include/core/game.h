#pragma once
#include <windows.h>
#include "types.h"
#include <string>

// ================= SYSTEM =================
void LoadCustomFont();
void LoadBitmaps(HWND hwnd, HINSTANCE hInstance);
void RefreshSound();

// ================= UI HELPERS (in gameUI.cpp) =================
void DrawButton(HDC hdc, HBITMAP bmp, HBITMAP mask, Button& info);
bool IsPointInButton(const Button& btn, int x, int y);
void CheckHover(Button& button, int x, int y);
void DrawPlayerPlaceholder(HDC hdc, RECT rect, const std::string& text,
                          int x, int y, COLORREF textColor, int boxHeight);

// ================= COLLISION/PHYSICS (in collision.cpp) ==========
bool AABB(float ax, float ay, float aw, float ah,
          float bx, float by, float bw, float bh);
void CheckHeroDoorCollision();
void CheckHeroPillarCollision(StaticObject* pillar);

// ================= INPUT (in input.cpp) ==================
void CheckInputs(HWND hwnd);
void HandleMouseClick(HWND hwnd, int mx, int my);
void HandleMouseMove(HWND hwnd, int x, int y);
void HandleMenuClick(HWND hwnd, int x, int y);
void HandleMenuMouseMove(HWND hwnd, int x, int y);
void HandlePlayingClick(HWND hwnd, int mx, int my);
void HandlePauseClick(HWND hwnd, int mx, int my);
void HandleEndScreenClick(HWND hwnd, int mx, int my);
void HandleBackClick(HWND hwnd, int mx, int my);
void HandleSettingsClick(HWND hwnd, int mx, int my);
void HandleSettingsClick(HWND hwnd, int mx, int my);
void HandleHelpClick(HWND hwnd, int mx, int my);

// ================= GAME STATE (in gameState.cpp) =================
void StartGame(HWND hwnd);
void ResetGame(HWND hwnd);
void ResetBetweenLevels(HWND hwnd);

// ================= GAME LOOP (in update.cpp) ==============
void Update(HWND hwnd);
void RefreshScreen(HWND hwnd);

// ================= BALLOONS (in baloons.cpp) ===============
float GetBounceSpeedForRadius(float radius);
void InitBalloon(int index, float x, float y, float radius,
                 float speedX, COLORREF color);
void UpdateBalloons(HWND hwnd);
void SplitBalloon(int index, int scoringPlayer = 0);
bool AreSectionBalloonsDestroyed(float left, float right);
void CheckCollisions();
void ResolveBalloonPillarCollision(Balloon* b, StaticObject* wall);
void DrawBalloonGDI(HDC hdc, Balloon* b) ;
// ================= POWERUPS (in powerups.cpp) ===============
void InitPowerupSystemForLevel();
void SpawnPowerup(HWND hwnd);
void UpdatePowerups(HWND hwnd);
void ApplyPowerup(PowerUpType type, int playerIndex);
void DrawPowerups(HDC hdc, RECT rect);
void FreezeBalloons();
void UnfreezeBalloons();

// ================= MENU (in menu.cpp) ===================
void InitializeMenu(HWND hwnd);
void RenderMenu(HDC hdc, RECT rect);

// ================= SCREENS ================
void DrawGameOverScreen(HDC hdc, RECT rect);
void DrawPausedScreen(HDC hdc, RECT rect);
void DrawLevelPassedScreen(HDC hdc, RECT rect);
void RenderSettings(HDC, RECT);
void RenderHelp(HDC hdc, RECT rect);

// ================= UI / RENDERING (in gameUI.cpp) =========
void DrawHeartsAndScore(HDC hdc, RECT rect, int barHeight);
void RenderStaticUI(HDC hdc, RECT rect);
void RenderDynamicGameUI(HDC hdcBuffer, RECT rect);
void RenderWalls(HDC, RECT);

// ================= LEVELS =================
void InitLevel(HWND hwnd);
void RenderLevel(HDC, RECT);
void InitLevel1();
void InitLevel2(HWND hwnd);
void InitLevel3(HWND hwnd);
void InitLevel4(HWND hwnd);
void InitLevel5(HWND hwnd);
void InitLevel6(HWND hwnd);
void RenderLevel1(HDC hdcBuffer, RECT rect);
void RenderLevel2(HDC hdcBuffer, RECT rect);
void RenderLevel3(HDC hdcBuffer, RECT rect);
void RenderLevel4(HDC hdcBuffer, RECT rect);
void RenderLevel5(HDC hdcBuffer, RECT rect);
void RenderLevel6(HDC hdcBuffer, RECT rect);
void RecalculateLevel3Layout(HWND);
void RecalculateLevel4Layout(HWND hwnd);

// ================= TRANSITIONS (in animatedWalls.cpp) ============
void StartWallTransition(HWND hwnd);
void UpdateWallTransition(HWND hwnd);
void DrawTransitionWalls(HDC hdc, RECT rect);

// ================= LAYOUT (in update.cpp) =================
void UpdateLayout(int oldW, int oldH, int newW, int newH);

// ================ SETTINGS =======================
void InitDefaultSettings();
void HandleKeyDown(HWND hwnd, WPARAM wParam);
const char* VKCodeToString(int vkCode);
// ================= MULTIPLAYER  =================
void InitMultiplayer(HWND hwnd);
void UpdatePlayer2Input(HWND hwnd);
void UpdateHarpoon2(HWND hwnd);
void CheckHeroDoorCollisionP2();
void CheckHeroPillarCollision2(StaticObject* pillar);
void UpdateHeroCoolDownP2(float dt);

// ====== HEROS
void RenderHero(
    HDC hdc,
    Hero* hero,
    HBITMAP heroBitmap,
    HBITMAP heroMask,
    RECT rect,
    bool useStretch,
    float scale
);

// ============= DASHBOARD ===================
void RenderDashboard(HDC, RECT);



// ================= LOADING (in loading.cpp) ===================
void InitializeLoading(HWND hwnd);
void UpdateLoading();
void RenderLoading(HDC hdc, RECT rect);
bool IsLoadingComplete();
