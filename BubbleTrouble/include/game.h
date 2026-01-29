#pragma once

// ======== POMOCNE FUNKCIJE ======== //
void LoadCustomFont();
void LoadBitmaps(HWND hwnd, HINSTANCE hInstance);
void CheckInputs(HWND hwnd);
void Update(HWND hwnd);
void RefreshScreen(HWND hwnd);
void RefreshSound();

float GetBounceSpeedForRadius(float radius);
void InitBalloon(int index, float x, float y, float radius, float speedX, COLORREF color);
void UpdateBalloons(HWND hwnd);
void CheckCollisions();
void SplitBalloon(int index);
void DrawBalloonGDI(HDC hdc, Balloon* b);

void InitializeMenu(HWND hwnd);
void RenderMenu(HWND hwnd);
void HandleMenuClick(HWND hwnd, int x, int y);
void HandleMenuMouseMove(HWND hwnd, int x, int y);
void StartGame(HWND hwnd);
void ResetGame(HWND hwnd);

void DrawGameOverScreen(HDC hdc, RECT rect);
void DrawLevelPassedScreen(HDC hdc, RECT rect);
void DrawButton(HDC hdc, HBITMAP bmp, HBITMAP mask, Button& info);
void CheckHover(Button&, int, int);
