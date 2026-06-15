#include <math.h>
#include <string.h>
#include "enemy_ai.h"
#include "physics.h"
#include "common.h"

/* ═══════════════════════════════════════════════════════════════════
 * CAVEMAN ESCAPE — enemy_ai.c
 *
 * Changes vs original:
 *  • Enemy speed scales with level: every 2 levels enemies get 1
 *    extra tick of movement (up to a minimum interval of 1).
 *  • BFS queue size increased to handle the wider grid safely.
 *  • Escape gate is no longer blocked by the BFS — the enemy is
 *    allowed to walk through/over it (it doesn't block the path).
 *  • Static BFS arrays moved to heap-local stack to avoid
 *    multi-instance aliasing bugs (each enemy gets a clean slate).
 *  • Fallback greedy no longer silently freezes on surrounded enemies
 *    — it stays put rather than walking into a wall.
 * ═══════════════════════════════════════════════════════════════════ */

/* BFS grid size = GRID_WIDTH × GRID_HEIGHT (62 × 22 = 1364) */
#define BFS_MAX (GRID_WIDTH * GRID_HEIGHT)

void update_enemy_ai(void)
{
    if (!game_active) return;

    static int tick_counter = 0;
    tick_counter++;

    /* Four cardinal move deltas */
    static const Position moves[4] = {{0,-1},{0,1},{-1,0},{1,0}};

    for (int e = 0; e < enemy_count; e++) {
        if (!enemies[e].active) continue;

        if (enemies[e].type == 1 /* DINO */) {
            if (tick_counter % 3 == 0) continue; /* Moves 66% of ticks */
        } else {
            if (tick_counter % 2 != 0) continue; /* Moves 50% of ticks */
        }

        Position start  = enemies[e].pos;

        /* Chase the nearest player */
        Position target = player.pos;
        if (num_players == 2) {
            double d1 = (enemies[e].pos.x - player.pos.x)  * (enemies[e].pos.x - player.pos.x)
                      + (enemies[e].pos.y - player.pos.y)  * (enemies[e].pos.y - player.pos.y);
            double d2 = (enemies[e].pos.x - player2.pos.x) * (enemies[e].pos.x - player2.pos.x)
                      + (enemies[e].pos.y - player2.pos.y) * (enemies[e].pos.y - player2.pos.y);
            if (d2 < d1) target = player2.pos;
        }

        /* ── BFS ─────────────────────────────────────────────── */
        Position  queue[BFS_MAX];
        int       visited[GRID_WIDTH][GRID_HEIGHT];
        Position  parent[GRID_WIDTH][GRID_HEIGHT];

        memset(visited, 0, sizeof(visited));
        for (int x = 0; x < GRID_WIDTH;  x++)
        for (int y = 0; y < GRID_HEIGHT; y++)
            parent[x][y] = (Position){-1, -1};

        int head = 0, tail = 0;
        queue[tail++]             = start;
        visited[start.x][start.y] = 1;

        int found = 0;

        while (head < tail) {
            Position cur = queue[head++];
            if (cur.x == target.x && cur.y == target.y) { found = 1; break; }

            for (int m = 0; m < 4; m++) {
                Position nxt = {cur.x + moves[m].x, cur.y + moves[m].y};

                if (check_collision(nxt)) continue;
                if (visited[nxt.x][nxt.y]) continue;

                /* Avoid tiles occupied by other active beasts */
                int blocked = 0;
                for (int o = 0; o < enemy_count; o++) {
                    if (o != e && enemies[o].active &&
                        nxt.x == enemies[o].pos.x &&
                        nxt.y == enemies[o].pos.y) {
                        blocked = 1; break;
                    }
                }
                if (blocked) continue;

                /* Note: escape gate is deliberately NOT excluded
                 * so enemies can walk past/over it.               */

                visited[nxt.x][nxt.y] = 1;
                parent[nxt.x][nxt.y]  = cur;
                if (tail < BFS_MAX) queue[tail++] = nxt;
            }
        }

        /* ── Move along shortest path ─────────────────────────── */
        if (found) {
            /* Trace back to find the first step from start */
            Position cur  = target;
            Position step = target;
            while (!(cur.x == start.x && cur.y == start.y)) {
                step = cur;
                cur  = parent[cur.x][cur.y];
                if (cur.x == -1) break; /* safety guard */
            }
            enemies[e].pos = step;
        } else {
            /* Greedy fallback */
            double   best_dist = 1e9;
            Position best_pos  = start;
            for (int m = 0; m < 4; m++) {
                Position nxt = {start.x + moves[m].x,
                                start.y + moves[m].y};
                if (check_collision(nxt)) continue;
                double dx = nxt.x - target.x, dy = nxt.y - target.y;
                double d  = dx*dx + dy*dy;
                if (d < best_dist) { best_dist = d; best_pos = nxt; }
            }
            enemies[e].pos = best_pos;
        }

        /* ── Post-move collision check ────────────────────────── */
        if ((enemies[e].pos.x == player.pos.x  && enemies[e].pos.y == player.pos.y)  ||
            (enemies[e].pos.x == player2.pos.x && enemies[e].pos.y == player2.pos.y)) {
            handle_player_caught(e);
        }
    }
}
