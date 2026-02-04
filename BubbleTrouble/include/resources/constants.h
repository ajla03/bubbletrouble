#pragma once

// ========== KONSTANTE ZA ZIVOTE =========== //
#define HEART_FRAMES        10
#define HEART_ANIM_SPEED     5
#define MAX_LIVES            5
#define START_LIVES          3

// ========= KONSTANTE ZA KRETANJE BALONA ========= //
#define MAX_BALLOONS 20
#define BOUNCE_DAMPING 0.9f
#define MIN_RADIUS 10.0f
#define BALLOON_GRAVITY  0.7f
#define MAX_FALL_SPEED   8.0f
#define MIN_BOUNCE_SPEED   10.0f
#define MAX_BOUNCE_SPEED   13.0f
#define SPLIT_BOOST_FACTOR 1.2f
#define MAX_RADIUS 32.0f

// ==== MAX VRIJEME IGRE - PO LEVELU === //
#define maxTime 2000.0

// ==== KONSTANTA ZA MENU DUGMD ==== //
#define NUM_MENU_BUTTONS 3

#define HERO_INVINCIBLE_TIME 0.5f

#define MAX_LEVELS 6
#define CURRENT_LEVEL gGame.levels[gGame.currentLevel]

#define BONUS_PER_SECOND 5


/* POWERUP KONSTANTE */
#define MAX_POWERUPS 2
#define POWERUP_SIZE 30
#define POWERUP_FALL_SPEED 2.0f

// spawn svake 8-15 sekundi
#define POWERUP_SPAWN_MIN_TIME 10000
#define POWERUP_SPAWN_MAX_TIME 20000

#define LIFE_POWERUP_SPAWN_CHANCE 50
#define TIME_POWERUP_SPAWN_CHANCE 30
#define FREEZE_POWERUP_SPAWN_CHANCE 20

#define TIME_BONUS_AMOUNT 500.0f
#define FREEZE_DURATION 3000  // 3 sekunde

#define SHEET_W 700
#define SHEET_H 550
