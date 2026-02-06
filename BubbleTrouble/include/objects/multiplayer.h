#pragma once
#include <windows.h>
#include "types.h"

void InitMultiplayer(HWND hwnd);
void UpdatePlayer2Input(HWND hwnd);
void UpdateHarpoon2(HWND hwnd);
void CheckHeroDoorCollisionP2();
void CheckHeroPillarCollision2(StaticObject* pillar);
void UpdateHeroCoolDownP2(float dt);
