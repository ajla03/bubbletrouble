#pragma once
#include <windows.h>

struct LoadingBubble {
    float x, y;
    float size;
    float bouncePhase;
    COLORREF color;
};

struct LoadingState {
    LoadingBubble bubbles[4];
    int progress;
    DWORD startTime;
    bool isComplete;

    float ballX, ballY;
    float ballVy;
    bool ballIsFalling;
    bool ballPopped;

    float harpoonHeight;
    bool harpoonActive;
    float groundLevel;
};

void InitializeLoading(HWND hwnd);
void UpdateLoading();
void RenderLoading(HDC hdc, RECT rect);
bool IsLoadingComplete();
