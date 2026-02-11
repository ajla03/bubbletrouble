#pragma once
#include <windows.h>

struct LoadingBubble {
    float x, y;
    float size;
    float bouncePhase;
    COLORREF color;
};

struct LoadingState {
    // Stari podaci
    LoadingBubble bubbles[4];
    int progress;
    DWORD startTime;
    bool isComplete;

    float ballX, ballY;      // Pozicija balona
    float ballVy;            // Brzina pada
    bool ballIsFalling;      // Da li je pao s ivice?
    bool ballPopped;         // Da li je pogođen?

    float harpoonHeight;     // Visina harpuna
    bool harpoonActive;      // Da li je ispaljen?
};

void InitializeLoading(HWND hwnd);
void UpdateLoading();
void RenderLoading(HDC hdc, RECT rect);
bool IsLoadingComplete();
