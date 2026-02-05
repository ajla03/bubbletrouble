#include "gameContext.h"
// GLOBAL GAME CONTEXT
GameContext gGame = {
    // ===== LEVELS =====
    /* levels */        {},
    /* currentLevel */  0,
    /* totalScore */    0,
    /* displayScore */  0,

    // ===== GAME OBJEKTI =====
    /* harpoon */       {},
    /* hero */          {},
    /* leftWall */      {},
    /* rightWall */     {},
    /* floorWall */     {},
    /* animatedWall */  {},
    /* levelPlaceholderInfo */ {},
    /* playerHolderInfo */     {},
    /* scoreHolderInfo */      {},
    /* heartInfo */     {},
    /* heartBgInfo */   {},
    /* heartBorderInfo */ {},
    /* hearts */        {},
    /* torchInfo */     {},
    /* animatedWalls */ { 0, 0, 12.0f },
    /* logoInfo */      {},

    // ===== UI =====
    /* menuButtons */   {},
    /* restartButtonInfo */  {},
    /* homeButtonInfo */     {},
    /* nextButtonInfo */     {},
    /* pauseButtonInfo */    {},
    /* unpauseButtonInfo */  {},
    /* soundButtonInfo */    {},
    /* backButtonInfo */     {},
    /* settingsSoundButtonInfo */ {},
    /* player1 */       {},
    /* player2 */       {},

    // ===== STATE =====
    /* inputState */    {},
    /* gameState */     {
        false,              // gameActive
        false,              // isPaused
        GAME_MODE_MENU,     // currentGameMode
        START_LIVES,        // lives
        false,              // showPauseMenu
        false,              // showGameOverMenu
        false,              // showVictoryMenu
        0,                  // menuSelection
        false,              // showNextButton
        0,                  // (reserved)
        false,              // (reserved)
        0                   // (reserved)
    },
    /* transitionState */ TRANSITION_NONE,
    /* settingsState */ {}
};
