#include "database.h"
#include "sqlite3.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <windows.h>

static sqlite3* db = nullptr;

static int GetScoresCallback(void *data, int argc, char **argv, char **colName);

// Poziva se za svaki red rezultata SQL upita
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



