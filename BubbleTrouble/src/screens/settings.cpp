#include "game.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "constants.h"
#include <wingdi.h>

static void RenderBackground(HDC hdcBuffer, RECT rect);
static void RenderTransparentSheet(HDC hdcBuffer, RECT rect, RECT* outSheet);
static void RenderSettingsTitle(HDC hdcBuffer, RECT sheet);
static void RenderControlsHolder(HDC hdcBuffer, RECT sheet, int* outX, int* outY, int* outControlW, int* outControlH);
static void RenderHeroesInHolder(HDC hdcBuffer, int x, int y, int controlW, int controlH);
static void RenderSingleKeyButton(HDC hdcBuffer, int currentBtnX, int currentBtnY, int btnW, int btnH, int buttonIndex);
static void RenderKeyButtons(HDC hdcBuffer, int x, int y, int controlW, int controlH);
static void RenderPlayerButtons(HDC hdcBuffer, RECT sheet, int x, int y, int controlH);
static void RenderBackButton(HDC hdcBuffer, RECT sheet);
static void RenderTorches(HDC hdcBuffer, RECT sheet);
static void RenderSoundHolder(HDC hdcBuffer, RECT sheet);
static void RenderKeyBindingOverlay(HDC hdcBuffer, RECT rect);


void RenderSettings(HDC hdcBuffer, RECT rect)
{
    RECT sheet;

    RenderBackground(hdcBuffer, rect);

    // --- PROZIRNI SHEET ---
    RenderTransparentSheet(hdcBuffer, rect, &sheet);

    // SETTINGS TEKST //
    HFONT oldFont = (HFONT)SelectObject(hdcBuffer, gRes.hFont);
    RenderSettingsTitle(hdcBuffer, sheet);

    /* CONTROLS */
    int controlX, controlY, controlW, controlH;
    RenderControlsHolder(hdcBuffer, sheet, &controlX, &controlY, &controlW, &controlH);
    RenderHeroesInHolder(hdcBuffer, controlX, controlY, controlW, controlH);
    RenderKeyButtons(hdcBuffer, controlX, controlY, controlW, controlH);
    RenderPlayerButtons(hdcBuffer, sheet, controlX, controlY, controlH);

    // BACK BUTTON //
    RenderBackButton(hdcBuffer, sheet);

    // --- SOUND HOLDER ---
    RenderSoundHolder(hdcBuffer, sheet);

    // --- TORCHES ---
    RenderTorches(hdcBuffer, sheet);

    // ANIMATION - KEY BINDING //
    if (gGame.settingsState.waitingForKey != KEYBIND_NONE) {
        RenderKeyBindingOverlay(hdcBuffer, rect);
    }

    SelectObject(hdcBuffer, oldFont);
}


void InitDefaultSettings(){
    gGame.settingsState.soundState.bgMusicOn = true;
    gGame.settingsState.soundState.soundEffectsOn = true;
    gGame.settingsState.currentHeroSelected = gRes.characterMask;

    // player 1 - default controls
    gGame.settingsState.player1Keys.moveLeft = VK_LEFT;
    gGame.settingsState.player1Keys.moveRight = VK_RIGHT;
    gGame.settingsState.player1Keys.shoot = VK_SPACE;

    // player 2 - default controls
    gGame.settingsState.player2Keys.moveLeft = VK_LEFT;
    gGame.settingsState.player2Keys.moveRight = VK_RIGHT;
    gGame.settingsState.player2Keys.shoot = VK_SPACE;

    gGame.settingsState.waitingForKey = KEYBIND_NONE;
    gGame.settingsState.currentPlayerBinding = 1;
    gGame.settingsState.dotAnimation.dotCount = 0;
    gGame.settingsState.dotAnimation.lastDotTime = GetTickCount();
    gGame.settingsState.dotAnimation.dotInterval = 400;

}

const char* VKCodeToString(int vkCode)
{
    static char buffer[32];

     if (vkCode >= 'A' && vkCode <= 'Z') {
        buffer[0] = (char)vkCode;
        buffer[1] = '\0';
        return buffer;
    }

    if (vkCode >= '0' && vkCode <= '9') {
        buffer[0] = (char)vkCode;
        buffer[1] = '\0';
        return buffer;
    }

    switch (vkCode) {
        case VK_LEFT:   return "<-";
        case VK_RIGHT:  return "->";
        case VK_UP:     return "UP";
        case VK_DOWN:   return "DOWN";
        case VK_SPACE:  return "SPC";
        case VK_SHIFT:  return "SHIFT";
        case VK_CONTROL:return "CTRL";
        case VK_MENU:   return "ALT";
        case VK_TAB:    return "TAB";
        case VK_BACK:   return "BKSP";
        case VK_DELETE: return "DEL";
        case VK_INSERT: return "INS";
        case VK_HOME:   return "HOME";
        case VK_END:    return "END";
        case VK_PRIOR:  return "PGUP";
        case VK_NEXT:   return "PGDN";

        // Function keys
        case VK_F1:  return "F1";
        case VK_F2:  return "F2";
        case VK_F3:  return "F3";
        case VK_F4:  return "F4";
        case VK_F5:  return "F5";
        case VK_F6:  return "F6";
        case VK_F7:  return "F7";
        case VK_F8:  return "F8";
        case VK_F9:  return "F9";
        case VK_F10: return "F10";
        case VK_F11: return "F11";
        case VK_F12: return "F12";

        // Numpad
        case VK_NUMPAD0: return "NUM0";
        case VK_NUMPAD1: return "NUM1";
        case VK_NUMPAD2: return "NUM2";
        case VK_NUMPAD3: return "NUM3";
        case VK_NUMPAD4: return "NUM4";
        case VK_NUMPAD5: return "NUM5";
        case VK_NUMPAD6: return "NUM6";
        case VK_NUMPAD7: return "NUM7";
        case VK_NUMPAD8: return "NUM8";
        case VK_NUMPAD9: return "NUM9";
        case VK_MULTIPLY: return "*";
        case VK_ADD:      return "+";
        case VK_SUBTRACT: return "-";
        case VK_DECIMAL:  return ".";
        case VK_DIVIDE:   return "/";
    }

    switch (vkCode) {
        case VK_OEM_PLUS:   return "+";
        case VK_OEM_MINUS:  return "-";
        case VK_OEM_COMMA:  return ",";
        case VK_OEM_PERIOD: return ".";
        case VK_OEM_1:      return ";";
        case VK_OEM_2:      return "/";
        case VK_OEM_3:      return "`";
        case VK_OEM_4:      return "[";
        case VK_OEM_6:      return "]";
        case VK_OEM_7:      return "'";
    }

    return "?";
}


static void RenderBackground(HDC hdcBuffer, RECT rect){
    SetStretchBltMode(hdcBuffer, HALFTONE);
    int tileW = gGame.floorWall.width;
    int tileH = gGame.leftWall.height;

    SelectObject(gRes.hdcMem, gRes.wall);
    StretchBlt(hdcBuffer, 0, 0, rect.right, rect.bottom, gRes.hdcMem, 0, 0, tileW, tileH, SRCCOPY);
}

static void RenderTransparentSheet(HDC hdcBuffer, RECT rect, RECT* outSheet) {
    outSheet->left = rect.right / 2 - SHEET_W / 2;
    outSheet->right = outSheet->left + SHEET_W;
    outSheet->top = rect.bottom / 2 - SHEET_H / 2;
    outSheet->bottom = outSheet->top + SHEET_H;

    HBITMAP bmp = CreateCompatibleBitmap(hdcBuffer, 1, 1);
    HBITMAP hBmpOld = (HBITMAP) SelectObject(gRes.hdcMem, bmp);
    SetPixel(gRes.hdcMem, 0, 0, RGB(255, 255, 255));

    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 160;
    bf.AlphaFormat = 0;
    AlphaBlend(hdcBuffer, outSheet->left, outSheet->top,
               outSheet->right - outSheet->left, outSheet->bottom - outSheet->top,
               gRes.hdcMem, 0, 0, 1, 1, bf);

    SelectObject(gRes.hdcMem, hBmpOld);
    DeleteObject(bmp);

    // SIVI BORDER //

    HPEN darkPen = CreatePen(PS_SOLID, 2, RGB(80,80,80));
    HPEN oldPen = (HPEN)SelectObject(hdcBuffer, darkPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdcBuffer, GetStockObject(NULL_BRUSH));

    Rectangle(
        hdcBuffer,
        outSheet->left - 2,
        outSheet->top - 2,
        outSheet->right + 2,
        outSheet->bottom + 2
    );

    HPEN lightPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    SelectObject(hdcBuffer, lightPen);

    Rectangle(
        hdcBuffer,
        outSheet->left,
        outSheet->top,
        outSheet->right,
        outSheet->bottom
    );

    SelectObject(hdcBuffer, oldPen);
    SelectObject(hdcBuffer, oldBrush);
    DeleteObject(darkPen);
    DeleteObject(lightPen);
}

static void RenderSettingsTitle(HDC hdcBuffer, RECT sheet) {
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(60, 60, 60));

    RECT textRect;
    textRect.left = sheet.left;
    textRect.right = sheet.right;
    textRect.top = sheet.top + 20;
    textRect.bottom = sheet.top + 60;
    DrawText(hdcBuffer, "SETTINGS", -1, &textRect, DT_CENTER | DT_TOP | DT_SINGLELINE);
}

static void RenderControlsHolder(HDC hdcBuffer, RECT sheet, int* outX, int* outY, int* outControlW, int* outControlH) {
    BITMAP bm;
    int padding = 40;
    GetObject(gRes.controlsHolder, sizeof(BITMAP), &bm);

    *outControlW = bm.bmWidth / 1.5;
    *outControlH = bm.bmHeight / 1.5;
    *outX = sheet.right - *outControlW - padding;
    *outY = 15 + (sheet.top + 60);

    SelectObject(gRes.hdcMem, gRes.controlsHolder);
    TransparentBlt(hdcBuffer, *outX, *outY, *outControlW, *outControlH,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));
}

static void RenderHeroesInHolder(HDC hdcBuffer, int x, int y, int controlW, int controlH) {
    int heroW = gGame.hero.width * 1.5;
    int heroH = gGame.hero.height * 1.5;
    int spacing = 30;
    int heroY = y + (controlH / 2) - (heroH / 2) - 10;
    int midHeroX = x + (controlW / 2) - (heroW / 2);
    int leftHeroX = midHeroX - heroW - spacing;
    int rightHeroX = midHeroX + heroW + spacing;

    SetStretchBltMode(hdcBuffer, COLORONCOLOR);
    SelectObject(gRes.hdcMem, gGame.settingsState.currentHeroSelected);

    // LIJEVO
    int srcX_left = 0 * gGame.hero.width;
    int srcY_left = 1 * gGame.hero.height;
    TransparentBlt(hdcBuffer, leftHeroX, heroY, heroW, heroH,
                   gRes.hdcMem, srcX_left, srcY_left, gGame.hero.width, gGame.hero.height, RGB(255, 255, 255));

    // SREDINA
    int srcX_back = 0 * gGame.hero.width;
    int srcY_back = 2 * gGame.hero.height;
    TransparentBlt(hdcBuffer, midHeroX, heroY, heroW, heroH,
                   gRes.hdcMem, srcX_back, srcY_back, gGame.hero.width, gGame.hero.height, RGB(255, 255, 255));

    // DESNO
    int srcX_right = 3 * gGame.hero.width;
    int srcY_right = 0 * gGame.hero.height;
    TransparentBlt(hdcBuffer, rightHeroX, heroY, heroW, heroH,
                   gRes.hdcMem, srcX_right, srcY_right, gGame.hero.width, gGame.hero.height, RGB(255, 255, 255));
}

static void RenderSingleKeyButton(HDC hdcBuffer, int currentBtnX, int currentBtnY, int btnW, int btnH, int buttonIndex) {
    if (buttonIndex == 0) {
        gGame.settingsState.leftKeyButton.x = currentBtnX;
        gGame.settingsState.leftKeyButton.y = currentBtnY;
        gGame.settingsState.leftKeyButton.width = btnW;
        gGame.settingsState.leftKeyButton.height = btnH;
    } else if (buttonIndex == 1) {
        gGame.settingsState.spaceKeyButton.x = currentBtnX;
        gGame.settingsState.spaceKeyButton.y = currentBtnY;
        gGame.settingsState.spaceKeyButton.width = btnW;
        gGame.settingsState.spaceKeyButton.height = btnH;
    } else if (buttonIndex == 2) {
        gGame.settingsState.rightKeyButton.x = currentBtnX;
        gGame.settingsState.rightKeyButton.y = currentBtnY;
        gGame.settingsState.rightKeyButton.width = btnW;
        gGame.settingsState.rightKeyButton.height = btnH;
    }

    BITMAP bmBtn;
    GetObject(gRes.settingsPlayer, sizeof(BITMAP), &bmBtn);
    SelectObject(gRes.hdcMem, gRes.settingsPlayer);
    TransparentBlt(hdcBuffer, currentBtnX, currentBtnY, btnW, btnH,
                   gRes.hdcMem, 0, 0, bmBtn.bmWidth, bmBtn.bmHeight, RGB(255, 255, 255));

    bool isWaiting = false;
    if (buttonIndex == 0 && gGame.settingsState.waitingForKey == KEYBIND_LEFT) isWaiting = true;
    if (buttonIndex == 1 && gGame.settingsState.waitingForKey == KEYBIND_SHOOT) isWaiting = true;
    if (buttonIndex == 2 && gGame.settingsState.waitingForKey == KEYBIND_RIGHT) isWaiting = true;

    RECT btnRect = { currentBtnX, currentBtnY, currentBtnX + btnW, currentBtnY + btnH };

    if (isWaiting) {
        SetTextColor(hdcBuffer, RGB(255, 255, 0));
        DrawText(hdcBuffer, "...", -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SetTextColor(hdcBuffer, RGB(255, 255, 255)); // VRATI BIJELU BOJU NAZAD!
    } else {
        KeyBindings* keys = (gGame.settingsState.currentPlayerBinding == 1) ?
                            &gGame.settingsState.player1Keys : &gGame.settingsState.player2Keys;
        const char *keyText;
        if (buttonIndex == 0) keyText = VKCodeToString(keys->moveLeft);
        else if (buttonIndex == 1) keyText = VKCodeToString(keys->shoot);
        else keyText = VKCodeToString(keys->moveRight);

        SetTextColor(hdcBuffer, RGB(255, 255, 255));
        DrawText(hdcBuffer, keyText, -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

static void RenderKeyButtons(HDC hdcBuffer, int x, int y, int controlW, int controlH) {
    int heroW = gGame.hero.width * 1.5;
    int heroH = gGame.hero.height * 1.5;
    int spacing = 40;
    int heroY = y + (controlH / 2) - (heroH / 2) - 20;
    int midHeroX = x + (controlW / 2) - (heroW / 2);
    int leftHeroX = midHeroX - heroW - spacing;
    int rightHeroX = midHeroX + heroW + spacing;

    int btnW = 75;
    int btnH = 40;
    int btnOffsetY = heroH + 15;

    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    SetBkMode(hdcBuffer, TRANSPARENT);

    int btnXPositions[] = {
        leftHeroX + (heroW / 2) - (btnW / 2),
        midHeroX + (heroW / 2) - (btnW / 2),
        rightHeroX + (heroW / 2) - (btnW / 2)
    };

    for (int i = 0; i < 3; i++) {
        int currentBtnX = btnXPositions[i];
        int currentBtnY = heroY + btnOffsetY;
        RenderSingleKeyButton(hdcBuffer, currentBtnX, currentBtnY, btnW, btnH, i);
    }
}


static void RenderPlayerButtons(HDC hdcBuffer, RECT sheet, int x, int y, int controlH) {
    int btnW = gGame.backButtonInfo.width * 1.5;
    int btnH = gGame.backButtonInfo.height / 1.5;
    int padding = 40;
    int btnX = x / 2 - btnW / 2 + sheet.left / 2;

    int groupHeight = btnH * 2 + padding;
    int groupTop = y + (controlH - groupHeight) / 2;
    int firstBtnY = groupTop;
    int secondBtnY = groupTop + btnH + padding;

    BITMAP bm;
    HBITMAP currentBitmap;

    // PLAYER 1
    if (gGame.player1.isHover)
        currentBitmap = gRes.playerHover;
    else
        currentBitmap = gRes.backButtonMask;

    GetObject(currentBitmap, sizeof(BITMAP), &bm);
    SelectObject(gRes.hdcMem, currentBitmap);
    TransparentBlt(hdcBuffer, btnX, firstBtnY, btnW, btnH,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));

    gGame.player2.x = gGame.player1.x = btnX;
    gGame.player2.y = secondBtnY;
    gGame.player1.y = firstBtnY;

    RECT r1 = { btnX, firstBtnY, btnX + btnW, firstBtnY + btnH };
    DrawText(hdcBuffer, "PLAYER 1", -1, &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // PLAYER 2
    if (gGame.player2.isHover)
        currentBitmap = gRes.playerHover;
    else
        currentBitmap = gRes.backButtonMask;

    GetObject(currentBitmap, sizeof(BITMAP), &bm);
    SelectObject(gRes.hdcMem, currentBitmap);
    TransparentBlt(hdcBuffer, btnX, secondBtnY, btnW, btnH,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));

    RECT r2 = { btnX, secondBtnY, btnX + btnW, secondBtnY + btnH };
    DrawText(hdcBuffer, "PLAYER 2", -1, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static void RenderBackButton(HDC hdcBuffer, RECT sheet) {
    int btnWidth = gGame.backButtonInfo.width;
    int btnHeight = gGame.backButtonInfo.height;
    int padding = 40;
    int x = sheet.left + padding;
    int y = sheet.bottom - btnHeight / 2 - padding;

    gGame.backButtonInfo.x = x;
    gGame.backButtonInfo.y = y;

    BITMAP bm;
    HBITMAP currentBitmap;

    if (gGame.backButtonInfo.isHover)
        currentBitmap = gRes.playerHover;
    else
        currentBitmap = gRes.backButtonMask;

    GetObject(currentBitmap, sizeof(BITMAP), &bm);
    SelectObject(gRes.hdcMem, currentBitmap);
    TransparentBlt(hdcBuffer, x, y, btnWidth, btnHeight / 2,
                   gRes.hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));

    RECT textRect = { x, y, x + btnWidth, y + btnHeight / 2 };
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    DrawText(hdcBuffer, "BACK", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static void RenderTorches(HDC hdcBuffer, RECT sheet) {
    float torchScale = 2.0f;
    int torchW = (int)(gGame.torchInfo.width * torchScale);
    int torchH = (int)(gGame.torchInfo.height * torchScale);
    int torchX1 = sheet.left / 2 - torchW / 2;
    int torchX2 = sheet.right + sheet.left / 2 - torchW / 2;
    int torchY = sheet.bottom / 2;

    int torchSrcX = gGame.torchInfo.currentFrame * gGame.torchInfo.width;
    int torchSrcY = gGame.torchInfo.currentRow * gGame.torchInfo.height;

    // Prva baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX1, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);

    // Druga baklja
    SelectObject(gRes.hdcMem, gRes.torchMask);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCPAINT);
    SelectObject(gRes.hdcMem, gRes.torch);
    StretchBlt(hdcBuffer, torchX2, torchY, torchW, torchH,
               gRes.hdcMem, torchSrcX, torchSrcY, gGame.torchInfo.width, gGame.torchInfo.height, SRCAND);
}

static void RenderSoundHolder(HDC hdcBuffer, RECT sheet)
{
    int padding = 40;

    /* ================= MUSIC BUTTON ================= */

    BITMAP bmBase;
    GetObject(gRes.hMusicHolder, sizeof(BITMAP), &bmBase);

    float soundScale = 0.7f;
    int soundW = (int)(bmBase.bmWidth * soundScale);
    int soundH = (int)(bmBase.bmHeight * soundScale);

    int x = sheet.right - soundW - padding;
    int y = sheet.bottom - soundH - padding;

    gGame.settingsMusicButtonInfo.x = x;
    gGame.settingsMusicButtonInfo.y = y;
    gGame.settingsMusicButtonInfo.width  = soundW;
    gGame.settingsMusicButtonInfo.height = soundH;

    HBITMAP hbmMusic;
    if (gGame.settingsState.soundState.bgMusicOn) {
        hbmMusic = gGame.settingsMusicButtonInfo.isHover
                 ? gRes.settingsMusicOnHover
                 : gRes.hMusicHolder;
    } else {
        hbmMusic = gGame.settingsMusicButtonInfo.isHover
                 ? gRes.musicOffHover
                 : gRes.musicOff;
    }

    BITMAP bmDraw;
    GetObject(hbmMusic, sizeof(BITMAP), &bmDraw);

    SelectObject(gRes.hdcMem, hbmMusic);
    TransparentBlt(
        hdcBuffer,
        x, y,
        soundW, soundH,
        gRes.hdcMem,
        0, 0,
        bmDraw.bmWidth, bmDraw.bmHeight,
        RGB(255, 255, 255)
    );

    RECT labelRect = { x - 350, y, x - 10, y + soundH };
    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(80, 80, 80));
    DrawText(hdcBuffer, "Background Music:", -1, &labelRect,
             DT_RIGHT | DT_VCENTER | DT_SINGLELINE);


    /* ================= SOUND EFFECTS BUTTON ================= */

    int sfxY = y - padding / 2 - soundH;

    gGame.settingsSoundButtonInfo.x = x;
    gGame.settingsSoundButtonInfo.y = sfxY;
    gGame.settingsSoundButtonInfo.width  = soundW;
    gGame.settingsSoundButtonInfo.height = soundH;

    HBITMAP hbmSfx;
    if (gGame.settingsState.soundState.soundEffectsOn) {
        hbmSfx = gGame.settingsSoundButtonInfo.isHover
               ? gRes.soundOnHover
               : gRes.hSoundOn;
    } else {
        hbmSfx = gGame.settingsSoundButtonInfo.isHover
               ? gRes.settingsSoundOffHover
               : gRes.settingsSoundOff;
    }

    GetObject(hbmSfx, sizeof(BITMAP), &bmDraw);

    SelectObject(gRes.hdcMem, hbmSfx);
    TransparentBlt(
        hdcBuffer,
        x, sfxY,
        soundW, soundH,
        gRes.hdcMem,
        0, 0,
        bmDraw.bmWidth, bmDraw.bmHeight,
        RGB(255, 255, 255)
    );

    labelRect = { x - 350, sfxY, x - 10, sfxY + soundH };
    DrawText(hdcBuffer, "Sound Effects:", -1, &labelRect,
             DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
}

static void RenderKeyBindingOverlay(HDC hdcBuffer, RECT rect){
    HBITMAP bmp = CreateCompatibleBitmap(hdcBuffer, 1, 1);
    HBITMAP hBmpOld = (HBITMAP) SelectObject(gRes.hdcMem, bmp);
    SetPixel(gRes.hdcMem, 0, 0, RGB(0, 0, 0));

    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 180;
    bf.AlphaFormat = 0;

    AlphaBlend(hdcBuffer, 0, 0, rect.right, rect.bottom, gRes.hdcMem, 0, 0, 1, 1, bf);

    SelectObject(gRes.hdcMem, hBmpOld);
    DeleteObject(bmp);

    DWORD currentTime = GetTickCount();
    if (currentTime - gGame.settingsState.dotAnimation.lastDotTime >=
        gGame.settingsState.dotAnimation.dotInterval) {
        gGame.settingsState.dotAnimation.dotCount++;
        if (gGame.settingsState.dotAnimation.dotCount > 3) {
            gGame.settingsState.dotAnimation.dotCount = 1;
        }
        gGame.settingsState.dotAnimation.lastDotTime = currentTime;
    }

    SetBkMode(hdcBuffer, TRANSPARENT);
    SetTextColor(hdcBuffer, RGB(255, 255, 0));

    RECT textRect = rect;
    DrawText(hdcBuffer, "Enter  a   key", -1, &textRect,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SIZE textSize;
    GetTextExtentPoint32(hdcBuffer, "Enter\ta\tkey", 11, &textSize);
    char dots[4] = "";
    for (int i = 0; i < gGame.settingsState.dotAnimation.dotCount; i++) {
        strcat(dots, ".");
    }

    RECT dotsRect;
    dotsRect.left = rect.left + (rect.right - rect.left) / 2 + textSize.cx / 2;
    dotsRect.right = rect.right;
    dotsRect.top = rect.top;
    dotsRect.bottom = rect.bottom;

    DrawText(hdcBuffer, dots, -1, &dotsRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}



