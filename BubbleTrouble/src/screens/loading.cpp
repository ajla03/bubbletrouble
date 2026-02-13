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
            gLoading.harpoonHeight += 35.0f;

            // Dinamička provjera - postavi targetnu visinu u renderu
            // Ovdje samo rastemo, provjera je u RenderLoading funkciji
        }
        last = now;
    }
}

void RenderLoading(HDC hdc, RECT rect) {
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBM = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);

    // 1. POZADINA - Tamno siva (charcoal) uvijek
    HBRUSH bgBrush = CreateSolidBrush(RGB(35, 35, 40));
    FillRect(memDC, &rect, bgBrush);
    DeleteObject(bgBrush);

    /* Zakomentarisano - koristimo solid sivu umjesto bitmap-a
    if (gRes.background) {
        HDC resDC = CreateCompatibleDC(hdc);
        HBITMAP oldRes = (HBITMAP)SelectObject(resDC, gRes.background);
        BITMAP bm; GetObject(gRes.background, sizeof(BITMAP), &bm);
        StretchBlt(memDC, 0, 0, rect.right, rect.bottom, resDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        SelectObject(resDC, oldRes); DeleteDC(resDC);
    }
    */

    // --- KOORDINATE - OPTIMIZOVANO ZA 1280x720 I FULLSCREEN ---
    int centerX = rect.right / 2;
    int centerY = rect.bottom / 2;

    // Progress bar skalira pametno
    int barW;
    if (rect.right <= 1280) {
        // Mali ekran (1280x720) - fiksna veličina
        barW = 450;
    } else {
        // Fullscreen - skalira se (50% širine)
        barW = (rect.right * 50) / 100;
        if (barW > 900) barW = 900;  // Max 900px
    }

    int barH;
    if (rect.bottom <= 720) {
        // Mali ekran - fiksna visina
        barH = 25;
    } else {
        // Fullscreen - proporcionalna visina
        barH = rect.bottom / 35;
        if (barH < 25) barH = 25;
        if (barH > 45) barH = 45;
    }

    int barX = centerX - (barW / 2);
    int barY = centerY;

    // Heroj dimenzije - takođe optimizovane
    int heroW;
    int heroH;
    if (rect.right <= 1280) {
        // Mali ekran
        heroW = 70;
        heroH = 90;
    } else {
        // Fullscreen - skalira
        heroW = rect.right / 18;
        if (heroW < 70) heroW = 70;
        if (heroW > 130) heroW = 130;

        heroH = (heroW * 9) / 7;
        if (heroH < 90) heroH = 90;
        if (heroH > 160) heroH = 160;
    }

    // Fiksiramo heroja na samo dno ekrana (bez oduzimanja, ili vrlo malo ako treba)
    int heroY = rect.bottom - heroH;

    // --- LOGIKA KRETANJA - do pozicije balona (15% van bara) ---
    int targetX = (int)(barX + (barW * 1.15f)) - (heroW / 2); // Hoda do pozicije balona
    int startX = -heroW;

    float moveFactor = (float)gLoading.progress / 90.0f;
    if (moveFactor > 1.0f) moveFactor = 1.0f;

    int heroX = startX + (int)((targetX - startX) * moveFactor);

    // 3. LOADING BAR - SA OKVIROM (optimizovan za obe rezolucije)
    int frameThickness;
    if (rect.bottom <= 720) {
        frameThickness = 4; // Mali ekran
    } else {
        frameThickness = barH / 6; // Fullscreen
        if (frameThickness < 4) frameThickness = 4;
        if (frameThickness > 7) frameThickness = 7;
    }

    // Spoljašnji okvir (bijela boja za bolji kontrast sa gradijentom)
    HBRUSH outerFrameBrush = CreateSolidBrush(RGB(255, 255, 255)); // Bijela
    RECT outerRect = { barX - frameThickness, barY - frameThickness,
                       barX + barW + frameThickness, barY + barH + frameThickness };
    FillRect(memDC, &outerRect, outerFrameBrush);
    DeleteObject(outerFrameBrush);

    // Unutrašnji frame (tamna pozadina)
    HBRUSH innerFrameBrush = CreateSolidBrush(RGB(30, 30, 30));
    RECT innerRect = { barX, barY, barX + barW, barY + barH };
    FillRect(memDC, &innerRect, innerFrameBrush);
    DeleteObject(innerFrameBrush);

    // Fill (crveni progres sa gradijentom)
    int fillW = (barW * gLoading.progress) / 100;

    if (fillW > 0) {
        // Gradijent od tamno crvene do svijetle crvene
        TRIVERTEX vertex[2];
        vertex[0].x = barX;
        vertex[0].y = barY;
        vertex[0].Red = 0x6000;    // Tamno crvena lijevo
        vertex[0].Green = 0x1000;
        vertex[0].Blue = 0x1000;
        vertex[0].Alpha = 0x0000;

        vertex[1].x = barX + fillW;
        vertex[1].y = barY + barH;
        vertex[1].Red = 0xFF00;    // Svijetlo crvena desno
        vertex[1].Green = 0x3000;
        vertex[1].Blue = 0x3000;
        vertex[1].Alpha = 0x0000;

        GRADIENT_RECT gRect;
        gRect.UpperLeft = 0;
        gRect.LowerRight = 1;

        GradientFill(memDC, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
    }

    // 4. BALON - optimizovan za obe rezolucije
    if (!gLoading.ballPopped) {
        float bx, by;
        float radius;

        if (rect.bottom <= 720) {
            // Mali ekran - fiksni radius
            radius = 16.0f;
        } else {
            // Fullscreen - proporcionalni radius
            radius = (float)(barH * 0.8f);
            if (radius < 16.0f) radius = 16.0f;
            if (radius > 28.0f) radius = 28.0f;
        }

        if (!gLoading.ballIsFalling) {
            // Balon skakuće NA baru i malo PREKO ivice (1.15 = 115% - prelazi ivicu)
            float t = (float)gLoading.progress / 80.0f; if (t > 1.0f) t = 1.0f;
            bx = barX + (barW * t * 1.15f); // Dodato 1.15 da ide 15% dalje od kraja
            by = barY - radius - fabs(sinf(t * 15.0f)) * 50.0f;
            gLoading.ballY = by;
        } else {
            // Pada sa kraja bara + malo offset-a
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

        HFONT hFontPow = CreateFont(fontSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
        HFONT oldF = (HFONT)SelectObject(memDC, hFontPow);
        TextOut(memDC, barX + (int)(barW * 1.15f), barY + 50, "POP!", 4);
        SelectObject(memDC, oldF); DeleteObject(hFontPow);
    }

    // 5. HARPUN
    if (gLoading.harpoonActive) {
        int hX = heroX + (heroW / 2);
        int hBaseY = rect.bottom;
        int hTopY = hBaseY - (int)gLoading.harpoonHeight;

        // Jednostavna ravna linija umjesto lanca - optimizovana debljina
        int harpoonThickness;
        if (rect.bottom <= 720) {
            harpoonThickness = 3; // Mali ekran
        } else {
            harpoonThickness = barH / 8;
            if (harpoonThickness < 3) harpoonThickness = 3;
            if (harpoonThickness > 6) harpoonThickness = 6;
        }

        HPEN harpPen = CreatePen(PS_SOLID, harpoonThickness, RGB(220, 220, 220)); // Srebrna linija
        HPEN oldPen = (HPEN)SelectObject(memDC, harpPen);
        MoveToEx(memDC, hX, hBaseY, NULL);
        LineTo(memDC, hX, hTopY); // Samo ravna linija gore
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

        // DINAMIČKA PROVJERA POGOTKA - bazirana na stvarnoj poziciji balona
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

            // Provjeri da li je vrh harpuna stigao do balona (sa malim offsetom)
            if (hTopY <= (int)ballY + (int)ballRadius) { // tolerance zona bazirana na radiusu
                gLoading.ballPopped = true;
            }
        }
    }

    // 6. KARAKTER
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

    // 7. TEXT - ISPOD BARA sa animiranim tačkama i optimizovanim fontom
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));

    char buf[30];

    if (gLoading.progress >= 100) {
        // Kada je 100%, prikaži COMPLETE
        sprintf(buf, "COMPLETE!");
        SetTextColor(memDC, RGB(0, 255, 0)); // Zelena boja za complete
    } else {
        // Animirane tačke (. .. ... .) dok se učitava
        int dotCount = (GetTickCount() / 500) % 4; // Mijenja se svake 500ms
        char dots[5] = "";
        for (int i = 0; i < dotCount; i++) {
            dots[i] = '.';
        }
        dots[dotCount] = '\0';

        sprintf(buf, "LOADING%s %d%%", dots, gLoading.progress);
    }

    // Optimizovani font size za obe rezolucije
    int loadingFontSize;
    if (rect.bottom <= 720) {
        loadingFontSize = 16; // Mali ekran
    } else {
        loadingFontSize = rect.bottom / 45; // Fullscreen
        if (loadingFontSize < 16) loadingFontSize = 16;
        if (loadingFontSize > 28) loadingFontSize = 28;
    }

    HFONT hLoadingFont = CreateFont(loadingFontSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
    HFONT oldLoadingFont = (HFONT)SelectObject(memDC, hLoadingFont);

    SIZE sz;
    GetTextExtentPoint32(memDC, buf, strlen(buf), &sz);
    int textX = barX + (barW - sz.cx) / 2;
    int textY = barY + barH + 10; // PROMIJENIO - tekst sada ide ispod bara
    TextOut(memDC, textX, textY, buf, strlen(buf));

    SelectObject(memDC, oldLoadingFont);
    DeleteObject(hLoadingFont);

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBM); DeleteObject(memBM); DeleteDC(memDC);
}

bool IsLoadingComplete() {
    return gLoading.isComplete;
}
