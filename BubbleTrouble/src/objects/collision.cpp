#include "resources.h"
#include "gameContext.h"
#include "game.h"
#include <windows.h>

// Moved from input.cpp
bool AABB(float ax, float ay, float aw, float ah,
          float bx, float by, float bw, float bh)
{
    return (ax + aw > bx) &&
           (ax < bx + bw) &&
           (ay + ah > by) &&
           (ay < by + bh);
}

// Moved from input.cpp
void CheckHeroDoorCollision()
{
    if(!CURRENT_LEVEL.door.active) return;

    StaticObject* d = &CURRENT_LEVEL.door;
    Hero* h = &gGame.hero;

    if (d->width <= 0 || d->height <= 0)
        return;

    if (AABB(h->x, h->y, h->width, h->height,
             d->x, d->y, d->width, d->height))
    {
        if (h->x < d->x)
            h->x = d->x - h->width;
        else
            h->x = d->x + d->width;
        return;
    }
}

// Moved from input.cpp
void CheckHeroPillarCollision(StaticObject* pillar)
{
    if(pillar->width <= 0 || pillar->height <= 0) return;

    Hero* h = &gGame.hero;

    if (AABB(h->x, h->y, h->width, h->height,
             pillar->x, pillar->y, pillar->width, pillar->height))
    {
        if (h->x < pillar->x)
            h->x = pillar->x - h->width;
        else
            h->x = pillar->x + pillar->width;
        return;
    }
}
