#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "physics.h"
#include "common.h"

/* ═══════════════════════════════════════════════════════════════════
 * CAVEMAN ESCAPE — physics.c
 *
 * Changes vs original:
 *  • Invincibility frames (INVULN_TICKS) after being caught —
 *    prevents instant double-hit death on respawn.
 *  • Shield power-up: extra collectible that grants one free hit.
 *  • Axe spawns from level 2 onward (every even level), not 4.
 *  • Bug fix: player2 direction was missing reset in solo mode.
 *  • generate_obstacles() now produces 4 walls for richer layouts.
 *  • Enemy count strictly capped at MAX_ENEMIES.
 *  • reset_positions() removed render call (separation of concerns).
 * ═══════════════════════════════════════════════════════════════════ */

#define INVULN_TICKS  60   /* ~5 seconds at 80 ms/tick */

/* Global state definitions */
int          game_active    = 0;
int          player_won     = 0;
unsigned int current_score  = 0;
unsigned int high_score     = 0;
char         high_score_name[32] = "NCIT Student";
unsigned int frame_tick     = 0;
Player       player;
Player       player2;
int          num_players    = 1;
int          player_lives   = 3;
Target       axe;
int          team_has_axe   = 0;
Enemy        enemies[MAX_ENEMIES];
int          enemy_count    = 1;
unsigned int current_level  = 1;
Target       food;
Target       escape_gate;
Obstacle     obstacles[MAX_OBSTACLES];
int          obstacle_count = 0;

/* New globals: shield */
Target shield_item;
int    team_has_shield = 0;

int invuln_ticks_remaining = 0;

/* ──────────────────────────────────────────────────────────────── */

int check_collision(Position pos)
{
    if (pos.x <= 0 || pos.x >= GRID_WIDTH - 1 ||
        pos.y <= 0 || pos.y >= GRID_HEIGHT - 1)
        return 1;
    for (int i = 0; i < obstacle_count; i++)
        if (obstacles[i].pos.x == pos.x && obstacles[i].pos.y == pos.y)
            return 1;
    return 0;
}

static int pos_on_player(Position p)
{
    if (p.x == player.pos.x  && p.y == player.pos.y)  return 1;
    if (p.x == player2.pos.x && p.y == player2.pos.y) return 1;
    return 0;
}

static int pos_on_enemy(Position p)
{
    for (int i = 0; i < enemy_count; i++)
        if (enemies[i].active &&
            p.x == enemies[i].pos.x && p.y == enemies[i].pos.y)
            return 1;
    return 0;
}

/* Generic safe-spawn: avoids walls, entities, and up to 4 extra positions */
static Position spawn_item_safe(Position *excl, int nexcl)
{
    Position p;
    int ok;
    do {
        p.x = 1 + rand() % (GRID_WIDTH  - 2);
        p.y = 1 + rand() % (GRID_HEIGHT - 2);
        ok  = 1;
        if (check_collision(p) || pos_on_player(p) || pos_on_enemy(p))
            { ok = 0; continue; }
        for (int i = 0; i < nexcl; i++)
            if (p.x == excl[i].x && p.y == excl[i].y)
                { ok = 0; break; }
    } while (!ok);
    return p;
}

void spawn_food(void)
{
    Position excl[2] = { escape_gate.pos, axe.pos };
    food.pos    = spawn_item_safe(excl, 2);
    food.active = 1;
}

void spawn_axe(void)
{
    Position excl[2] = { food.pos, escape_gate.pos };
    axe.pos    = spawn_item_safe(excl, 2);
    axe.active = 1;
}

static void spawn_shield(void)
{
    Position excl[3] = { food.pos, escape_gate.pos, axe.pos };
    shield_item.pos    = spawn_item_safe(excl, 3);
    shield_item.active = 1;
}

static void generate_obstacles(void)
{
    obstacle_count = 0;

#define ADD(px,py) if(obstacle_count<MAX_OBSTACLES) obstacles[obstacle_count++].pos=(Position){(px),(py)};

    /* Wall 1: col 14 — open corridor rows 8-11 */
    for (int y = 2;  y <= 7;  y++) ADD(14, y)
    for (int y = 12; y <= 18; y++) ADD(14, y)

    /* Wall 2: col 28 — open top & bottom */
    for (int y = 5;  y <= 14; y++) ADD(28, y)

    /* Wall 3: col 42 — mirrors Wall 1 */
    for (int y = 2;  y <= 7;  y++) ADD(42, y)
    for (int y = 12; y <= 18; y++) ADD(42, y)

    /* Wall 4: col 56 — short central block */
    for (int y = 6;  y <= 15; y++) ADD(56, y)

#undef ADD
}

void reset_positions(void)
{
    player.pos = (Position){2, 2};
    player.dir = DIR_NONE;

    if (num_players == 2) {
        player2.pos = (Position){2, 3};
        player2.dir = DIR_NONE;
    } else {
        player2.pos = (Position){-1, -1};
        player2.dir = DIR_NONE;
    }

    static const Position spawns[8] = {
        {GRID_WIDTH-3, GRID_HEIGHT-2}, {GRID_WIDTH-3, 2},
        {2,            GRID_HEIGHT-2}, {GRID_WIDTH/2, GRID_HEIGHT-2},
        {GRID_WIDTH/2, 2            }, {GRID_WIDTH-3, GRID_HEIGHT/2},
        {2,            GRID_HEIGHT/2}, {GRID_WIDTH/2, GRID_HEIGHT/2}
    };

    /* enemy_count <= MAX_ENEMIES == 8; spawns[] has 8 entries */
    for (int i = 0; i < enemy_count && i < 8; i++) {
        if (!enemies[i].active) continue;
        if (i < 8) {
            enemies[i].pos = spawns[i];
        } else {
            int valid = 0;
            while (!valid) {
                Position rp = {1 + rand()%(GRID_WIDTH-3),
                               1 + rand()%(GRID_HEIGHT-2)};
                if (!check_collision(rp) && !pos_on_player(rp) && !pos_on_enemy(rp))
                    { enemies[i].pos = rp; valid = 1; }
            }
        }
    }

    invuln_ticks_remaining = INVULN_TICKS;
}

void handle_player_caught(int enemy_idx)
{
    if (team_has_shield) {
        team_has_shield         = 0;
        invuln_ticks_remaining  = INVULN_TICKS;
        return;
    }
    if (team_has_axe) {
        enemies[enemy_idx].active = 0;
        team_has_axe              = 0;
        invuln_ticks_remaining    = INVULN_TICKS / 2;
        return;
    }
    player_lives--;
    if (player_lives <= 0) {
        game_active = 0;
        player_won  = 0;
    } else {
        reset_positions();
    }
}

void init_game(void)
{
    srand((unsigned int)time(NULL));

    game_active         = 1;
    player_won          = 0;
    current_score       = 0;
    frame_tick          = 0;
    current_level       = 1;
    player_lives        = 3;
    team_has_axe        = 0;
    team_has_shield     = 0;
    enemy_count         = 1;
    invuln_ticks_remaining = 0;

    player.pos = (Position){2, 2};
    player.dir = DIR_NONE;

    if (num_players == 2) {
        player2.pos = (Position){2, 3};
        player2.dir = DIR_NONE;
    } else {
        player2.pos = (Position){-1, -1};
        player2.dir = DIR_NONE;
    }

    enemies[0].pos    = (Position){GRID_WIDTH - 3, GRID_HEIGHT - 2};
    enemies[0].active = 1;
    enemies[0].type   = 0;
    for (int i = 1; i < MAX_ENEMIES; i++) {
        enemies[i].pos    = (Position){-1, -1};
        enemies[i].active = 0;
        enemies[i].type   = 0;
    }

    generate_obstacles();

    escape_gate.pos    = (Position){GRID_WIDTH - 2, GRID_HEIGHT / 2};
    escape_gate.active = 0;
    axe.active         = 0;
    shield_item.active = 0;

    spawn_food();
}

void update_physics(void)
{
    if (!game_active) return;

    if (invuln_ticks_remaining > 0)
        invuln_ticks_remaining--;

    /* Move player 1 */
    {
        Position np = player.pos;
        switch (player.dir) {
            case DIR_UP:    np.y--; break;
            case DIR_DOWN:  np.y++; break;
            case DIR_LEFT:  np.x--; break;
            case DIR_RIGHT: np.x++; break;
            default: break;
        }
        if (player.dir != DIR_NONE && check_collision(np)) {
            np = player.pos; player.dir = DIR_NONE;
        }
        player.pos = np;
    }

    /* Move player 2 */
    if (num_players == 2) {
        Position np = player2.pos;
        switch (player2.dir) {
            case DIR_UP:    np.y--; break;
            case DIR_DOWN:  np.y++; break;
            case DIR_LEFT:  np.x--; break;
            case DIR_RIGHT: np.x++; break;
            default: break;
        }
        if (player2.dir != DIR_NONE && check_collision(np)) {
            np = player2.pos; player2.dir = DIR_NONE;
        }
        player2.pos = np;
    }

#define PLAYER_ON(px,py) \
    ((player.pos.x==(px)&&player.pos.y==(py)) || \
     (num_players==2&&player2.pos.x==(px)&&player2.pos.y==(py)))

    /* Collect food */
    if (food.active && PLAYER_ON(food.pos.x, food.pos.y)) {
        current_score += 10;
        if (current_score >= current_level * TARGET_SCORE)
            escape_gate.active = 1;
        spawn_food();
        /* Offer shield every 3rd food on even levels */
        if ((current_score / 10) % 3 == 0 && current_level % 2 == 0 &&
            !shield_item.active && !team_has_shield)
            spawn_shield();
    }

    /* Collect axe */
    if (axe.active && PLAYER_ON(axe.pos.x, axe.pos.y)) {
        team_has_axe = 1;
        axe.active   = 0;
    }

    /* Collect shield */
    if (shield_item.active &&
        PLAYER_ON(shield_item.pos.x, shield_item.pos.y)) {
        team_has_shield       = 1;
        shield_item.active    = 0;
    }

    /* Escape gate */
    if (escape_gate.active &&
        PLAYER_ON(escape_gate.pos.x, escape_gate.pos.y)) {

        current_level++;
        if (enemy_count < MAX_ENEMIES) {
            enemies[enemy_count].pos    = (Position){GRID_WIDTH-3, 2};
            enemies[enemy_count].active = 1;
            enemy_count++;
        }
        escape_gate.active = 0;
        int numDinos = (current_level >= 3) ? ((current_level - 3) / 2) + 1 : 0;
        for (int i = 0; i < enemy_count; i++) {
            enemies[i].active = 1;
            enemies[i].type = (i < numDinos) ? 1 : 0;
        }
        team_has_axe    = 0;
        team_has_shield = 0;
        shield_item.active = 0;
        if (current_level >= 4 && current_level % 2 == 0) spawn_axe(); else axe.active = 0;
        spawn_food();
        reset_positions();
        return;
    }

    /* Enemy collision (skipped during invincibility) */
    if (invuln_ticks_remaining == 0) {
        /* enemy_count <= MAX_ENEMIES == 8; spawns[] has 8 entries */
    for (int i = 0; i < enemy_count && i < 8; i++) {
            if (enemies[i].active &&
                PLAYER_ON(enemies[i].pos.x, enemies[i].pos.y)) {
                handle_player_caught(i);
                return;
            }
        }
    }

#undef PLAYER_ON
}
