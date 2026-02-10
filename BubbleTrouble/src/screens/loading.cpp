#include "loading.h"
#include "gameContext.h"
#include "resourceManager.h"
#include<stdio.h>
#include <cmath>

LoadingState gLoading = {0};

void InitializeLoading(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    int GROUND_Y = rect.bottom - 100;

    // Inicijalizuj balončiće
    gLoading.bubbles[0] = { rect.right * 0.2f, (float)GROUND_Y, 120, 0.0f, RGB(255, 71, 87) };
    gLoading.bubbles[1] = { rect.right * 0.4f, (float)GROUND_Y, 90, 0.3f, RGB(255, 165, 2) };
    gLoading.bubbles[2] = { rect.right * 0.6f, (float)GROUND_Y, 110, 0.6f, RGB(30, 144, 255) };
    gLoading.bubbles[3] = { rect.right * 0.8f, (float)GROUND_Y, 85, 0.9f, RGB(46, 213, 115) };

    gLoading.progress = 0;
    gLoading.startTime = GetTickCount();
    gLoading.isComplete = false;
}

void UpdateLoading() {
    static DWORD lastUpdate = 0;
    DWORD current = GetTickCount();

    // Update progress svakih 50ms
    if (current - lastUpdate >= 50) {
        if (gLoading.progress < 100) {
            gLoading.progress++;
        } else {
            gLoading.isComplete = true;
        }
        lastUpdate = current;
    }
}

void RenderLoading(HDC hdc, RECT rect) {
    // Pozadina
    HBRUSH bgBrush = CreateSolidBrush(RGB(26, 26, 46));
    FillRect(hdc, &rect, bgBrush);
    DeleteObject(bgBrush);

    // "Tlo"
    HBRUSH groundBrush = CreateSolidBrush(RGB(255, 71, 87));
    RECT groundRect = { 0, rect.bottom - 100, rect.right, rect.bottom - 92 };
    FillRect(hdc, &groundRect, groundBrush);
    DeleteObject(groundBrush);

    // Balončići sa bounce animacijom
    static float time = 0;
    time += 0.05f;

    for (int i = 0; i < 4; i++) {
        LoadingBubble* b = &gLoading.bubbles[i];

        // Bounce animacija
        float phase = time * 3.14159f + b->bouncePhase * 3.14159f * 2;
        float bounce = sin(phase) * sin(phase);
        float currentY = b->y - bounce * 150;

        // Crtaj balončić
        int centerX = (int)b->x;
        int centerY = (int)currentY;
        int radius = (int)b->size / 2;

        // Gradijent efekat
        for (int r = radius; r > 0; r--) {
            float factor = (float)r / radius;
            int newR = GetRValue(b->color) + (int)((255 - GetRValue(b->color)) * (1.0f - factor) * 0.4f);
            int newG = GetGValue(b->color) + (int)((255 - GetGValue(b->color)) * (1.0f - factor) * 0.4f);
            int newB = GetBValue(b->color) + (int)((255 - GetBValue(b->color)) * (1.0f - factor) * 0.4f);

            HBRUSH brush = CreateSolidBrush(RGB(newR, newG, newB));
            HPEN pen = CreatePen(PS_SOLID, 1, RGB(newR, newG, newB));

            SelectObject(hdc, brush);
            SelectObject(hdc, pen);
            Ellipse(hdc, centerX - r, centerY - r, centerX + r, centerY + r);

            DeleteObject(brush);
            DeleteObject(pen);
        }

        // Kontura
        HPEN outlinePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        SelectObject(hdc, outlinePen);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
        DeleteObject(outlinePen);
    }

    // LOADING tekst
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    HFONT hFont = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    const char* text = "LOADING...";
    SIZE sz;
    GetTextExtentPoint32(hdc, text, strlen(text), &sz);
    TextOut(hdc, (rect.right - sz.cx) / 2, 50, text, strlen(text));

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);

    // Progress bar
    int barWidth = 400;
    int barHeight = 40;
    int barX = (rect.right - barWidth) / 2;
    int barY = rect.bottom - 150;

    // Okvir
    HBRUSH borderBrush = CreateSolidBrush(RGB(0, 0, 0));
    RECT borderRect = { barX, barY, barX + barWidth, barY + barHeight };
    FillRect(hdc, &borderRect, borderBrush);
    DeleteObject(borderBrush);

    // Progress
    int progressWidth = (barWidth * gLoading.progress) / 100;
    if (progressWidth > 0) {
        HBRUSH progressBrush = CreateSolidBrush(RGB(46, 213, 115));
        RECT progressRect = { barX, barY, barX + progressWidth, barY + barHeight };
        FillRect(hdc, &progressRect, progressBrush);
        DeleteObject(progressBrush);
    }

    // Border
    HPEN borderPen = CreatePen(PS_SOLID, 4, RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, barX, barY, barX + barWidth, barY + barHeight);
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);

    // Procenat
    char progressText[10];
    sprintf(progressText, "%d%%", gLoading.progress);
    GetTextExtentPoint32(hdc, progressText, strlen(progressText), &sz);
    TextOut(hdc, barX + (barWidth - sz.cx) / 2, barY + (barHeight - sz.cy) / 2,
            progressText, strlen(progressText));
}

bool IsLoadingComplete() {
    return gLoading.isComplete;
}
