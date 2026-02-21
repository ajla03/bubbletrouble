#include "loading.h"
#include "resourceManager.h"
#include "resources.h"
#include <windows.h>
#include <cmath>
#include <cstdio>

#pragma comment(lib, "Msimg32.lib")

LoadingState gLoading = { 0 };

void InitializeLoading(HWND hwnd) {
    if (gRes.background == NULL) {
        HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

    }

    gLoading.progress = 0;
    gLoading.isComplete = false;
    gLoading.startTime = GetTickCount();

    gLoading.ballIsFalling = false;
    gLoading.ballPopped = false;
    gLoading.ballVy = 0;
    gLoading.harpoonActive = false;
    gLoading.harpoonHeight = 0;
}

void UpdateLoading() {
    static DWORD last = 0;
    DWORD now = GetTickCount();

    if (now - last > 30) {
        if (gLoading.progress < 100) {
            gLoading.progress++;
        } else {
            static int finishDelay = 0;
            finishDelay++;
            if (finishDelay > 20) gLoading.isComplete = true;
        }

        if (gLoading.progress > 80 && gLoading.progress <= 90) {
            gLoading.ballIsFalling = true;
            gLoading.ballVy += 2.0f;
            gLoading.ballY += gLoading.ballVy;
        }
        else if (gLoading.progress > 90 && !gLoading.ballPopped) {
            gLoading.harpoonActive = true;
            gLoading.harpoonHeight += 35.0f;

        }
        last = now;
    }
}

void RenderLoading(HDC hdc, RECT rect) {
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBM = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);

    HBRUSH bgBrush = CreateSolidBrush(RGB(35, 35, 40));
    FillRect(memDC, &rect, bgBrush);
    DeleteObject(bgBrush);


    int centerX = rect.right / 2;
    int centerY = rect.bottom / 2;

    int barW;
    if (rect.right <= 1280) {
        barW = 450;
    } else {
        barW = (rect.right * 50) / 100;
        if (barW > 900) barW = 900;
    }

    int barH;
    if (rect.bottom <= 720) {
        barH = 25;
    } else {
        barH = rect.bottom / 50;
        if (barH < 25) barH = 25;
        if (barH > 35) barH = 35;
    }

    int barX = centerX - (barW / 2);
    int barY = centerY;

    int heroW;
    int heroH;
    if (rect.right <= 1280) {
        heroW = 70;
        heroH = 90;
    } else {
        heroW = rect.right / 18;
        if (heroW < 70) heroW = 70;
        if (heroW > 130) heroW = 130;

        heroH = (heroW * 9) / 7;
        if (heroH < 90) heroH = 90;
        if (heroH > 160) heroH = 160;
    }

    int heroY = rect.bottom - heroH;

    int targetX = (int)(barX + (barW * 1.15f)) - (heroW / 2);
    int startX = -heroW;

    float moveFactor = (float)gLoading.progress / 90.0f;
    if (moveFactor > 1.0f) moveFactor = 1.0f;

    int heroX = startX + (int)((targetX - startX) * moveFactor);

    int frameThickness;
    if (rect.bottom <= 720) {
        frameThickness = 4; // Mali ekran
    } else {
        frameThickness = barH / 6; // Fullscreen
        if (frameThickness < 4) frameThickness = 4;
        if (frameThickness > 7) frameThickness = 7;
    }

    HBRUSH outerFrameBrush = CreateSolidBrush(RGB(255, 255, 255));
    RECT outerRect = { barX - frameThickness, barY - frameThickness,
                       barX + barW + frameThickness, barY + barH + frameThickness };
    FillRect(memDC, &outerRect, outerFrameBrush);
    DeleteObject(outerFrameBrush);

    HBRUSH innerFrameBrush = CreateSolidBrush(RGB(30, 30, 30));
    RECT innerRect = { barX, barY, barX + barW, barY + barH };
    FillRect(memDC, &innerRect, innerFrameBrush);
    DeleteObject(innerFrameBrush);

    int fillW = (barW * gLoading.progress) / 100;

    if (fillW > 0) {
        TRIVERTEX vertex[2];
        vertex[0].x = barX;
        vertex[0].y = barY;
        vertex[0].Red = 0x6000;
        vertex[0].Green = 0x1000;
        vertex[0].Blue = 0x1000;
        vertex[0].Alpha = 0x0000;

        vertex[1].x = barX + fillW;
        vertex[1].y = barY + barH;
        vertex[1].Red = 0xFF00;
        vertex[1].Green = 0x3000;
        vertex[1].Blue = 0x3000;
        vertex[1].Alpha = 0x0000;

        GRADIENT_RECT gRect;
        gRect.UpperLeft = 0;
        gRect.LowerRight = 1;

        GradientFill(memDC, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
    }

    if (!gLoading.ballPopped) {
        float bx, by;
        float radius;

        if (rect.bottom <= 720) {
            radius = 16.0f;
        } else {
            radius = (float)(barH * 0.8f);
            if (radius < 16.0f) radius = 16.0f;
            if (radius > 28.0f) radius = 28.0f;
        }

        if (!gLoading.ballIsFalling) {
            float t = (float)gLoading.progress / 80.0f; if (t > 1.0f) t = 1.0f;
            bx = barX + (barW * t * 1.15f);
            by = barY - radius - fabs(sinf(t * 15.0f)) * 50.0f;
            gLoading.ballY = by;
        } else {
            bx = barX + (barW * 1.15f) + radius;
            by = gLoading.ballY;
            if (by > rect.bottom - radius) by = (float)(rect.bottom - radius);
        }
        HBRUSH ballBrush = CreateSolidBrush(RGB(255, 60, 60));
        SelectObject(memDC, ballBrush);
        Ellipse(memDC, (int)(bx - radius), (int)(by - radius), (int)(bx + radius), (int)(by + radius));
        DeleteObject(ballBrush);
    } else {
        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, RGB(255, 255, 0));

        int fontSize;
        if (rect.bottom <= 720) {
            fontSize = 40; // Mali ekran
        } else {
            fontSize = rect.bottom / 18; // Fullscreen
            if (fontSize < 40) fontSize = 40;
            if (fontSize > 70) fontSize = 70;
        }

        HFONT oldF = (HFONT)SelectObject(memDC,gRes.hFont);
        TextOut(memDC, barX + (int)(barW * 1.15f), barY - fontSize, "POP!", 4);
        SelectObject(memDC, oldF);
    }

    if (gLoading.harpoonActive) {
        int hX = heroX + (heroW / 2);
        int hBaseY = rect.bottom;
        int hTopY = hBaseY - (int)gLoading.harpoonHeight;

        int harpoonThickness;
        if (rect.bottom <= 720) {
            harpoonThickness = 3; // Mali ekran
        } else {
            harpoonThickness = barH / 8;
            if (harpoonThickness < 3) harpoonThickness = 3;
            if (harpoonThickness > 6) harpoonThickness = 6;
        }

        HPEN harpPen = CreatePen(PS_SOLID, harpoonThickness, RGB(220, 220, 220));
        HPEN oldPen = (HPEN)SelectObject(memDC, harpPen);
        MoveToEx(memDC, hX, hBaseY, NULL);
        LineTo(memDC, hX, hTopY);
        SelectObject(memDC, oldPen); DeleteObject(harpPen);

        if (gRes.arrow) {
            HDC resDC = CreateCompatibleDC(hdc);
            HBITMAP oldRes = (HBITMAP)SelectObject(resDC, gRes.arrow);
            BITMAP bmArrow; GetObject(gRes.arrow, sizeof(BITMAP), &bmArrow);

            int arrowX = hX - (bmArrow.bmWidth / 2);
            int arrowY = hTopY;

            TransparentBlt(memDC, arrowX, arrowY, bmArrow.bmWidth, bmArrow.bmHeight,
                           resDC, 0, 0, bmArrow.bmWidth, bmArrow.bmHeight,
                           RGB(255, 255, 255));

            SelectObject(resDC, oldRes); DeleteDC(resDC);
        }

        if (!gLoading.ballPopped) {
            float ballY = gLoading.ballY;
            float ballRadius;

            if (rect.bottom <= 720) {
                ballRadius = 16.0f; // Mali ekran
            } else {
                ballRadius = (float)(barH * 0.8f);
                if (ballRadius < 16.0f) ballRadius = 16.0f;
                if (ballRadius > 28.0f) ballRadius = 28.0f;
            }

            if (hTopY <= (int)ballY + (int)ballRadius) {
                if (!gLoading.ballPopped) {
                    gLoading.ballPopped = true;
                    PlaySound(MAKEINTRESOURCE(IDR_BALLOON_POP),
                              GetModuleHandle(NULL),
                              SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
                }
            }
        }
    }

    if (gRes.character && gRes.characterMask) {
        HDC resDC = CreateCompatibleDC(hdc);
        HBITMAP oldRes = (HBITMAP)SelectObject(resDC, gRes.characterMask);

        BITMAP bm; GetObject(gRes.character, sizeof(BITMAP), &bm);
        int frameW = bm.bmWidth / 4;
        int frameH = bm.bmHeight / 3;

        int row = 0;
        if (gLoading.harpoonActive || moveFactor >= 1.0f) {
            row = 2;
        } else {
            row = 0;
        }

        int srcX = 0;
        if (row == 0) {
             int walkFrame = (gLoading.progress / 5) % 4;
             srcX = walkFrame * frameW;
        }

        StretchBlt(memDC, heroX, heroY, heroW, heroH,
                   resDC, srcX, row * frameH, frameW, frameH, SRCAND);

        SelectObject(resDC, gRes.character);
        StretchBlt(memDC, heroX, heroY, heroW, heroH,
                   resDC, srcX, row * frameH, frameW, frameH, SRCPAINT);

        SelectObject(resDC, oldRes); DeleteDC(resDC);
    }

    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));

    char buf[30];

    if (gLoading.progress >= 100) {
        sprintf(buf, "COMPLETE!");
        SetTextColor(memDC, RGB(0, 255, 0));
    } else {
        int dotCount = (GetTickCount() / 500) % 4;
        char dots[5] = "";
        for (int i = 0; i < dotCount; i++) {
            dots[i] = '.';
        }
        dots[dotCount] = '\0';

        sprintf(buf, "LOADING%s %d%%", dots, gLoading.progress);
    }

    int loadingFontSize;
    if (rect.bottom <= 720) {
        loadingFontSize = 16; // Mali ekran
    } else {
        loadingFontSize = rect.bottom / 45; // Fullscreen
        if (loadingFontSize < 16) loadingFontSize = 16;
        if (loadingFontSize > 28) loadingFontSize = 28;
    }

    HFONT oldLoadingFont = (HFONT)SelectObject(memDC, gRes.hFont);

    SIZE sz;
    GetTextExtentPoint32(memDC, buf, strlen(buf), &sz);
    int textX = barX + (barW - sz.cx) / 2;
    int textY = barY + barH + 10;
    TextOut(memDC, textX, textY, buf, strlen(buf));

    SelectObject(memDC, oldLoadingFont);

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBM);
    DeleteObject(memBM);
    DeleteDC(memDC);
}

bool IsLoadingComplete() {
    return gLoading.isComplete;
}
