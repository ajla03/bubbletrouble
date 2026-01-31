#include "gameContext.h"

// GLOBAL GAME CONTEXT
GameContext gGame = {
    // ===== GAME OBJEKTI =====
    /* harpoon */ {},
    /* hero */ {},

    /* leftWall */ {},
    /* rightWall */ {},
    /* floorWall */ {},
    /* animatedWall */ {},

    /* backgroundInfo */ {},
    /* levelPlaceholderInfo */ {},

    /* heartInfo */ {},
    /* heartBgInfo */ {},
    /* heartBorderInfo */ {},

    /* balloons */ {},
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
        0,
        maxTime,    // time
        false,
        false,
        GAME_MODE_MENU,
        MAX_LIVES,
        false,
        false
    },
    /* transitionState */ TRANSITION_NONE
};

