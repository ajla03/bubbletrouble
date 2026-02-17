#include "resources.h"
#include "gameContext.h"
#include "game.h"
#include "resourceManager.h"
#include <windows.h>
#include <iostream>

static void UpdateTimers();
static void UpdateHarpoonHero1(HWND);
static void UpdateTorches();
static void UpdateLevelLogic(HWND hwnd);
static void UpdateHearts();
static void UpdateHeroCoolDown(float dt);
static void UpdateScoreAnimation();
static void UpdateLoginInput(float dt);


void Update(HWND hwnd){
    if(gGame.transitionState.transitionVars != TRANSITION_NONE )
       {
        if(gGame.gameState.currentMode!= GAME_MODE_PAUSE && gGame.gameState.currentMode != GAME_OVER ){
            UpdateTorches();
        }

        if(gGame.transitionState.transitionVars == TRANSITION_SOON_TO_END )
            UpdateBalloons(hwnd);

        UpdateWallTransition(hwnd);
        return;
       }
    else if( gGame.gameState.currentMode != GAME_MODE_PLAYING ) {

            if(gGame.gameState.currentMode == GAME_MODE_LOGIN)
                UpdateLoginInput(0.05f);
            if((gGame.gameState.currentMode == GAME_MODE_SETTINGS && gGame.settingsState.waitingForKey == KEYBIND_NONE ) ||
               gGame.gameState.currentMode == GAME_MODE_HELP  ||
               gGame.gameState.currentMode == GAME_MODE_DASHBOARD)
                UpdateTorches();
            return;
        }

    printf("%d", gGame.gameState.currentMode);
    printf("GDI Objects: %ld\n", GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS));

    // UPDATE TIMERS //
    UpdateTimers();

    // UPDATE HARPOON FOR HERO 1
    UpdateHarpoonHero1(hwnd);

    // UPDATE HARPOON - MULTIPLAYER //
    if(gGame.gameState.isMultiplayer && gGame.harpoon2.isActive) {
        UpdateHarpoon2(hwnd);
    }

    // UPDATE TORCHES //
    UpdateTorches();

    // ==== update aktivnih vrata ====== //
    UpdateLevelLogic(hwnd);

     if(gGame.gameState.hasShield && gGame.gameState.shieldTimeLeft > 0) {
        gGame.gameState.shieldTimeLeft--;
        if(gGame.gameState.shieldTimeLeft <= 0) {
            gGame.gameState.hasShield = false;
        }
    }

    if(gGame.gameState.hasDoubleShot && gGame.gameState.doubleShotTimeLeft > 0) {
        gGame.gameState.doubleShotTimeLeft--;
        if(gGame.gameState.doubleShotTimeLeft <= 0) {
            gGame.gameState.hasDoubleShot = false;
        }
    }

    // UDPATE HERO COOLDOWNS
    UpdateHeroCoolDown(0.016f);

    if(gGame.gameState.isMultiplayer) {
        UpdateHeroCoolDownP2(0.016f);
    }

    // UPDATE BALOONS
    UpdateBalloons(hwnd);

    // UPDATE HEARTS
    UpdateHearts();

    // UPDATE POWERUPS
    UpdatePowerups(hwnd);

    // UPDATE SCORE ANIMATION
     UpdateScoreAnimation();

    // CHECK COLLISIONS
    CheckCollisions();

    // UPDATE WALL TRANSITION
    UpdateWallTransition(hwnd);

}

void UpdateHeroCoolDown(float dt){
    if(gGame.hero.heroHitCooldown > 0){
        gGame.hero.heroHitCooldown -=dt;
        gGame.hero.blinkTimer+=dt;
    }

    if(gGame.hero.heroHitCooldown <=0){
        gGame.hero.heroHitCooldown = 0;
        gGame.hero.blinkTimer = 0;
    }
}

void UpdateHearts() {
    if (gGame.gameState.isMultiplayer) {
        // === PLAYER 1 ANIMATION ===
        for (int i = 0; i < gGame.player1Stats.lives; i++) {
            gGame.player1Stats.hearts[i].animCounter++;
            if (gGame.player1Stats.hearts[i].animCounter >= HEART_ANIM_SPEED) {
                gGame.player1Stats.hearts[i].animCounter = 0;
                gGame.player1Stats.hearts[i].currentFrame++;
                if (gGame.player1Stats.hearts[i].currentFrame >= HEART_FRAMES) {
                    gGame.player1Stats.hearts[i].currentFrame = 0;
                }
            }
        }
        // === PLAYER 2 ANIMATION ===
        for (int i = 0; i < gGame.player2Stats.lives; i++) {
            gGame.player2Stats.hearts[i].animCounter++;
            if (gGame.player2Stats.hearts[i].animCounter >= HEART_ANIM_SPEED) {
                gGame.player2Stats.hearts[i].animCounter = 0;
                gGame.player2Stats.hearts[i].currentFrame++;
                if (gGame.player2Stats.hearts[i].currentFrame >= HEART_FRAMES) {
                    gGame.player2Stats.hearts[i].currentFrame = 0;
                }
            }
        }
    } else {
        // === SINGLE PLAYER (Stari kod) ===
        for (int i = 0; i < gGame.player1Stats.lives; i++) {
            gGame.hearts[i].animCounter++;
            if (gGame.hearts[i].animCounter >= HEART_ANIM_SPEED) {
                gGame.hearts[i].animCounter = 0;
                gGame.hearts[i].currentFrame++;
                if (gGame.hearts[i].currentFrame >= HEART_FRAMES) {
                    gGame.hearts[i].currentFrame = 0;
                }
            }
        }
    }
}

void UpdateScoreAnimation() {
    if (gGame.gameState.isMultiplayer) {
        int diff1 = gGame.player1Stats.score - gGame.player1Stats.displayScore;
        if (diff1 > 0) {
            int step = std::max(1, diff1 / 10);
            gGame.player1Stats.displayScore += step;
        }

        int diff2 = gGame.player2Stats.score - gGame.player2Stats.displayScore;
        if (diff2 > 0) {
            int step = std::max(1, diff2 / 10);
            gGame.player2Stats.displayScore += step;
        }
    } else {
        // Single player
        int diff = gGame.totalScore - gGame.displayScore;
        if (diff > 0) {
            int step = std::max(1, diff / 10);
            gGame.displayScore += step;
        }
    }
}



void UpdateWallTransition(HWND hwnd){
    if(gGame.transitionState.transitionVars == TRANSITION_CLOSING){
        gGame.animatedWalls.wallTopY +=gGame.animatedWalls.wallSpeed;
        gGame.animatedWalls.wallBottomY -=gGame.animatedWalls.wallSpeed;

        if(gGame.animatedWalls.wallTopY + gGame.animatedWall.height >= gGame.animatedWalls.wallBottomY){

            if(gGame.transitionState.pendingRestart){
                ResetGame(hwnd);
            }

            gGame.animatedWalls.transitionWaitStart = GetTickCount();
            gGame.transitionState.transitionVars = TRANSITION_WAIT;
        }
     }else if (gGame.transitionState.transitionVars== TRANSITION_WAIT)
    {
        if (GetTickCount() - gGame.animatedWalls.transitionWaitStart >= 400)
        {
            if(gGame.transitionState.pendingHome){
                if(gGame.gameState.currentMode == GAME_MODE_SETTINGS){
                    gGame.settingsState.currentHeroSelected = gRes.characterMask;
                    gGame.settingsState.currentPlayerBinding = 1;
                }
                gGame.gameState.currentMode = GAME_MODE_MENU;
                gGame.gameState.isGameOver = false;
                gGame.gameState.isLevelCleared = false;
                ResetGame(hwnd);
            }else if(gGame.transitionState.pendingNextLevel){
                if (CURRENT_LEVEL.hdcCache) { printf("DELETED DC"); DeleteDC(CURRENT_LEVEL.hdcCache);}
                if (CURRENT_LEVEL.hStaticCache) DeleteObject(CURRENT_LEVEL.hStaticCache);
                gGame.currentLevel++;
                ResetBetweenLevels(hwnd);
                InitLevel(hwnd);
            }else if(gGame.transitionState.pendingRestart){
                gGame.gameState.currentMode = GAME_MODE_PLAYING;
            }else if(gGame.transitionState.pendingSettings){
                gGame.gameState.currentMode = GAME_MODE_SETTINGS;
                gGame.menuButtons[2].isHovered = false;
            }else if(gGame.transitionState.pendingHelp){
                gGame.gameState.currentMode = GAME_MODE_HELP;
            }else if(gGame.transitionState.pendingDashboard){
                gGame.gameState.currentMode = GAME_MODE_DASHBOARD;
            }else if(gGame.transitionState.pendingSingle){
                StartGame(hwnd);
                gGame.menuButtons[0].isHovered = false;
            }else if(gGame.transitionState.pendingMulti){
                StartGame(hwnd);
                InitMultiplayer(hwnd);
                gGame.menuButtons[1].isHovered = false;
            }
            gGame.transitionState.transitionVars = TRANSITION_OPENING;
        }
    }
     else if (gGame.transitionState.transitionVars == TRANSITION_OPENING)
    {
        gGame.animatedWalls.wallTopY -= gGame.animatedWalls.wallSpeed;
        gGame.animatedWalls.wallBottomY += gGame.animatedWalls.wallSpeed;

        if (gGame.animatedWalls.wallTopY <= -gGame.animatedWall.height / 2 && gGame.transitionState.transitionVars != TRANSITION_SOON_TO_END)
        {
            gGame.transitionState.transitionVars = TRANSITION_SOON_TO_END;
        }
    }
    else if(gGame.transitionState.transitionVars == TRANSITION_SOON_TO_END){

        gGame.animatedWalls.wallTopY -= gGame.animatedWalls.wallSpeed;
        gGame.animatedWalls.wallBottomY += gGame.animatedWalls.wallSpeed;

        if (gGame.animatedWalls.wallTopY <= -gGame.animatedWall.height)
        {

            gGame.transitionState.transitionVars = TRANSITION_NONE;
            gGame.transitionState.pendingHome = false;
            gGame.transitionState.pendingRestart = false;
            gGame.transitionState.pendingNextLevel = false;
            gGame.transitionState.pendingSettings = false;
            gGame.transitionState.pendingDashboard = false;
            gGame.transitionState.pendingHelp = false;
            gGame.transitionState.pendingSingle = false;
            gGame.transitionState.pendingMulti = false;
        }
    }
}

void UpdateLayout(int oldW, int oldH, int newW, int newH)
{
    if (oldW <= 0 || oldH <= 0)
        return;

    int oldBgX = gGame.leftWall.width;
    int oldBgW = oldW - gGame.leftWall.width - gGame.rightWall.width;
    int oldBgH = oldH - gGame.floorWall.height;

    int newBgX = gGame.leftWall.width;
    int newBgW = newW - gGame.leftWall.width - gGame.rightWall.width;
    int newBgH = newH - gGame.floorWall.height;

    float scaleX = (float)newBgW / (float)oldBgW;
    float scaleY = (float)newBgH / (float)oldBgH;

    if (gGame.transitionState.transitionVars != TRANSITION_NONE)
    {
        float globalScaleY = (float)newH / (float)oldH;

        gGame.animatedWalls.wallTopY = (int)(gGame.animatedWalls.wallTopY * globalScaleY);        gGame.animatedWalls.wallBottomY = (int)(gGame.animatedWalls.wallBottomY * scaleY);
        gGame.animatedWalls.wallBottomY = (int)(gGame.animatedWalls.wallBottomY * globalScaleY);
    }
    Level& lvl = gGame.levels[gGame.currentLevel];

    for (int i = 0; i < MAX_BALLOONS; i++)
    {
        if(!lvl.balloons[i].active) continue;

        Balloon& b = lvl.balloons[i];

        int localX = b.x - oldBgX;
        int localY = b.y;

        localX = (int)(localX * scaleX);
        localY = (int)(localY * scaleY);

        b.x = newBgX + localX;
        b.y = localY;
    }

    // UPDATE HERO
    {
        int localX = gGame.hero.x - oldBgX;
        int localY = gGame.hero.y;

        localX = (int)(localX * scaleX);
        localY = (int)(localY * scaleY);

        gGame.hero.x = newBgX + localX;
        gGame.hero.y = localY;

        // opcionalno: update grounded Y
       // gGame.hero.y = newH - gGame.floorWall.height - gGame.hero.height;
    }

    // UPDATE HARPOON

    if (gGame.harpoon.isActive)
    {
        int localX = gGame.harpoon.x - oldBgX;
        int localY = gGame.harpoon.y;

        localX = (int)(localX * scaleX);
        localY = (int)(localY * scaleY);

        gGame.harpoon.x = newBgX + localX;
        gGame.harpoon.y = localY;
    }

}

static void UpdateTimers(){
    if(!gGame.gameState.isGameOver && CURRENT_LEVEL.timeLeft > 0 && !gGame.gameState.isLevelCleared) {
        CURRENT_LEVEL.timeLeft -= 1.0;
    }else if (CURRENT_LEVEL.timeLeft <= 0) {

        CURRENT_LEVEL.timeLeft = 0;
        printf("timeLeft at start: %f\n", CURRENT_LEVEL.timeLeft);
        gGame.gameState.isGameOver = true;
        gGame.gameState.currentMode = GAME_OVER;

        if(!gGame.gameState.isScoreSaved) {
            SaveFinalScore();
            gGame.gameState.isScoreSaved = true;
      }
    }
}

static void UpdateHarpoonHero1(HWND hwnd){
   if (gGame.harpoon.isActive){
        RECT rect;
        GetClientRect(hwnd, &rect);
        gGame.harpoon.length += gGame.harpoon.dy;

        int baseY = (gGame.hero.floorY == 0) ? (rect.bottom - gGame.floorWall.height) : (gGame.hero.floorY + gGame.hero.height);
        gGame.harpoon.y = baseY;

        if (gGame.harpoon.length >= baseY){
            gGame.harpoon.isActive = false;
        }
    }
}

static void UpdateTorches(){
    gGame.torchInfo.animCounter++;
    if(gGame.torchInfo.animCounter > 5){
    gGame.torchInfo.currentFrame++;
    if (gGame.torchInfo.currentFrame >= 4) {
        gGame.torchInfo.currentFrame = 0;
        gGame.torchInfo.currentRow = (gGame.torchInfo.currentRow + 1) % 2;
     }
     gGame.torchInfo.animCounter = 0;
    }
}

static void UpdateLevelLogic(HWND hwnd){
    // === UPDATE DOOR IF LEVEL 3 ===
    if(CURRENT_LEVEL.door.width > 0 &&
       CURRENT_LEVEL.door.height > 0 &&
       gGame.currentLevel == 2){
        float sectionLeft  = gGame.leftWall.width;
        float sectionRight = CURRENT_LEVEL.longWall.x;

        if (AreSectionBalloonsDestroyed(sectionLeft, sectionRight))
        {
            CURRENT_LEVEL.door.active = false;
        }
    }

    // === UPDATE PILLARS - LEVEL 4 ===
    if(gGame.currentLevel == 3) {
        RECT rect;
        GetClientRect(hwnd, &rect);

        // Prvi stub za level 4
        if(CURRENT_LEVEL.pillar1.width > 0) {
            bool balloon0Destroyed = true;
            for(int i = 0; i < MAX_BALLOONS; i++) {
                if(CURRENT_LEVEL.balloons[i].active &&
                   CURRENT_LEVEL.balloons[i].color == RGB(255, 200, 0)) {
                    balloon0Destroyed = false;
                    break;
                }
            }

            if(balloon0Destroyed) {
                CURRENT_LEVEL.pillar1.width = 0;  // Ukloni prvi stub
            }
        }

        // Drugi stub za level 4
        if(CURRENT_LEVEL.pillar2.width > 0) {
            bool balloon1Destroyed = true;

            for(int i = 0; i < MAX_BALLOONS; i++) {
                if(CURRENT_LEVEL.balloons[i].active &&
                   CURRENT_LEVEL.balloons[i].color == RGB(255, 100, 0)) {
                    balloon1Destroyed = false;
                    break;
                }
            }

            if(balloon1Destroyed) {
                CURRENT_LEVEL.pillar2.width = 0;  // Ukloni drugi stub
            }
        }
    }

    // === UPDATE PILLARS - LEVEL 7 ===
    if (CURRENT_LEVEL.ladder.width > 0) {
        // Prvi stub (longWall) nestaje kada pukne prvi balon s lijeva (indeks 0)
        if (CURRENT_LEVEL.longWall.width > 0 && !CURRENT_LEVEL.balloons[0].active) {
            CURRENT_LEVEL.longWall.width = 0;
        }
        // Drugi stub (pillar1) nestaje kada pukne drugi balon (indeks 1)
        if (CURRENT_LEVEL.pillar1.width > 0 && !CURRENT_LEVEL.balloons[1].active) {
            CURRENT_LEVEL.pillar1.width = 0;
        }
        // Treći stub (pillar2) nestaje kada pukne treći balon (indeks 2)
        if (CURRENT_LEVEL.pillar2.width > 0 && !CURRENT_LEVEL.balloons[2].active) {
            CURRENT_LEVEL.pillar2.width = 0;
        }
    }
}

void UpdateLoginInput(float dt)
{
    gGame.loginInput.caretTimer += dt;

    if(gGame.loginInput.caretTimer >= 0.5f)
    {
        gGame.loginInput.caretVisible =
            !gGame.loginInput.caretVisible;

        gGame.loginInput.caretTimer = 0.0f;
    }
}

