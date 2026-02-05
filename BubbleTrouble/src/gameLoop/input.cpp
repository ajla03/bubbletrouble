#include "resources.h"
#include "gameContext.h"
#include "constants.h"
#include "game.h"
#include <windows.h>

void CheckInputs(HWND hwnd){
    if(gGame.gameState.currentMode != GAME_MODE_PLAYING || gGame.transitionState== TRANSITION_WAIT || gGame.transitionState == TRANSITION_CLOSING)
        return;

    RECT rect;
    GetClientRect(hwnd, &rect);
    int windowWidth = rect.right;

    bool isMoving = false;

    // === MOVEMENT ===
    if(GetAsyncKeyState(VK_LEFT)){
        gGame.hero.x -= gGame.hero.dx;
        gGame.hero.currentRow = 1;
        isMoving = true;
    }
    else if(GetAsyncKeyState(VK_RIGHT)){
        gGame.hero.x += gGame.hero.dx;
        gGame.hero.currentRow = 0;
        isMoving = true;
    }

    // === BOUNDARY CHECK ===
    if (gGame.hero.x < gGame.leftWall.width) {
        gGame.hero.x = gGame.leftWall.width;
    }

    int desnaGranica = windowWidth - gGame.rightWall.width - gGame.hero.width;
    if (gGame.hero.x > desnaGranica) {
        gGame.hero.x = desnaGranica;
    }

    // ===== CHECK ZA VRATA ===== //
    CheckHeroDoorCollision();

    // ===== CHECK ZA STUBOVE (Level 4) ===== //
    CheckHeroPillarCollision(&CURRENT_LEVEL.pillar1);
    CheckHeroPillarCollision(&CURRENT_LEVEL.pillar2);


    // === ANIMATION ===
    if(isMoving) {
        gGame.hero.animCounter++;
        if(gGame.hero.animCounter > 5) {
            gGame.hero.currentFrame++;
            if(gGame.hero.currentFrame > 3) {
                gGame.hero.currentFrame = 0;
            }
            gGame.hero.animCounter = 0;
        }
    } else {
        gGame.hero.currentRow = 2;
        gGame.hero.currentFrame = 0;
    }

    // === HARPOON SHOOTING ===
    bool isSpacePressed = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

    if (isSpacePressed && !gGame.inputState.wasSpacePressed && !gGame.harpoon.isActive) {
        gGame.harpoon.isActive = true;
        gGame.harpoon.length = 0;
        gGame.harpoon.x = gGame.hero.x + (gGame.hero.width / 2) - (gGame.harpoon.width / 2);
        gGame.harpoon.y = rect.bottom - gGame.floorWall.height;
        if(gGame.gameState.currentMode == GAME_MODE_PLAYING && gGame.soundState.soundEffectsOn)
        PlaySound(MAKEINTRESOURCE(IDR_HARPOON_SOUND), GetModuleHandle(NULL),
              SND_RESOURCE | SND_ASYNC);
    }

    gGame.inputState.wasSpacePressed = isSpacePressed;
}



void HandleMouseClick(HWND hwnd, int mx, int my)
{
    const auto& mode = gGame.gameState.currentMode;

    if (mode == GAME_OVER || gGame.gameState.isLevelCleared)
    {
        HandleEndScreenClick(hwnd, mx, my);
        return;
    }

    if (mode == GAME_MODE_MENU)
    {
        HandleMenuClick(hwnd, mx, my);
        return;
    }

    if (mode == GAME_MODE_PLAYING)
    {
        HandlePlayingClick(hwnd, mx, my);
        return;
    }

    if (mode == GAME_MODE_PAUSE)
    {
        HandlePauseClick(hwnd, mx, my);
        return;
    }

    if(mode == GAME_MODE_SETTINGS){
        HandleSettingsClick(hwnd, mx, my);
        return;
    }
}

void HandleSettingsClick(HWND hwnd, int mx, int my){
    // Back button
    if(IsPointInButton(gGame.backButtonInfo, mx, my)){
        gGame.gameState.currentMode = GAME_MODE_MENU;
        return;
    }

    if(IsPointInButton(gGame.settingsSoundButtonInfo, mx, my)){
        gGame.soundState.bgMusicOn = !gGame.soundState.bgMusicOn;

        if(gGame.soundState.bgMusicOn){
            mciSendString("play bgMusic from 0 notify", NULL, 0, hwnd);
        } else {
            mciSendString("pause bgMusic", NULL, 0, NULL);
        }
        return;
    }
}

void HandleBackClick(HWND hwnd, int mx, int my){
    if(IsPointInButton(gGame.backButtonInfo, mx, my)){
        gGame.gameState.currentMode = GAME_MODE_MENU;
        return;
    }
}

void HandlePlayingClick(HWND hwnd, int mx, int my)
{
    if (IsPointInButton(gGame.pauseButtonInfo, mx, my))
    {
        gGame.gameState.currentMode = GAME_MODE_PAUSE;
        return;
    }

    if (IsPointInButton(gGame.soundButtonInfo, mx, my))
    {
        // Toggle samo sound efekte (ne utice na pozadinsku muziku)
        gGame.soundState.soundEffectsOn = !gGame.soundState.soundEffectsOn;
        return;
    }
}


void HandlePauseClick(HWND hwnd, int mx, int my)
{
    if (IsPointInButton(gGame.unpauseButtonInfo, mx, my))
    {
        gGame.gameState.currentMode = GAME_MODE_PLAYING;
        return;
    }

    if (IsPointInButton(gGame.homeButtonInfo, mx, my))
    {
        gGame.gameState.pendingHome = true;
        StartWallTransition(hwnd);
    }
}

void HandleEndScreenClick(HWND hwnd, int mx, int my)
{
    if (IsPointInButton(gGame.restartButtonInfo, mx, my))
    {
        gGame.gameState.pendingRestart = true;
        StartWallTransition(hwnd);
        return;
    }

    if (IsPointInButton(gGame.homeButtonInfo, mx, my))
    {
        gGame.gameState.pendingHome = true;
        StartWallTransition(hwnd);
        return;
    }

    if (gGame.gameState.isLevelCleared &&
        IsPointInButton(gGame.nextButtonInfo, mx, my))
    {
        gGame.gameState.pendingNextLevel = true;
        StartWallTransition(hwnd);
    }
}


void HandleMouseMove(HWND hwnd, int x, int y)
{
    // MENU
    if (gGame.gameState.currentMode == GAME_MODE_MENU)
    {
        HandleMenuMouseMove(hwnd, x, y);
        return;
    }

    // SETTINGS
    if (gGame.gameState.currentMode == GAME_MODE_SETTINGS)
    {
        CheckHover(gGame.backButtonInfo, x, y);
        CheckHover(gGame.settingsSoundButtonInfo, x, y);
        CheckHover(gGame.player1, x, y);
        CheckHover(gGame.player2, x, y);

        return;
    }

    // GAME OVER ILI LEVEL CLEARED
    if (gGame.gameState.currentMode == GAME_OVER ||
        gGame.gameState.isLevelCleared)
    {
        CheckHover(gGame.homeButtonInfo, x, y);
        CheckHover(gGame.restartButtonInfo, x, y);

        if (gGame.gameState.isLevelCleared)
            CheckHover(gGame.nextButtonInfo, x, y);

        return;
    }

    // PLAYING
    if (gGame.gameState.currentMode == GAME_MODE_PLAYING)
    {
        CheckHover(gGame.pauseButtonInfo, x, y);
        CheckHover(gGame.soundButtonInfo, x, y);
        return;
    }

    // PAUSE
    if (gGame.gameState.currentMode == GAME_MODE_PAUSE)
    {
        CheckHover(gGame.unpauseButtonInfo, x, y);
        CheckHover(gGame.homeButtonInfo, x, y);
        return;
    }


}
