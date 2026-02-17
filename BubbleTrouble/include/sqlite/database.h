#pragma once
#include <windows.h>

struct HighScore {
    char playerName[50];
    int score;
    int level;
    char mode[20];
    char timestamp[64];
};

bool InitDatabase();
void CloseDatabase();
bool SaveScore(const char* playerName, int score, int level, const char* mode);
bool GetTopScores(HighScore* scores, int count, const char* mode);
bool CreateTables();
//za unlocked levels
int GetPlayerMaxLevel(const char* playerName);
void SavePlayerProgress(const char* playerName, int newMaxLevel);
