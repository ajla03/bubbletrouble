#pragma once

// ========== KONSTANTE ZA ZIVOTE =========== //
#define HEART_FRAMES        10
#define HEART_ANIM_SPEED     5
#define MAX_LIVES            3


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

#define MAX_POWERUPS 5
#define POWERUP_FALL_SPEED 2.0f
#define POWERUP_SPAWN_MIN_TIME 2000
#define POWERUP_SPAWN_MAX_TIME 3000
#define POWERUP_SIZE 30

// ==== TIME POWERUP KONSTANTE ==== //
#define TIME_BONUS_AMOUNT 500.0       // Koliko vremena dodaje
#define TIME_POWERUP_SPAWN_CHANCE 30  // 30% šansa za time, 70% za life

#define FREEZE_DURATION 1000  // 5 sekundi

// Spawn šanse (mora biti 100 ukupno):
#define LIFE_POWERUP_SPAWN_CHANCE 50   // 50%
#define TIME_POWERUP_SPAWN_CHANCE 30   // 30%
#define FREEZE_POWERUP_SPAWN_CHANCE 20 // 20%
