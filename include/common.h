#ifndef COMMON_H
#define COMMON_H

#include <termios.h>

#define GRID_WIDTH 60
#define GRID_HEIGHT 20
#define MAX_OBSTACLES 60
#define TICK_DURATION_MS 150
#define TARGET_SCORE 100
#define MAX_ENEMIES 20

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_NONE
} Direction;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position pos;
    Direction dir;
} Player;

typedef struct {
    Position pos;
    int active;
} Enemy;

typedef struct {
    Position pos;
    int active;
} Target;

typedef struct {
    Position pos;
} Obstacle;

typedef struct {
    char name[32];
    unsigned int score;
} ScoreRecord;

// Global Shared Variables
extern int game_active;
extern int player_won;
extern unsigned int current_score;
extern unsigned int high_score;
extern char high_score_name[32];
extern Player player;
extern Enemy enemies[MAX_ENEMIES];
extern int enemy_count;
extern unsigned int current_level;
extern Target food;
extern Target escape_gate;
extern Obstacle obstacles[MAX_OBSTACLES];
extern int obstacle_count;
extern unsigned int frame_tick;
extern struct termios orig_termios;

#endif // COMMON_H
