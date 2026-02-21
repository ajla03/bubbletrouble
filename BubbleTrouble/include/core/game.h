#pragma once
#include <windows.h>
#include "types.h"
#include <string>

void LoadCustomFont();
void LoadBitmaps(HWND hwnd, HINSTANCE hInstance);

void DrawButton(HDC hdc, HBITMAP bmp, HBITMAP mask, Button& info);
bool IsPointInButton(const Button& btn, int x, int y);
void CheckHover(Button& button, int x, int y);
void DrawPlayerPlaceholder(HDC hdc, RECT rect, const std::string& text,
                          int x, int y, COLORREF textColor, int boxHeight);

bool AABB(float ax, float ay, float aw, float ah,
          float bx, float by, float bw, float bh);
void CheckHeroDoorCollision();
void CheckHeroPillarCollision(StaticObject* pillar);

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

void StartGame(HWND hwnd);
void ResetGame(HWND hwnd);
void ResetBetweenLevels(HWND hwnd);

void Update(HWND hwnd);
void RefreshScreen(HWND hwnd);

float GetBounceSpeedForRadius(float radius);
void InitBalloon(int index, float x, float y, float radius,
                 float speedX, COLORREF color);
void UpdateBalloons(HWND hwnd);
void SplitBalloon(int index, int scoringPlayer = 0);
bool AreSectionBalloonsDestroyed(float left, float right);
void CheckCollisions();
void ResolveBalloonPillarCollision(Balloon* b, StaticObject* wall);
void DrawBalloonGDI(HDC hdc, Balloon* b) ;

void InitPowerupSystemForLevel();
void SpawnPowerup(HWND hwnd);
void UpdatePowerups(HWND hwnd);
void ApplyPowerup(PowerUpType type, int playerIndex);
void DrawPowerups(HDC hdc, RECT rect);
void FreezeBalloons();
void UnfreezeBalloons();

void InitializeMenu(HWND hwnd);
void RenderMenu(HDC hdc, RECT rect);

void DrawGameOverScreen(HDC hdc, RECT rect);
void DrawPausedScreen(HDC hdc, RECT rect);
void DrawLevelPassedScreen(HDC hdc, RECT rect);
void RenderSettings(HDC, RECT);
void RenderHelp(HDC hdc, RECT rect);

void DrawHeartsAndScore(HDC hdc, RECT rect, int barHeight);
void RenderStaticUI(HDC hdc, RECT rect);
void RenderDynamicGameUI(HDC hdcBuffer, RECT rect);
void RenderWalls(HDC, RECT);

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
void InitLevel7(HWND hwnd);
void RenderLevel7(HDC hdcBuffer, RECT rect);
void RecalculateLevel7Layout(HWND hwnd);
void InitLevel8(HWND hwnd);
void RenderLevel8(HDC hdcBuffer, RECT rect);
void RecalculateLevel8Layout(HWND hwnd);
void UpdateLevelSelectHover(HWND hwnd, int mx, int my);

void StartWallTransition(HWND hwnd);
void UpdateWallTransition(HWND hwnd);
void DrawTransitionWalls(HDC hdc, RECT rect);

void UpdateLayout(int oldW, int oldH, int newW, int newH);

void InitDefaultSettings();
void HandleKeyDown(HWND hwnd, WPARAM wParam);
const char* VKCodeToString(int vkCode);

void InitMultiplayer(HWND hwnd);
void UpdatePlayer2Input(HWND hwnd);
void UpdateHarpoon2(HWND hwnd);
void CheckHeroDoorCollisionP2();
void CheckHeroPillarCollision(Hero* h, StaticObject* pillar);
void UpdateHeroCoolDownP2(float dt);

void RenderHero(
    HDC hdc,
    Hero* hero,
    HBITMAP heroBitmap,
    HBITMAP heroMask,
    RECT rect,
    bool useStretch,
    float scale
);

void RenderDashboard(HDC, RECT);


void InitializeLoading(HWND hwnd);
void UpdateLoading();
void RenderLoading(HDC hdc, RECT rect);
bool IsLoadingComplete();


void RenderLoginScreen(HDC hdcBuffer, RECT rect, HWND hwnd);
void InitLoginInput();
void HandleCharInput(HWND hwnd, WPARAM wParam);

void SaveFinalScore();
void ResetScoreFlag();
int GetBestScoreForPlayer(const char* playerName);
void RenderLevelSelectScreen(HDC hdc, RECT rect);

void CreateFonts();

