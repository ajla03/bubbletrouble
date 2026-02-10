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
};

void InitializeLoading(HWND hwnd);
void UpdateLoading();
void RenderLoading(HDC hdc, RECT rect);
bool IsLoadingComplete();
