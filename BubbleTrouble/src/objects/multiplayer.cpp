#include "game.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "constants.h"
#include "resources.h"
#include <windows.h>
#include <cmath>

void InitMultiplayer(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    // Player 1 - lijeva strana
    gGame.hero.x = gGame.leftWall.width + 50;
    gGame.hero.y = rect.bottom - gGame.floorWall.height - gGame.hero.height;
    gGame.hero.currentRow = 2;
    gGame.hero.currentFrame = 0;

    // Player 2 - desna strana
    gGame.hero2.width = gGame.hero.width;
    gGame.hero2.height = gGame.hero.height;
    gGame.hero2.dx = gGame.hero.dx;
    gGame.hero2.dy = gGame.hero.dy;
    gGame.hero2.x = rect.right - gGame.rightWall.width - gGame.hero2.width - 50;
    gGame.hero2.y = rect.bottom - gGame.floorWall.height - gGame.hero2.height;
    gGame.hero2.currentRow = 2;
    gGame.hero2.currentFrame = 0;
    gGame.hero2.animCounter = 0;
    gGame.hero2.heroHitCooldown = 0.0f;

    // Inicijalizuj oba harpuna
    gGame.harpoon.isActive = false;
    gGame.harpoon.length = 0;

    gGame.harpoon2.width = gGame.harpoon.width;
    gGame.harpoon2.height = gGame.harpoon.height;
    gGame.harpoon2.dy = gGame.harpoon.dy;
    gGame.harpoon2.isActive = false;
    gGame.harpoon2.length = 0;

    gGame.gameState.isMultiplayer = true;
    gGame.inputState.wasSpacePressedP2 = false;

    // ← DODAJ OVO NA KRAJ
    // Initialize player stats
    gGame.player1Stats.lives = START_LIVES;
    gGame.player1Stats.score = 0;
    gGame.player1Stats.displayScore = 0;
    gGame.player1Stats.isAlive = true;
    for(int i = 0; i < 5; i++) {
        gGame.player1Stats.hearts[i].currentFrame = 0;
        gGame.player1Stats.hearts[i].animCounter = 0;
    }

    gGame.player2Stats.lives = START_LIVES;
    gGame.player2Stats.score = 0;
    gGame.player2Stats.displayScore = 0;
    gGame.player2Stats.isAlive = true;
    for(int i = 0; i < 5; i++) {
        gGame.player2Stats.hearts[i].currentFrame = 0;
        gGame.player2Stats.hearts[i].animCounter = 0;
    }
}

void UpdatePlayer2Input(HWND hwnd) {
    // PROMJENA: Ako Player 2 nema života, nema kontrole
    if (gGame.player2Stats.lives <= 0) return;

    if(gGame.gameState.currentMode != GAME_MODE_PLAYING ||
       gGame.transitionState.transitionVars == TRANSITION_WAIT ||
       gGame.transitionState.transitionVars == TRANSITION_CLOSING)
        return;

    RECT rect;
    GetClientRect(hwnd, &rect);
    int windowWidth = rect.right;

    bool isMoving = false;
    KeyBindings* keys = &gGame.settingsState.player2Keys;

    // Movement
    if(GetAsyncKeyState(keys->moveLeft) & 0x8000) {
        gGame.hero2.x -= gGame.hero2.dx;
        gGame.hero2.currentRow = 1;
        isMoving = true;
    }
    else if(GetAsyncKeyState(keys->moveRight) & 0x8000) {
        gGame.hero2.x += gGame.hero2.dx;
        gGame.hero2.currentRow = 0;
        isMoving = true;
    }

    // Boundary check
    if (gGame.hero2.x < gGame.leftWall.width) {
        gGame.hero2.x = gGame.leftWall.width;
    }

    int desnaGranica = windowWidth - gGame.rightWall.width - gGame.hero2.width;
    if (gGame.hero2.x > desnaGranica) {
        gGame.hero2.x = desnaGranica;
    }

    bool heroNearLadderP2 = false;
    if (CURRENT_LEVEL.ladder.width > 0) {
        StaticObject& ladder = CURRENT_LEVEL.ladder;
        heroNearLadderP2 =
            (gGame.hero2.x + gGame.hero2.width + 80 > ladder.x) &&
            (gGame.hero2.x < ladder.x + ladder.width + 80);
    }

    // Door collision
    if (!heroNearLadderP2) {
        CheckHeroDoorCollisionP2();
    }

    // Pillar collision
    CheckHeroPillarCollision2(&CURRENT_LEVEL.pillar1);
    CheckHeroPillarCollision2(&CURRENT_LEVEL.pillar2);

    // Animation
    if(isMoving) {
        gGame.hero2.animCounter++;
        if(gGame.hero2.animCounter > 5) {
            gGame.hero2.currentFrame++;
            if(gGame.hero2.currentFrame > 3) {
                gGame.hero2.currentFrame = 0;
            }
            gGame.hero2.animCounter = 0;
        }
    } else {
        gGame.hero2.currentRow = 2;
        gGame.hero2.currentFrame = 0;
    }

    // Harpoon shooting
    bool isSpacePressed = (GetAsyncKeyState(keys->shoot) & 0x8000) != 0;

    if (isSpacePressed && !gGame.inputState.wasSpacePressedP2 && !gGame.harpoon2.isActive) {
        gGame.harpoon2.isActive = true;
        gGame.harpoon2.length = 0;
        gGame.harpoon2.x = gGame.hero2.x + (gGame.hero2.width / 2) - (gGame.harpoon2.width / 2);
        gGame.harpoon2.y = (gGame.hero2.floorY == 0) ? (rect.bottom - gGame.floorWall.height) : (gGame.hero2.floorY + gGame.hero2.height);
        gGame.harpoon2.ownerPlayer = 2;

        if(gGame.gameState.currentMode == GAME_MODE_PLAYING &&
           gGame.settingsState.soundState.soundEffectsOn)
            PlaySound(MAKEINTRESOURCE(IDR_HARPOON_SOUND), GetModuleHandle(NULL),
                  SND_RESOURCE | SND_ASYNC);
    }

    gGame.inputState.wasSpacePressedP2 = isSpacePressed;
}

void RenderHero(
    HDC hdc,
    Hero* hero,
    HBITMAP heroBitmap,
    HBITMAP heroMask,
    RECT rect,
    bool useStretch,
    float scale
) {
    int srcX = hero->currentFrame * hero->width;
    int srcY = hero->currentRow   * hero->height;

    // === BLINK LOGIC ===
    if(hero->heroHitCooldown > 0){
        if((int)(hero->blinkTimer * 30) % 2 == 0)
            return;
    }

    int drawW = hero->width;
    int drawH = hero->height;

    if (useStretch) {
        drawW = (int)(hero->width  * scale);
        drawH = (int)(hero->height * scale);
    }

// Ako floorY nije specifično definisan (za ostale levele), spusti heroja na glavno dno
    if (hero->floorY == 0) {
        hero->y = rect.bottom - gGame.floorWall.height - drawH;
    } else {
        // Za Level 7: Zadrži heroja na definisanom spratu i prilagodi visinu zbog skaliranja slike
        hero->y = hero->floorY - (drawH - hero->height);
    }

    // === MASK ===
    SelectObject(gRes.hdcMem, heroMask);
    if (useStretch) {
        StretchBlt(
            hdc,
            hero->x, hero->y,
            drawW, drawH,
            gRes.hdcMem,
            srcX, srcY,
            hero->width, hero->height,
            SRCAND
        );
    } else {
        BitBlt(
            hdc,
            hero->x, hero->y,
            drawW, drawH,
            gRes.hdcMem,
            srcX, srcY,
            SRCAND
        );
    }

    // === SPRITE ===
    SelectObject(gRes.hdcMem, heroBitmap);
    if (useStretch) {
        StretchBlt(
            hdc,
            hero->x, hero->y,
            drawW, drawH,
            gRes.hdcMem,
            srcX, srcY,
            hero->width, hero->height,
            SRCPAINT
        );
    } else {
        BitBlt(
            hdc,
            hero->x, hero->y,
            drawW, drawH,
            gRes.hdcMem,
            srcX, srcY,
            SRCPAINT
        );
    }
}



void UpdateHarpoon2(HWND hwnd) {
    if(!gGame.harpoon2.isActive) return;

    RECT rect;
    GetClientRect(hwnd, &rect);

    gGame.harpoon2.length += gGame.harpoon2.dy;

    int baseY = (gGame.hero2.floorY == 0) ? (rect.bottom - gGame.floorWall.height) : (gGame.hero2.floorY + gGame.hero2.height);
    gGame.harpoon2.y = baseY;

    int maxLength = baseY;

    if (gGame.hero2.floorY == 0 && CURRENT_LEVEL.door.active && CURRENT_LEVEL.door.width > 0) {
        if (gGame.harpoon2.x + gGame.harpoon2.width > CURRENT_LEVEL.door.x &&
            gGame.harpoon2.x < CURRENT_LEVEL.door.x + CURRENT_LEVEL.door.width) {

            int platformBottom = CURRENT_LEVEL.door.y + CURRENT_LEVEL.door.height;
            maxLength = baseY - platformBottom;
        }
    }

    if (gGame.harpoon2.length >= maxLength) {
        gGame.harpoon2.isActive = false;
    }
}

void CheckHeroDoorCollisionP2() {
    if(!CURRENT_LEVEL.door.active) return;

    StaticObject* d = &CURRENT_LEVEL.door;
    Hero* h = &gGame.hero2;

    if (d->width <= 0 || d->height <= 0)
        return;

    if (AABB(h->x, h->y, h->width, h->height,
             d->x, d->y, d->width, d->height)) {
        if (h->x < d->x)
            h->x = d->x - h->width;
        else
            h->x = d->x + d->width;
    }
}

void CheckHeroPillarCollision2(StaticObject* pillar) {
    if(pillar->width <= 0 || pillar->height <= 0) return;

    Hero* h = &gGame.hero2;

    if (AABB(h->x, h->y, h->width, h->height,
             pillar->x, pillar->y, pillar->width, pillar->height)) {
        if (h->x < pillar->x)
            h->x = pillar->x - h->width;
        else
            h->x = pillar->x + pillar->width;
    }
}

void UpdateHeroCoolDownP2(float dt){
    if(gGame.hero2.heroHitCooldown > 0){
        gGame.hero2.heroHitCooldown -=dt;
        gGame.hero2.blinkTimer+=dt;
    }

    if(gGame.hero2.heroHitCooldown <=0){
        gGame.hero2.heroHitCooldown = 0;
        gGame.hero2.blinkTimer = 0;
    }
}
