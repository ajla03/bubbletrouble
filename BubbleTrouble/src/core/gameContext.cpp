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
    /* harpoon2 */       {},
    /* hero2 */          {},
    /* leftWall */      {},
    /* rightWall */     {},
    /* floorWall */     {},
    /* animatedWall */  {},
    /* levelPlaceholderInfo */ {},
    /* playerHolderInfo */     {},
    /* scoreHolderInfo */      {},
    /* player2HolderInfo */    {},
    /* score2HolderInfo */     {},
    /* heartInfo */     {},
    /* heartBgInfo */   {},
    /* heartBorderInfo */ {},
    /* hearts */        {},
    /* torchInfo */     {},
    /* animatedWalls */ { 0, 0, 12.0f },
    /* logoInfo */      {},

    // === PLAYER STATS ===
    /* player1Stats */ {
        START_LIVES,  // lives
        0,            // score
        0,            // displayScore
        true,         // isAlive
        {}            // hearts animation
    },
    /* player2Stats */ {
        START_LIVES,  // lives
        0,            // score
        0,            // displayScore
        true,         // isAlive
        {}            // hearts animation
    },

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
    /* settingsMusicButtonInfo */ {},
    /* player1 */       {},
    /* player2 */       {},
    /* dashboardButtoInfo*/  {},
    /* helpIconRect */  {},
    /* helpIconHovered */ false,

    // ===== STATE =====
    /* inputState */    {false,false},
    /* gameState */     {
        false,              // gameActive
        false,              // isPaused
        GAME_MODE_MENU,     // currentGameMode
        false,              // showPauseMenu
        false,              // showGameOverMenu
        false,              // showVictoryMenu
        0,                  // menuSelection
        false,              // showNextButton
        0,                  // (reserved)
        false,              // (reserved)
        0,                   // (reserved)
        false               // isMultiplayer
    },
    /* transitionState */ TRANSITION_NONE,
    /* settingsState */ {},
    /* loadingState */ {
        {},      // bubbles
        0,       // progress
        0,       // startTime
        false,   // isComplete
        0.0f, 0.0f, // ballX, ballY
        0.0f,       // ballVy
        false,      // ballIsFalling
        false,      // ballPopped
        0.0f,       // harpoonHeight
        false,
        0.0f
    }
};
