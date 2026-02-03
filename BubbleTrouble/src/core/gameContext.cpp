//#include "gameContext.h"

#include "gameContext.h"

// GLOBAL GAME CONTEXT
GameContext gGame = {
    // ===== LEVELS =====
    /* levels */ {},
    /* currentLevel */ 0,
    /* totalScore */ 0,
    /* displayScore */ 0,

    // ===== GAME OBJEKTI =====
    /* harpoon */ {},
    /* hero */ {},
    /* leftWall */ {},
    /* rightWall */ {},
    /* floorWall */ {},
    /* animatedWall */ {},
    /* levelPlaceholderInfo */ {},
    /* scoreHolderInfo */  {},
    /* heartInfo */ {},
    /* heartBgInfo */ {},
    /* heartBorderInfo */ {},
    /* hearts */ {},
    /* torchInfo */ {},
    /* playerHolderInfo */ {},
    /* animatedWalls */ { 0, 0, 12.0f },
    /* logoInfo */ {},

    // ===== UI =====
    /* menuButtons */ {},
    /* restartButtonInfo */ {},
    /* homeButtonInfo */ {},
    /* nextButtonInfo */ {},
    /* pauseButtonInfo */ {},
    /* unpauseButtonInfo */ {},
    // ===== STATE =====
    /* inputState */ {},
    /* gameState */ {
        false,
        false,
        GAME_MODE_MENU,
        MAX_LIVES,
        false,
        false
    },
    /* transitionState */ TRANSITION_NONE
};

