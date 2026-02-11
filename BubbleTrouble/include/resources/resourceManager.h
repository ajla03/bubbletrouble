#pragma once
#include <windows.h>

struct ResourceManager{
    HBITMAP character, characterMask;
    HBITMAP arrow, arrowMask;
    HBITMAP wall, wall1, wall2;
    HBITMAP background;
    HBITMAP torch, torchMask;
    HBITMAP levelPlaceholderBlack, levelPlaceholderWhite;
    HBITMAP scoreHolder;
    HBITMAP heart, heartMask;
    HBITMAP heartBkg, heartBkgMask;
    HBITMAP heartBorder, heartBorderMask;
    HBITMAP menuScreen, menuCharacter, menuCharacterMask;
    HBITMAP hButtonsHolder, hButtonsHolderMask;
    HBITMAP restartButton, restartButtonMask;
    HBITMAP homeButton, homeButtonMask;
    HBITMAP nextButton, nextButtonMask;
    HBITMAP pauseButton, pauseButtonMask;
    HBITMAP unpauseButton, unpauseButtonMask;
    HBITMAP soundButton, soundButtonMask;
    HBITMAP settingsSoundOff, settingsSoundOffHover;
    HBITMAP settingsMusicOnHover;
    HBITMAP musicOff, musicOffHover;
    HBITMAP hMenuButton, hMenuButtonMask;
    HBITMAP player1MenuChar, player1MenuCharMask;
    HBITMAP player2MenuChar, player2MenuCharMask;
    HBITMAP settingsMenuChar, settingsMenuCharMask;
    HBITMAP hYellowButton, hYellowButtonMask;
    HBITMAP hGreenButton, hGreenButtonMask;
    HBITMAP hMusicHolder, hMusicHolderMask;
    HBITMAP soundOn;
    HBITMAP controlsHolder, controlsHolderMask;
    HBITMAP settingsPlayer, settingsPlayerMask;
    HBITMAP playerHover, soundOnHover;
    HBITMAP backButton, backButtonMask;
    HBITMAP hGreyButton, hGreyButtonMask;
    HBITMAP hBgLevel2, hBgLevel3;
    HBITMAP longWall;
    HBITMAP doorClosed, doorClosedMask, door, doorMask;
    HCURSOR gameCursor;
    HBITMAP logo, logoMask;
    HDC hdcMem, hdcBuffer;
    HBITMAP hbmBuffer, hOldBufferBmp;
    HBITMAP lifePowerup, lifePowerupMask;
    HBITMAP timePowerup, timePowerupMask;
    HBITMAP freezePowerup, freezePowerupMask;
    HBITMAP hero2, hero2Mask;
    HICON hIcon;
    HBITMAP help;
    HBITMAP helpHover;
    HBITMAP podium, podiumHover;
    HBITMAP singleScoreHolder, multiScoreHolder;
    HBITMAP singleplayerBanner, multiplayerBanner;
    HBITMAP loginBg, loginPopup, loginButton;

    HFONT hFont, hFontTitle, hFontTable;
    HANDLE hFontRes;

    void Init(HDC hdc, HWND hwnd);
    void UpdateDC(HDC hdc, HWND hwnd);
    bool LoadAll(HINSTANCE hInstance);
    void ReleaseAll();

};

extern ResourceManager gRes;
