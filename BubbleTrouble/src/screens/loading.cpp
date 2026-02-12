#include "loading.h"
#include "resourceManager.h"
#include "resources.h"
#include <windows.h>
#include <cmath>
#include <cstdio>

#pragma comment(lib, "Msimg32.lib")

LoadingState gLoading = { 0 };

void InitializeLoading(HWND hwnd) {
    // Hack: Ako slike nisu učitane, učitaj ih sad
    if (gRes.background == NULL) {
        HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
        // Pretpostavka da funkcija postoji (deklarisana u game.h ili slično)
        // LoadBitmaps(hwnd, hInstance);
    }

    gLoading.progress = 0;
    gLoading.isComplete = false;
    gLoading.startTime = GetTickCount();

    // Reset animacije
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

        // Faza 2: Pad (80% do 90%)
        if (gLoading.progress > 80 && gLoading.progress <= 90) {
            gLoading.ballIsFalling = true;
            gLoading.ballVy += 2.0f;
            gLoading.ballY += gLoading.ballVy;
        }
        // Faza 3: Pucanje (90%+)
        else if (gLoading.progress > 90 && !gLoading.ballPopped) {
            gLoading.harpoonActive = true;
            gLoading.harpoonHeight += 35.0f; // Brži harpun jer mora preći veći put

            // Povećali smo granicu jer je heroj sada na dnu, a balon visoko
            if (gLoading.harpoonHeight > 250) {
                gLoading.ballPopped = true;
            }
        }
        last = now;
    }
}

void RenderLoading(HDC hdc, RECT rect) {
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBM = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);

    // 1. POZADINA
    if (gRes.background) {
        HDC resDC = CreateCompatibleDC(hdc);
        HBITMAP oldRes = (HBITMAP)SelectObject(resDC, gRes.background);
        BITMAP bm; GetObject(gRes.background, sizeof(BITMAP), &bm);
        StretchBlt(memDC, 0, 0, rect.right, rect.bottom, resDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        SelectObject(resDC, oldRes); DeleteDC(resDC);
    } else {
        FillRect(memDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    }

    // --- KOORDINATE ---
    int centerX = rect.right / 2;
    int centerY = rect.bottom / 2;

    int barW = 400;
    int barH = 20;
    int barX = centerX - (barW / 2);
    int barY = centerY;

    // Heroj dimenzije
    int heroW = 70;
    int heroH = 90;

    // Fiksiramo heroja na samo dno ekrana (bez oduzimanja, ili vrlo malo ako treba)
    int heroY = rect.bottom - heroH;

    // --- LOGIKA KRETANJA ---
    int targetX = (barX + barW) - (heroW / 2);
    int startX = -heroW;

    float moveFactor = (float)gLoading.progress / 90.0f;
    if (moveFactor > 1.0f) moveFactor = 1.0f;

    int heroX = startX + (int)((targetX - startX) * moveFactor);

    // 3. LOADING BAR
    HBRUSH frameBrush = CreateSolidBrush(RGB(50, 50, 50));
    RECT frameRect = { barX - 2, barY - 2, barX + barW + 2, barY + barH + 2 };
    FillRect(memDC, &frameRect, frameBrush); DeleteObject(frameBrush);

    int fillW = (barW * gLoading.progress) / 100;
    HBRUSH fillBrush = CreateSolidBrush(RGB(220, 40, 40));
    RECT fillRect = { barX, barY, barX + fillW, barY + barH };
    FillRect(memDC, &fillRect, fillBrush); DeleteObject(fillBrush);

    // 4. BALON
    if (!gLoading.ballPopped) {
        float bx, by;
        float radius = 15.0f;
        if (!gLoading.ballIsFalling) {
            float t = (float)gLoading.progress / 80.0f; if (t > 1.0f) t = 1.0f;
            bx = barX + (barW * t);
            by = barY - radius - fabs(sinf(t * 15.0f)) * 50.0f;
            gLoading.ballY = by;
        } else {
            bx = barX + barW + radius;
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
        HFONT hFontPow = CreateFont(40, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
        HFONT oldF = (HFONT)SelectObject(memDC, hFontPow);
        TextOut(memDC, barX + barW, barY + 50, "POP!", 4);
        SelectObject(memDC, oldF); DeleteObject(hFontPow);
    }

    // 5. HARPUN
    if (gLoading.harpoonActive) {
        int hX = heroX + (heroW / 2);
        int hBaseY = rect.bottom;
        int hTopY = hBaseY - (int)gLoading.harpoonHeight;

        // Lanac
        HPEN harpPen = CreatePen(PS_SOLID, 2, RGB(200, 0, 0));
        HPEN oldPen = (HPEN)SelectObject(memDC, harpPen);
        MoveToEx(memDC, hX, hBaseY, NULL);
        for (int y = hBaseY; y > hTopY; y -= 6) {
            int offset = (y % 12) ? 3 : -3;
            LineTo(memDC, hX + offset, y);
        }
        SelectObject(memDC, oldPen); DeleteObject(harpPen);

        // Vrh Harpuna (Arrow) - KORISTIMO TransparentBlt SA BIJELOM BOJOM
        if (gRes.arrow) {
            HDC resDC = CreateCompatibleDC(hdc);
            HBITMAP oldRes = (HBITMAP)SelectObject(resDC, gRes.arrow);
            BITMAP bmArrow; GetObject(gRes.arrow, sizeof(BITMAP), &bmArrow);

            int arrowX = hX - (bmArrow.bmWidth / 2);
            int arrowY = hTopY;

            // RGB(255, 255, 255) uklanja bijelu pozadinu
            TransparentBlt(memDC, arrowX, arrowY, bmArrow.bmWidth, bmArrow.bmHeight,
                           resDC, 0, 0, bmArrow.bmWidth, bmArrow.bmHeight,
                           RGB(255, 255, 255));

            SelectObject(resDC, oldRes); DeleteDC(resDC);
        }
    }

 if (gRes.character && gRes.characterMask) {
        HDC resDC = CreateCompatibleDC(hdc);
        HBITMAP oldRes = (HBITMAP)SelectObject(resDC, gRes.characterMask); // Maska prvo

        BITMAP bm; GetObject(gRes.character, sizeof(BITMAP), &bm);
        int frameW = bm.bmWidth / 4;
        int frameH = bm.bmHeight / 3;

        int row = 0;
        if (gLoading.harpoonActive || moveFactor >= 1.0f) {
            row = 2; // Leđa (puca/čeka)
        } else {
            row = 0; // Hoda desno
        }

        int srcX = 0;
        if (row == 0) {
             int walkFrame = (gLoading.progress / 5) % 4;
             srcX = walkFrame * frameW;
        }

        // 1. Maska (SRCAND)
        StretchBlt(memDC, heroX, heroY, heroW, heroH,
                   resDC, srcX, row * frameH, frameW, frameH, SRCAND);

        // 2. Slika (SRCPAINT)
        SelectObject(resDC, gRes.character);
        StretchBlt(memDC, heroX, heroY, heroW, heroH,
                   resDC, srcX, row * frameH, frameW, frameH, SRCPAINT);

        SelectObject(resDC, oldRes); DeleteDC(resDC);
    }
    // 7. TEXT
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));
    char buf[30]; sprintf(buf, "LOADING... %d%%", gLoading.progress);
    SIZE sz; GetTextExtentPoint32(memDC, buf, strlen(buf), &sz);
    int textX = barX + (barW - sz.cx) / 2;
    TextOut(memDC, textX, barY - 30, buf, strlen(buf));

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBM); DeleteObject(memBM); DeleteDC(memDC);
}

bool IsLoadingComplete() {
    return gLoading.isComplete;
}
