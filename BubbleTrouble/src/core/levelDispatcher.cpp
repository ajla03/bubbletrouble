#include "game.h"
#include "gameContext.h"

void RenderLevel(HDC hdcBuffer, RECT rect){
  switch (gGame.currentLevel) {
    case 0:
        RenderLevel1(hdcBuffer, rect);
        break;
    case 1:
        RenderLevel2(hdcBuffer, rect);
        break;
    }
}
