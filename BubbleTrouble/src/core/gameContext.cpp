//#include "gameContext.h"

#include "gameContext.h"

// GLOBAL GAME CONTEXT
GameContext gGame = {
    // ===== LEVELS =====
    /* levels */ {},
    /* currentLevel */ 0,

    // ===== GAME OBJEKTI =====
    /* harpoon */ {},
    /* hero */ {},
    /* leftWall */ {},
    /* rightWall */ {},
    /* floorWall */ {},
    /* animatedWall */ {},
    /* levelPlaceholderInfo */ {},
    /* heartInfo */ {},
    /* heartBgInfo */ {},
    /* heartBorderInfo */ {},
    /* hearts */ {},
    /* torchInfo */ {},
    /* animatedWalls */ { 0, 0, 12.0f },

    // ===== UI =====
    /* menuButtons */ {},
    /* restartButtonInfo */ {},
    /* homeButtonInfo */ {},
    /* nextButtonInfo */ {},

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

