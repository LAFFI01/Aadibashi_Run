#ifndef COMMON_H
#define COMMON_H

/* ═══════════════════════════════════════════════════════════════════
 * CAVEMAN ESCAPE — common.h
 * Shared constants, types, and global state declarations.
 * ═══════════════════════════════════════════════════════════════════ */

/* ── Grid dimensions ───────────────────────────────────────────────
 * GRID_WIDTH  : total columns including border walls (cols 0 and W-1)
 * GRID_HEIGHT : total rows    including border walls (rows 0 and H-1)
 * Playfield is therefore [1..W-2] × [1..H-2].
 */
#define GRID_WIDTH      31
#define GRID_HEIGHT     22

/* ── Gameplay constants ────────────────────────────────────────────*/
#define MAX_ENEMIES     8
#define MAX_OBSTACLES   200

/* Score needed per level to activate the escape gate (food × 10 pts) */
#define TARGET_SCORE    50    /* 5 food items × 10 pts each            */

/* Main loop tick in milliseconds (controls overall game speed).
 * Lower = faster.  80 ms ≈ ~12 fps, comfortable for a console game.  */
#define TICK_DURATION_MS  120

/* Enemy moves once every N game ticks (set in enemy_ai.c).
 * Kept as a named constant so it can be tuned from one place.       */
#define ENEMY_TICK_INTERVAL  2

/* ── Direction enum ───────────────────────────────────────────────*/
typedef enum {
    DIR_NONE  = 0,
    DIR_UP    = 1,
    DIR_DOWN  = 2,
    DIR_LEFT  = 3,
    DIR_RIGHT = 4
} Direction;

/* ── Coordinate pair ──────────────────────────────────────────────*/
typedef struct {
    int x, y;
} Position;

/* ── Entity types ─────────────────────────────────────────────────*/
typedef struct {
    Position  pos;
    Direction dir;
} Player;

typedef struct {
    Position pos;
    int      active;   /* 1 = alive/visible, 0 = defeated/hidden */
    int      type;     /* 0 = MAMMOTH, 1 = DINO */
} Enemy;

typedef struct {
    Position pos;
    int      active;
} Target;

typedef struct {
    Position pos;
} Obstacle;

/* ── Global game state (defined in physics.c) ─────────────────────*/
extern int          game_active;
extern int          player_won;
extern unsigned int current_score;
extern unsigned int high_score;
extern char         high_score_name[32];
extern unsigned int frame_tick;

extern Player  player;
extern Player  player2;
extern int     num_players;
extern int     player_lives;

extern Target  axe;
extern int     team_has_axe;

extern Enemy   enemies[MAX_ENEMIES];
extern int     enemy_count;

extern unsigned int current_level;

extern Target    food;
extern Target    escape_gate;
extern Obstacle  obstacles[MAX_OBSTACLES];
extern int       obstacle_count;

/* ── Pause flag (defined in main.c) ──────────────────────────────*/
extern int game_paused;

#endif /* COMMON_H */
