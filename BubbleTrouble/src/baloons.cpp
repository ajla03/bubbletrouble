#include "resources.h"
#include <windows.h>
#include <cmath>

void UpdateBalloons(HWND hwnd) {
    if(gameState.isGameOver)
        return;

    RECT rect;
    GetClientRect(hwnd, &rect);

    int FLOOR_Y = rect.bottom - floorWall.height;
    int LEFT_WALL_X = leftWall.width;
    int RIGHT_WALL_X = rect.right - rightWall.width;

    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (!balloons[i].active) continue;

        Balloon* b = &balloons[i];

        //gravitacija
        b->speedY += BALLOON_GRAVITY;
        if(b->speedY > MAX_FALL_SPEED)
            b->speedY = MAX_FALL_SPEED;

        //pomicanje
        b->x += b->speedX;
        b->y += b->speedY;

        //lijevi zid check
        if (b->x - b->radius <= LEFT_WALL_X) {
            b->x = LEFT_WALL_X + b->radius;
            b->speedX = -b->speedX;
        }

        //desni zid check
        if (b->x + b->radius >= RIGHT_WALL_X) {
            b->x = RIGHT_WALL_X - b->radius;
            b->speedX = -b->speedX;
        }

        //pod check
        if (b->y + b->radius >= FLOOR_Y) {
            b->y = FLOOR_Y - b->radius;
            b->speedY = -b->bounceSpeed;
        }

        //plafon check
        if (b->y - b->radius <= 0) {
            b->y = b->radius;
            b->speedY = 0.0f;
        }

        float minBounce = hero.height + 40.0f;
        float estimatedPeak  = b->bounceSpeed * 12.0f;
        if(estimatedPeak < minBounce)
            b->bounceSpeed +=0.2f;
    }
}

void CheckCollisions(){
    float harpoonCenterX = harpoon.x + harpoon.width / 2.0f;
    float harpoonTop = harpoon.y - harpoon.length;
    float harpoonBottom = harpoon.y;

    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (!balloons[i].active) continue;
        Balloon* b = &balloons[i];

        if(harpoon.isActive){

        float distanceHarpoonX = fabs(b->x - harpoonCenterX);

        if (distanceHarpoonX <= b->radius) {
                if (harpoonTop <= b->y + b->radius && harpoonBottom >= b->y - b->radius) {
                    SplitBalloon(i);
                    harpoon.isActive = false;
                    break;
                }
            }
        }
    }
}

void SplitBalloon(int index) {
    Balloon* b = &balloons[index];
    float newRadius = b->radius /2.0f;

    if (newRadius >= MIN_RADIUS) {
        for (int i = 0; i < MAX_BALLOONS; i++) {
            if (!balloons[i].active) {
                InitBalloon(i, b->x - 10, b->y, newRadius, -3.5f, b->color);
                balloons[i].speedY = -balloons[i].bounceSpeed * SPLIT_BOOST_FACTOR;
                break;
            }
        }

        for (int i = 0; i < MAX_BALLOONS; i++) {
            if (!balloons[i].active) {
                InitBalloon(i, b->x + 10, b->y, newRadius, 3.5f, b->color);
                balloons[i].speedY = -balloons[i].bounceSpeed * SPLIT_BOOST_FACTOR;

                break;
            }
        }
    }

    b->active = false;
    gameState.activeBalloonCount--;
}

void DrawBalloonGDI(HDC hdc, Balloon* b) {
    int centerX = (int)b->x;
    int centerY = (int)b->y;
    int radius = (int)b->radius;

    int r = GetRValue(b->color);
    int g = GetGValue(b->color);
    int b_val = GetBValue(b->color);

    for (int i = radius; i > 0; i--) {
        float factor = (float)i / radius;
        int newR = r + (int)((255 - r) * (1.0f - factor) * 0.4f);
        int newG = g + (int)((255 - g) * (1.0f - factor) * 0.4f);
        int newB = b_val + (int)((255 - b_val) * (1.0f - factor) * 0.4f);

        HBRUSH brush = CreateSolidBrush(RGB(newR, newG, newB));
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(newR, newG, newB));

        SelectObject(hdc, brush);
        SelectObject(hdc, pen);

        Ellipse(hdc, centerX - i, centerY - i, centerX + i, centerY + i);

        DeleteObject(brush);
        DeleteObject(pen);
    }

    HPEN outlinePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    SelectObject(hdc, outlinePen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Ellipse(hdc, centerX - radius, centerY - radius,
            centerX + radius, centerY + radius);
    DeleteObject(outlinePen);

    HBRUSH highlightBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, highlightBrush);
    SelectObject(hdc, GetStockObject(NULL_PEN));

    int hlX = centerX - radius / 3;
    int hlY = centerY - radius / 2;
    int hlW = radius / 2;
    int hlH = radius / 3;
    Ellipse(hdc, hlX - hlW/2, hlY - hlH/2, hlX + hlW/2, hlY + hlH/2);

    DeleteObject(highlightBrush);
}
