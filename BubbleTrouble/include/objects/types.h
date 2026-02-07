#pragma once
#include<windows.h>

// ======== POMOCNE STRUKTURE ======= //
struct Hero{
 int x, y;
 int dx, dy;
 int width, height;
 int currentFrame, currentRow;
 int animCounter;
 float heroHitCooldown; // in secs
 float blinkTimer;
};

struct Projectile{
 int x, y;
 int width, height;
 int dy;
 int length;
 bool isActive;
 int ownerPlayer;
};

struct StaticObject{
 int x, y;
 int width, height;
};



struct Door : public StaticObject{
 int originalWidth, originalHeight;
 bool active = true;
};

struct Pillar : public StaticObject{
 int originalWidth, originalHeight;
};



struct Balloon {
    float x, y;
    float speedX, speedY;
    float radius;
    float bounceSpeed;
    bool active;
    COLORREF color;
    bool isFrozen;
    float frozenSpeedX;
    float frozenSpeedY;
};

struct InputState{
 bool wasSpacePressed;
 bool wasSpacePressedP2;
};

enum GameMode {
    GAME_MODE_MENU,
    GAME_MODE_SETTINGS,
    GAME_MODE_PLAYING,
    GAME_MODE_PAUSE,
    GAME_OVER,
};

struct MenuButton {
    RECT rect;
    const char* text;
    bool isHovered;
};


struct GameState{
    bool isGameOver;
    bool isLevelCleared;
    GameMode currentMode;
    bool pendingHome;
    bool pendingRestart;
    bool pendingNextLevel;
    bool popupSettings;
    bool hasShield;
    int shieldTimeLeft;
    bool hasDoubleShot;
    int doubleShotTimeLeft;
    bool balloonsAreFrozen;
    int freezeTimeLeft;
    bool isMultiplayer;
    bool isScoreSaved = false;
};

struct HeartAnim{
    int currentFrame;
    int animCounter;
};

// === NEW: PLAYER STATS STRUCTURE ===
struct PlayerStats {
    int lives;
    int score;
    int displayScore;  // For smooth animation
    bool isAlive;
    HeartAnim hearts[5];
};


struct Torch{
 int x, y;
 int width, height;
 int currentFrame, currentRow;
 int animCounter = 0;
};

// used for MENU, RESTART and NEXT button etc.
struct Button {
   int x, y;
   int width, height;
   bool isHover;
};


enum TransitionState{
 TRANSITION_NONE,
 TRANSITION_CLOSING,
 TRANSITION_OPENING,
 TRANSITION_WAIT,
 };

struct AnimatedWalls{
 int wallTopY ;
 int wallBottomY;
 float wallSpeed;
 DWORD transitionWaitStart = 0;
};
enum PowerUpType {
       POWERUP_EXTRA_LIFE,
       POWERUP_EXTRA_TIME,
       POWERUP_FREEZE,
   };

struct PowerUp {
   float x, y;
   float speedY;
   PowerUpType type;
   bool active;
   int width, height;
   DWORD spawnTime;
};

/*  SETTINGS */

enum KeyBindingMode{
    KEYBIND_NONE = 0,
    KEYBIND_LEFT,
    KEYBIND_RIGHT,
    KEYBIND_SHOOT
};

struct KeyBindings{
    int moveLeft;
    int moveRight;
    int shoot;
};

struct SoundState{
 bool soundEffectsOn = true;
 bool bgMusicOn = true;
 };


typedef struct{
    int dotCount;
    DWORD lastDotTime;
    int dotInterval;
}DotAnimation;

struct SettingsState{
 SoundState soundState;
 HBITMAP currentHeroSelected;

 // KEY BINDING MEMBERS //
 KeyBindingMode waitingForKey;
 KeyBindings player1Keys;
 KeyBindings player2Keys;
 Button leftKeyButton;
 Button rightKeyButton;
 Button spaceKeyButton;
 int currentPlayerBinding;

 // ANIMATION
 DotAnimation dotAnimation;
};

// DATA BASE
void SaveFinalScore();
void ResetScoreFlag();

