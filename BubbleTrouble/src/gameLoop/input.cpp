#include "resources.h"
#include "gameContext.h"
#include "constants.h"
#include "game.h"
#include <windows.h>
#include "resourceManager.h"

void CheckInputs(HWND hwnd){
    if(gGame.gameState.currentMode != GAME_MODE_PLAYING
        || gGame.transitionState== TRANSITION_WAIT
        || gGame.transitionState == TRANSITION_CLOSING)
        return;

    RECT rect;
    GetClientRect(hwnd, &rect);
    int windowWidth = rect.right;

    bool p1Active = !gGame.gameState.isMultiplayer || gGame.player1Stats.lives > 0;

    if (p1Active) {
        bool isMoving = false;
        KeyBindings* keys = &gGame.settingsState.player1Keys;

        // === MOVEMENT ===
        if(GetAsyncKeyState(keys->moveLeft) & 0x8000){
            gGame.hero.x -= gGame.hero.dx;
            gGame.hero.currentRow = 1;
            isMoving = true;
        }
        else if(GetAsyncKeyState(keys->moveRight) & 0x8000){
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
        bool isSpacePressed = (GetAsyncKeyState(keys->shoot) & 0x8000) != 0;

        if (isSpacePressed && !gGame.inputState.wasSpacePressed && !gGame.harpoon.isActive) {
            gGame.harpoon.isActive = true;
            gGame.harpoon.length = 0;
            gGame.harpoon.x = gGame.hero.x + (gGame.hero.width / 2) - (gGame.harpoon.width / 2);
            gGame.harpoon.y = rect.bottom - gGame.floorWall.height;
            gGame.harpoon.ownerPlayer = 1;

            if(gGame.gameState.currentMode == GAME_MODE_PLAYING && gGame.settingsState.soundState.soundEffectsOn)
                PlaySound(MAKEINTRESOURCE(IDR_HARPOON_SOUND), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
        }

        gGame.inputState.wasSpacePressed = isSpacePressed;
    }

    // === PLAYER 2 INPUT ===
    if(gGame.gameState.isMultiplayer) {
        UpdatePlayer2Input(hwnd);
    }
}


void HandleMouseClick(HWND hwnd, int mx, int my)
{
    if(gGame.transitionState != TRANSITION_NONE)
        return;

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
        if(gGame.settingsState.waitingForKey != KEYBIND_NONE)
            return;
        HandleSettingsClick(hwnd, mx, my);
        return;
    }
}

void HandleSettingsClick(HWND hwnd, int mx, int my){

   // Sound Button
   if(IsPointInButton(gGame.settingsSoundButtonInfo, mx, my)){
    gGame.settingsState.soundState.soundEffectsOn = !gGame.settingsState.soundState.soundEffectsOn;
    return;
   }

   // Music Button
   if(IsPointInButton(gGame.settingsMusicButtonInfo, mx, my)){
        gGame.settingsState.soundState.bgMusicOn = !gGame.settingsState.soundState.bgMusicOn;
        if(gGame.settingsState.soundState.bgMusicOn){
            mciSendString("play bgMusic from 0 notify", NULL, 0, hwnd);
        } else {
            mciSendString("pause bgMusic", NULL, 0, NULL);
        }
    return;
   }

    // Back button
    if(IsPointInButton(gGame.backButtonInfo, mx, my)){
        gGame.settingsState.currentHeroSelected = gRes.characterMask;
        gGame.settingsState.currentPlayerBinding = 1;
        gGame.gameState.pendingHome = true;
        StartWallTransition(hwnd);
        return;
    }

    // Player selection
    if(IsPointInButton(gGame.player1, mx, my)){
        gGame.settingsState.currentHeroSelected = gRes.characterMask;
        gGame.settingsState.currentPlayerBinding = 1;
        return;
    }

    if(IsPointInButton(gGame.player2, mx, my)){
        gGame.settingsState.currentHeroSelected = gRes.hero2Mask;
        gGame.settingsState.currentPlayerBinding = 2;
        return;
    }

    // Key binding buttons
    if (IsPointInButton(gGame.settingsState.leftKeyButton, mx, my)) {
        gGame.settingsState.waitingForKey = KEYBIND_LEFT;
        return;
    }

    if (IsPointInButton(gGame.settingsState.spaceKeyButton, mx, my)) {
        gGame.settingsState.waitingForKey = KEYBIND_SHOOT;
        return;
    }

    if (IsPointInButton(gGame.settingsState.rightKeyButton, mx, my)) {
        gGame.settingsState.waitingForKey = KEYBIND_RIGHT;
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
        gGame.settingsState.soundState.soundEffectsOn = !gGame.settingsState.soundState.soundEffectsOn;
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
        CheckHover(gGame.settingsMusicButtonInfo, x, y);
        CheckHover(gGame.player1, x, y);
        CheckHover(gGame.player2, x, y);
        CheckHover(gGame.settingsSoundButtonInfo, x, y);

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

void HandleKeyDown(HWND hwnd, WPARAM wParam)
{
    int vkCode = (int)wParam;

    if (gGame.gameState.currentMode == GAME_MODE_SETTINGS &&
        gGame.settingsState.waitingForKey != KEYBIND_NONE)
    {
        // ESC ili ENTER otkazuju key binding
        if (vkCode == VK_ESCAPE || vkCode == VK_RETURN) {
            gGame.settingsState.waitingForKey = KEYBIND_NONE;
            return;
        }

        KeyBindings* keys = (gGame.settingsState.currentPlayerBinding == 1) ? &gGame.settingsState.player1Keys
                                                                            : &gGame.settingsState.player2Keys;


       // sanity check - ako smo odabrali neki key za neku akciju vec abortaj promjenu
        if(vkCode == keys->moveLeft ||
           vkCode == keys->moveRight ||
           vkCode == keys->shoot)
        {
            gGame.settingsState.waitingForKey = KEYBIND_NONE;
            return;
        }

        switch (gGame.settingsState.waitingForKey) {
            case KEYBIND_LEFT:
                keys->moveLeft = vkCode;
                break;
            case KEYBIND_RIGHT:
                keys->moveRight = vkCode;
                break;
            case KEYBIND_SHOOT:
                keys->shoot = vkCode;
                break;
        }

        gGame.settingsState.waitingForKey = KEYBIND_NONE;
        return;
    }

    // === NORMAL ESC HANDLING ===
    if (vkCode == VK_ESCAPE) {
        gGame.gameState.currentMode = GAME_MODE_MENU;
        InitializeMenu(hwnd);
        return;
    }
}
