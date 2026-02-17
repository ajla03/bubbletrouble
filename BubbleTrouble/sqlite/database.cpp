#include "database.h"
#include "sqlite3.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <windows.h>

static sqlite3* db = nullptr;

static int GetScoresCallback(void *data, int argc, char **argv, char **colName);

// Poziva se za svaki red rezultata upita
static int GetScoresCallback(void *data, int argc, char **argv, char **colName) {
    HighScore** scorePtr = static_cast<HighScore**>(data);

    if (argc < 5 || !scorePtr || !(*scorePtr)){
        return 1;
    }

    HighScore& current = **scorePtr;

    if (argv[0]) {
        strncpy(current.playerName, argv[0], sizeof(current.playerName) - 1);
        current.playerName[sizeof(current.playerName) - 1] = '\0';
    } else {
        current.playerName[0] = '\0';
    }

    current.score = argv[1] ? atoi(argv[1]) : 0;
    current.level = argv[2] ? atoi(argv[2]) : 0;

    if (argv[3]) {
        strncpy(current.mode, argv[3], sizeof(current.mode) - 1);
        current.mode[sizeof(current.mode) - 1] = '\0';
    } else {
        current.mode[0] = '\0';
    }

    if (argv[4]) {
        strncpy(current.timestamp, argv[4], sizeof(current.timestamp) - 1);
        current.timestamp[sizeof(current.timestamp) - 1] = '\0';
    } else {
        current.timestamp[0] = '\0';
    }

    (*scorePtr)++;

    return 0;
}


bool CreateTables(){
    const char *sql =
        "CREATE TABLE IF NOT EXISTS high_scores ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "player_name TEXT NOT NULL,"
        "score INTEGER NOT NULL,"
        "level INTEGER NOT NULL,"
        "mode TEXT NOT NULL,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        MessageBoxA(NULL, errMsg, "SQL Error", MB_OK | MB_ICONERROR);
        sqlite3_free(errMsg);
        return false;
    }
  const char *sqlProgress =
        "CREATE TABLE IF NOT EXISTS player_progress ("
        "player_name TEXT,"
        "mode TEXT,"
        "max_level INTEGER DEFAULT 0,"
        "PRIMARY KEY (player_name, mode)"
        ");";

    char *errMsg2 = nullptr;
    sqlite3_exec(db, sqlProgress, nullptr, nullptr, &errMsg2);
    if (errMsg2) sqlite3_free(errMsg2);\

    const char *sqlStars =
        "CREATE TABLE IF NOT EXISTS level_stars ("
        "player_name TEXT,"
        "mode TEXT,"
        "level INTEGER,"
        "stars INTEGER,"
        "PRIMARY KEY (player_name, mode, level)"
        ");";

    char *errMsg3 = nullptr;
    sqlite3_exec(db, sqlStars, nullptr, nullptr, &errMsg3);
    if (errMsg3) sqlite3_free(errMsg3);

    return true;
}

bool InitDatabase(){
    int rc = sqlite3_open("bubble_trouble.db", &db);
    if(rc != SQLITE_OK){
        MessageBoxA(NULL, sqlite3_errmsg(db), "Database Error", MB_OK | MB_ICONERROR);
        return false;
    }
    return CreateTables();
}

void CloseDatabase(){
    if(db){
        sqlite3_close(db);
        db = nullptr;
    }
}

bool SaveScore(const char* playerName, int score, int level, const char* mode){
    if(!db) return false;

    char *sql = sqlite3_mprintf(
        "INSERT INTO high_scores (player_name, score, level, mode) "
        "VALUES (%Q, %d, %d, %Q);",
        playerName, score, level, mode
    );

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    sqlite3_free(sql);  // jer sql mptrinf alocira memoriju

    if (rc != SQLITE_OK) {
        MessageBoxA(NULL, errMsg, "Save Error", MB_OK | MB_ICONERROR);
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool GetTopScores(HighScore* scores, int count, const char* mode) {
    if (!db || !scores || count <= 0) return false;

    HighScore* currentScore = scores;

    char *sql = sqlite3_mprintf(
        "SELECT player_name, score, level, mode, timestamp "
        "FROM high_scores "
        "WHERE mode = %Q "
        "ORDER BY score DESC "
        "LIMIT %d;",
        mode, count
    );

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, GetScoresCallback, &currentScore, &errMsg);
    sqlite3_free(sql);

    if (rc != SQLITE_OK) {
        MessageBoxA(NULL, errMsg, "Query Error", MB_OK | MB_ICONERROR);
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

static int GetMaxCallback(void *data, int argc, char **argv, char **colName) {
    int* resultPtr = static_cast<int*>(data);

    if (argc > 0 && argv[0]) {
        *resultPtr = atoi(argv[0]);
    } else {
        *resultPtr = 0;
    }
    return 0;
}

int GetBestScoreForPlayer(const char* playerName){
    if(!db) return 0;

    int maxScore = 0;

    char* sql = sqlite3_mprintf(
    "SELECT MAX(score) FROM high_scores WHERE player_name = %Q",
     playerName);

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, GetMaxCallback, &maxScore, &errMsg );

    sqlite3_free(sql);

    if(rc!=SQLITE_OK){
        MessageBoxA(NULL, errMsg, "GetMaxScore Error", MB_OK | MB_ICONERROR);
        sqlite3_free(errMsg);
        return 0;
    }

    return maxScore;
}
int GetPlayerMaxLevel(const char* playerName, const char* mode) {
    if (!db) return 0;

    int maxLevel = 0;
    char *sql = sqlite3_mprintf("SELECT max_level FROM player_progress WHERE player_name = %Q AND mode = %Q;", playerName, mode);

    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
        int* val = (int*)data;
        if (argc > 0 && argv[0]) *val = atoi(argv[0]);
        return 0;
    };

    char *errMsg = nullptr;
    sqlite3_exec(db, sql, callback, &maxLevel, &errMsg);
    sqlite3_free(sql);
    if (errMsg) sqlite3_free(errMsg);

    return maxLevel;
}

void SavePlayerProgress(const char* playerName, const char* mode, int newMaxLevel) {
    if (!db) return;

    char *sql = sqlite3_mprintf(
        "INSERT OR REPLACE INTO player_progress (player_name, mode, max_level) VALUES (%Q, %Q, %d);",
        playerName, mode, newMaxLevel
    );

    char *errMsg = nullptr;
    sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    sqlite3_free(sql);
    if (errMsg) sqlite3_free(errMsg);
}

void SaveLevelStars(const char* playerName, const char* mode, int level, int stars) {
    if (!db) return;

    char *sql = sqlite3_mprintf(
        "INSERT OR REPLACE INTO level_stars (player_name, mode, level, stars) VALUES (%Q, %Q, %d, %d);",
        playerName, mode, level, stars
    );

    char *errMsg = nullptr;
    sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    sqlite3_free(sql);
    if (errMsg) sqlite3_free(errMsg);
}

int GetLevelStars(const char* playerName, const char* mode, int level) {
    if (!db) return 0;

    int stars = 0;
    char *sql = sqlite3_mprintf("SELECT stars FROM level_stars WHERE player_name = %Q AND mode = %Q AND level = %d;", playerName, mode, level);

    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
        int* val = (int*)data;
        if (argc > 0 && argv[0]) *val = atoi(argv[0]);
        return 0;
    };

    char *errMsg = nullptr;
    sqlite3_exec(db, sql, callback, &stars, &errMsg);
    sqlite3_free(sql);
    if (errMsg) sqlite3_free(errMsg);

    return stars;
}
