#include "resources.h"
#include "gameContext.h"
#include "constants.h"
#include "game.h"
#include <windows.h>
#include "resourceManager.h"

static bool CanProgressInput();
static void UpdatePlayer1Input(HWND);
static void SaveLoginInfo(HWND);

void CheckInputs(HWND hwnd){

    if(!CanProgressInput())
        return;

    bool p1Active = !gGame.gameState.isMultiplayer || gGame.player1Stats.lives > 0;

    // ===== PLAYER 1 INPUT ====
    if (p1Active){
        UpdatePlayer1Input(hwnd);
    }

    // === PLAYER 2 INPUT ===
    if(gGame.gameState.isMultiplayer) {
        UpdatePlayer2Input(hwnd);
    }
}

static bool CanProgressInput() {
    return gGame.gameState.currentMode == GAME_MODE_PLAYING &&
           gGame.transitionState.transitionVars == TRANSITION_NONE
           || gGame.transitionState.transitionVars == TRANSITION_SOON_TO_END;
}

static void UpdatePlayer1Input(HWND hwnd){
    RECT rect;
    GetClientRect(hwnd, &rect);
    int windowWidth = rect.right;

    bool isMoving = false;
    KeyBindings* keys = &gGame.settingsState.player1Keys;

    bool heroNearLadder = false;
    if (CURRENT_LEVEL.ladder.width > 0) {
        StaticObject& ladder = CURRENT_LEVEL.ladder;
        int heroCenterX = gGame.hero.x + (gGame.hero.width / 2);
        heroNearLadder = (heroCenterX > ladder.x) && (heroCenterX < ladder.x + ladder.width);
    }

    bool isMidAirOnLadder = false;
    if (CURRENT_LEVEL.ladder.width > 0 && CURRENT_LEVEL.door.active) {
        int platformY = CURRENT_LEVEL.door.y;
        int upperFloorY = platformY - gGame.hero.height;
        isMidAirOnLadder = (gGame.hero.floorY != 0 && gGame.hero.floorY != upperFloorY);
    }

    if (!isMidAirOnLadder) {
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

        if (gGame.hero.x < gGame.leftWall.width) {
            gGame.hero.x = gGame.leftWall.width;
        }

        int desnaGranica = windowWidth - gGame.rightWall.width - gGame.hero.width;
        if (gGame.hero.x > desnaGranica) {
            gGame.hero.x = desnaGranica;
        }
    }

    if (!heroNearLadder) {
        CheckHeroDoorCollision();
    }

    if(gGame.currentLevel == 3){
        CheckHeroPillarCollision(&CURRENT_LEVEL.pillar1);
        CheckHeroPillarCollision(&CURRENT_LEVEL.pillar2);
    }

    if(CURRENT_LEVEL.ladder.width > 0){
        CheckHeroPillarCollision(&CURRENT_LEVEL.longWall);
        CheckHeroPillarCollision(&CURRENT_LEVEL.pillar1);
        CheckHeroPillarCollision(&CURRENT_LEVEL.pillar2);
    }

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

    bool isSpacePressed = (GetAsyncKeyState(keys->shoot) & 0x8000) != 0;

    if (isSpacePressed && !gGame.inputState.wasSpacePressed && !gGame.harpoon.isActive && !isMidAirOnLadder) {
        gGame.harpoon.isActive = true;
        gGame.harpoon.length = 0;
        gGame.harpoon.x = gGame.hero.x + (gGame.hero.width / 2) - (gGame.harpoon.width / 2);
        gGame.harpoon.y = (gGame.hero.floorY == 0) ? (rect.bottom - gGame.floorWall.height) : (gGame.hero.floorY + gGame.hero.height);
        gGame.harpoon.ownerPlayer = 1;

        if(gGame.gameState.currentMode == GAME_MODE_PLAYING && gGame.settingsState.soundState.soundEffectsOn)
            PlaySound(MAKEINTRESOURCE(IDR_HARPOON_SOUND), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
    }

    gGame.inputState.wasSpacePressed = isSpacePressed;

    if (CURRENT_LEVEL.ladder.width > 0) {
        int platformY   = CURRENT_LEVEL.door.y;
        int lowerFloorY = rect.bottom - gGame.floorWall.height - gGame.hero.height;
        int upperFloorY = platformY - gGame.hero.height;

        if (heroNearLadder) {
            if (gGame.hero.floorY == 0) {
                gGame.hero.floorY = lowerFloorY;
            }

            if (GetAsyncKeyState(VK_UP) & 0x8000) {
                gGame.hero.floorY -= 5;
                if (gGame.hero.floorY < upperFloorY) {
                    gGame.hero.floorY = upperFloorY;
                }
            }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                gGame.hero.floorY += 5;
                if (gGame.hero.floorY > lowerFloorY) {
                    gGame.hero.floorY = lowerFloorY;
                }
            }

            if (gGame.hero.floorY >= lowerFloorY) {
                gGame.hero.floorY = 0;
            } else if (gGame.hero.floorY <= upperFloorY) {
                gGame.hero.floorY = upperFloorY;
            }
        }
    }
}

void HandleMouseClick(HWND hwnd, int mx, int my)
{
    if (gGame.transitionState.transitionVars != TRANSITION_NONE)
        return;

    const auto& mode = gGame.gameState.currentMode;

    if (gGame.gameState.isLevelCleared) {
        HandleEndScreenClick(hwnd, mx, my);
        return;
    }

    switch (mode)
    {
        case GAME_MODE_LOGIN:
            if (IsPointInButton(gGame.loginButtonInfo, mx, my)) {
                SaveLoginInfo(hwnd);
            }
            break;

        case GAME_MODE_DASHBOARD:
            if (IsPointInButton(gGame.backButtonInfo, mx, my)) {
                gGame.transitionState.pendingHome = true;
                StartWallTransition(hwnd);
            }
            break;

        case GAME_OVER:
            HandleEndScreenClick(hwnd, mx, my);
            break;

        case GAME_MODE_MENU:
            HandleMenuClick(hwnd, mx, my);
            break;

        case GAME_MODE_PLAYING:
            HandlePlayingClick(hwnd, mx, my);
            break;

        case GAME_MODE_PAUSE:
            HandlePauseClick(hwnd, mx, my);
            break;

        case GAME_MODE_SETTINGS:
            if (gGame.settingsState.waitingForKey == KEYBIND_NONE) {
                HandleSettingsClick(hwnd, mx, my);
            }
            break;

        case GAME_MODE_HELP:
            HandleHelpClick(hwnd, mx, my);
            break;

        default:
            break;
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
        gGame.transitionState.pendingHome = true;
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


void HandleHelpClick(HWND hwnd, int mx, int my) {
    // Back button logika (koristimo istu logiku tranzicije kao u settings)
    if(IsPointInButton(gGame.backButtonInfo, mx, my)){
        gGame.transitionState.pendingHome = true; // Ovo signalizira povratak u Menu
        StartWallTransition(hwnd);
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
        gGame.transitionState.pendingHome = true;
        StartWallTransition(hwnd);
    }
}

void HandleEndScreenClick(HWND hwnd, int mx, int my)
{
    if (IsPointInButton(gGame.restartButtonInfo, mx, my))
    {
        gGame.transitionState.pendingRestart = true;
        StartWallTransition(hwnd);
        return;
    }

    if (IsPointInButton(gGame.homeButtonInfo, mx, my))
    {
        gGame.transitionState.pendingHome = true;
        StartWallTransition(hwnd);
        return;
    }

    if (gGame.gameState.isLevelCleared &&
        IsPointInButton(gGame.nextButtonInfo, mx, my))
    {
        gGame.transitionState.pendingNextLevel = true;
        StartWallTransition(hwnd);
    }
}


void HandleMouseMove(HWND hwnd, int x, int y)
{

    // LOGIN
    if(gGame.gameState.currentMode == GAME_MODE_LOGIN){
        CheckHover(gGame.loginButtonInfo, x, y);
        return;
    }

    // DASHBOARD
    if(gGame.gameState.currentMode == GAME_MODE_DASHBOARD){
        CheckHover(gGame.backButtonInfo,x, y);
        return;
    }

    // MENU
    if (gGame.gameState.currentMode == GAME_MODE_MENU)
    {
        CheckHover(gGame.dashboardButtonInfo, x, y);
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
    if (gGame.gameState.currentMode == GAME_MODE_HELP)
    {
        CheckHover(gGame.backButtonInfo, x, y);
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

bool IsPointInButton(const Button& btn, int x, int y)
{
    return x >= btn.x &&
           x <= btn.x + btn.width &&
           y >= btn.y &&
           y <= btn.y + btn.height;
}

void CheckHover(Button& button, int mx, int my)
{
    if (mx >= button.x && mx <= button.x + button.width &&
        my >= button.y && my <= button.y + button.height)
    {
        button.isHover = true;
    }
    else
    {
        button.isHover = false;
    }
}


void HandleCharInput(HWND hwnd, WPARAM wParam)
{
    if (gGame.gameState.currentMode == GAME_MODE_LOGIN &&
        gGame.loginInput.active)
    {
        // BACKSPACE
        if (wParam == VK_BACK)
        {
            if (gGame.loginInput.length > 0)
            {
                gGame.loginInput.length--;
                gGame.loginInput.text[gGame.loginInput.length] = '\0';
            }
        }

        if(wParam == VK_RETURN ){
            SaveLoginInfo(hwnd);
        }
        // Printable ASCII karakteri
        else if (wParam >= 32 && wParam <= 126)
        {
            bool isAllowed = isalnum((unsigned char)wParam) ||
                     (char)wParam == '.' ||
                     (char)wParam == '_';

            if(isAllowed){
                if (gGame.loginInput.length < 11)
                {
                    gGame.loginInput.text[gGame.loginInput.length++] = (char)wParam;
                    gGame.loginInput.text[gGame.loginInput.length] = '\0';
                }
            }
        }
    }
}

static void SaveLoginInfo(HWND hwnd){
    if(gGame.loginInput.length > 0)
    {
        strcpy_s(
            gGame.playerName,
            sizeof(gGame.playerName),
            gGame.loginInput.text
        );

        gGame.transitionState.pendingHome = true;
        StartWallTransition(hwnd);
    }
}
