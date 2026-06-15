#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <math.h>
#include "display.h"
#include "common.h"

/* ═══════════════════════════════════════════════════════════════════
 * CAVEMAN ESCAPE — display.c
 *
 * Changes vs original:
 *  • Pause overlay (render_pause_overlay) added.
 *  • Invincibility blink: player sprite flashes when invuln frames
 *    are active (exposed via extern invuln_ticks_remaining).
 *  • Shield icon shown in HUD; shield item rendered on board (🛡).
 *  • Axe rendered with correct emoji (🪓) — original had a blank.
 *  • render_ui: legend updated, shield/axe status shown clearly.
 *  • render_game_over_overlay: level reached shown on overlay.
 *  • Bug fix: double-width emoji boundary patch extended to left
 *    border column 0 as well.
 *  • Tidy: gotoGrid is now file-local static (not exposed in header).
 * ═══════════════════════════════════════════════════════════════════ */

struct termios orig_termios;

/* Expose invuln counter from physics.c so display can flash player */
extern int team_has_shield;
extern Target shield_item;

/* We track invincibility via a simple file-local flag set by physics */
/* Access the static via a getter shim declared in physics.h          */
/* Instead, we expose it as a weakly-typed extern for simplicity       */
extern int invuln_ticks_remaining __attribute__((weak));

/* ── Terminal helpers ─────────────────────────────────────────── */

void init_terminal(void)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    raw.c_lflag    &= ~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    printf("\033[2J\033[H\033[?25l");
    fflush(stdout);
}

void restore_terminal(void)
{
    printf("\033[0m\033[?25h\033[%d;1H\n", GRID_HEIGHT + 6);
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

static void gotoGrid(int x, int y) {
    printf("\033[%d;%dH", y + 1, (x * 2) + 1);
}

static void gotoText(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

/* ── Static board ─────────────────────────────────────────────── */

void render_static_board(void)
{
    printf("\033[2J\033[H");

    /* Border walls — desert sand brown box */
    printf("\033[38;5;130m");
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (y == 0 || y == GRID_HEIGHT-1 || x == 0 || x == GRID_WIDTH-1) {
                gotoGrid(x, y);
                printf("🧱");
            }
        }
    }

    /* Desert rocks/sand obstacles */
    printf("\033[1;33m");
    for (int i = 0; i < obstacle_count; i++) {
        gotoGrid(obstacles[i].pos.x, obstacles[i].pos.y);
        printf("🧱");
    }

    printf("\033[0m");
    fflush(stdout);
}

/* ── Delta render (called every tick) ─────────────────────────── */

void render_delta(Position prev_player, Position prev_player2,
                  Position prev_enemies[MAX_ENEMIES])
{
    /* 1. Erase previous dynamic positions */
    gotoGrid(prev_player.x, prev_player.y); printf("  ");

    if (num_players == 2) {
        gotoGrid(prev_player2.x, prev_player2.y); printf("  ");
    }

    for (int i = 0; i < enemy_count; i++) {
        if (prev_enemies[i].x > 0) {
            gotoGrid(prev_enemies[i].x, prev_enemies[i].y); printf("  ");
        }
    }

    /* Erase previous item positions */
    if (food.active)         { gotoGrid(food.pos.x,        food.pos.y);         printf("  "); }
    if (axe.active)          { gotoGrid(axe.pos.x,         axe.pos.y);          printf("  "); }
    if (shield_item.active)  { gotoGrid(shield_item.pos.x, shield_item.pos.y);  printf("  "); }
    if (escape_gate.active)  { gotoGrid(escape_gate.pos.x, escape_gate.pos.y);  printf("  "); }

    /* 2. Re-patch obstacles & right/left borders after emoji bleed */
    printf("\033[1;33m");
    for (int i = 0; i < obstacle_count; i++) {
        gotoGrid(obstacles[i].pos.x, obstacles[i].pos.y);
        printf("🧱");
    }
    printf("\033[38;5;130m");
    for (int y = 1; y < GRID_HEIGHT-1; y++) {
        gotoGrid(0,            y); printf("🧱");
        gotoGrid(GRID_WIDTH-1, y); printf("🧱");
    }
    /* 3. Draw Food — twinkling meat */
    if (food.active) {
        printf("\033[1;33m");
        gotoGrid(food.pos.x, food.pos.y);
        printf(frame_tick % 2 == 0 ? "🍖" : "🍗");
    }

    /* 4. Draw Axe */
    if (axe.active) {
        printf("\033[1;31m");
        gotoGrid(axe.pos.x, axe.pos.y);
        printf("🪓");
    }

    /* 5. Draw Shield */
    if (shield_item.active) {
        printf(frame_tick % 2 == 0 ? "\033[1;34m" : "\033[1;36m");
        gotoGrid(shield_item.pos.x, shield_item.pos.y);
        printf("🛡");
    }

    /* 6. Draw Escape Gate — pulsing vortex */
    if (escape_gate.active) {
        gotoGrid(escape_gate.pos.x, escape_gate.pos.y);
        int d = frame_tick % 4;
        if      (d == 0 || d == 2) printf("🌀");
        else             printf("💫");
    }

    /* 7. Draw Player 1 — blinks during invincibility */
    {
        int invuln = (invuln_ticks_remaining > 0);
        int show   = !invuln || (frame_tick % 4 < 2); /* blink every 2 ticks */

        if (show) {
            if (team_has_shield)
                printf("\033[1;34m"); /* blue tint when shielded */
            else
                printf("\033[1;32m");
            gotoGrid(player.pos.x, player.pos.y);
            printf("🧔");
        } else {
            gotoGrid(player.pos.x, player.pos.y);
            printf("  "); /* blank during blink */
        }
    }

    /* 8. Draw Player 2 */
    if (num_players == 2) {
        int invuln = (invuln_ticks_remaining > 0);
        int show   = !invuln || (frame_tick % 4 < 2);
        if (show) {
            printf("\033[1;36m");
            gotoGrid(player2.pos.x, player2.pos.y);
            printf("👱");
        } else {
            gotoGrid(player2.pos.x, player2.pos.y);
            printf("  ");
        }
    }

    /* 9. Draw Enemies — rage mode within 5 tiles */
    static const char *beast_colors[8] = {
        "\033[1;31m", "\033[1;33m", "\033[1;35m", "\033[1;36m",
        "\033[1;34m", "\033[38;5;208m", "\033[38;5;130m", "\033[1;37m"
    };

    for (int i = 0; i < enemy_count; i++) {
        if (!enemies[i].active) continue;

        double dx1 = enemies[i].pos.x - player.pos.x;
        double dy1 = enemies[i].pos.y - player.pos.y;
        double dist = sqrt(dx1*dx1 + dy1*dy1);

        if (num_players == 2) {
            double dx2 = enemies[i].pos.x - player2.pos.x;
            double dy2 = enemies[i].pos.y - player2.pos.y;
            double d2  = sqrt(dx2*dx2 + dy2*dy2);
            if (d2 < dist) dist = d2;
        }

        gotoGrid(enemies[i].pos.x, enemies[i].pos.y);

        if (dist <= 5.0) {
            if (frame_tick % 2 == 0)
                printf("\033[1;5;31m☠\033[0m");
            else
                printf("\033[1;31m%s\033[0m", enemies[i].type == 1 ? "🦖" : "🐘");
        } else {
            printf("%s%s\033[0m", beast_colors[i % 8], enemies[i].type == 1 ? "🦖" : "🐘");
        }
    }

    /* Draw HUD */
    printf("\033[0m");
    render_ui();
    fflush(stdout);
}

/* ── HUD ──────────────────────────────────────────────────────── */

void render_ui(void)
{
    /* Determine weapon string */
    const char *weapon = "NONE";
    if (team_has_axe)    weapon = "\033[1;31mAXE \033[0m";
    if (team_has_shield) weapon = "\033[1;34mSHLD\033[0m";
    if (team_has_axe && team_has_shield) weapon = "\033[1;31mAXE\033[0m+\033[1;34mSHLD\033[0m";

    gotoText(2, GRID_HEIGHT);
    printf("Lv:\033[1;36m%-2d\033[0m Sc:\033[1;32m%-5d\033[0m "
           "Lives:\033[1;31m%d\033[0m Gear:%s  "
           "Best:\033[1;33m%d\033[0m  [P]Pause [ESC]Quit   ",
           current_level, current_score, player_lives,
           weapon, high_score);

    gotoText(2, GRID_HEIGHT + 1);
    printf("Caveman[🧔] P2[👱] Beast[🐘/🦖] Meat[🍖] "
           "Axe[🪓] Shield[🛡] Portal[🌀]          ");

    gotoText(2, GRID_HEIGHT + 2);
    if (escape_gate.active) {
        printf("\033[1;35m[ALERT] ESCAPE GATE OPEN at right wall — RUN!  "
               "                    \033[0m");
    } else {
        unsigned int need = current_level * TARGET_SCORE;
        printf("Collect Meat to open Escape Gate  "
               "[\033[1;32m%d\033[0m / \033[1;33m%d\033[0m pts]"
               "                    ",
               current_score > (current_level-1)*TARGET_SCORE
                   ? current_score - (current_level-1)*TARGET_SCORE : 0,
               TARGET_SCORE);
        (void)need;
    }
}

/* ── Pause overlay ────────────────────────────────────────────── */

void render_pause_overlay(void)
{
    int sx = 18, sy = 7;
    printf("\033[1;37m");
    gotoText(sx, sy);     printf("╔══════════════════════════╗");
    gotoText(sx, sy+1);   printf("║   \033[1;33m★  GAME PAUSED  ★   \033[1;37m║");
    gotoText(sx, sy+2);   printf("╠══════════════════════════╣");
    gotoText(sx, sy+3);   printf("║  Press [P] to Resume      ║");
    gotoText(sx, sy+4);   printf("║  Press [ESC] to Quit      ║");
    gotoText(sx, sy+5);   printf("╚══════════════════════════╝");
    printf("\033[0m");
    fflush(stdout);
}

/* ── Game-over overlay ────────────────────────────────────────── */

void render_game_over(int won)
{
    printf("\033[2J\033[H\033[1;37m");
    printf("============================================================\n");
    printf("                      GAME OVER                             \n");
    printf("============================================================\n\n");
    if (won)
        printf("\033[1;32m   CONGRATULATIONS! You escaped the Beast!\n\033[0m");
    else
        printf("\033[1;31m   DEFEAT! Caught by the Beast!\n\033[0m");
    printf("\n   Final Score: %d\n   High Score:  %d\n\n", current_score, high_score);
    printf("============================================================\n");
    fflush(stdout);
}

void render_game_over_overlay(int won, int is_new_record,
                               const char *record_holder)
{
    int sx = 12, sy = 3;
    printf("\033[1;37m");

    gotoText(sx, sy);   printf("╔══════════════════════════════════╗");
    gotoText(sx, sy+1); printf("║");
    printf("\033[1;31m            GAME  OVER            ");
    printf("\033[1;37m║");

    gotoText(sx, sy+2); printf("╠══════════════════════════════════╣");
    gotoText(sx, sy+3); printf("║");
    if (won) printf("\033[1;32m  Result : ESCAPED SUCCESSFULLY!  ");
    else     printf("\033[1;31m  Result : CAUGHT BY THE BEAST!   ");
    printf("\033[1;37m║");

    gotoText(sx, sy+4); printf("║  Level Reached: \033[1;36m%-16d\033[1;37m║", current_level);
    gotoText(sx, sy+5); printf("║  Final Score  : \033[1;32m%-16d\033[1;37m║", current_score);
    gotoText(sx, sy+6); printf("║  High Score   : \033[1;33m%-16d\033[1;37m║", high_score);

    gotoText(sx, sy+7); printf("║");
    if (is_new_record)
        printf("\033[1;33m  ★ NEW RECORD: %-17.17s★ ", record_holder);
    else
        printf("  Holder : %-23.23s", record_holder);
    printf("\033[1;37m║");

    gotoText(sx, sy+8);  printf("╠══════════════════════════════════╣");
    gotoText(sx, sy+9);  printf("║  \033[1;32m[Y] Play Again                \033[1;37m║");
    gotoText(sx, sy+10); printf("║  \033[1;35m[N] / [ESC] Exit              \033[1;37m║");
    gotoText(sx, sy+11); printf("╚══════════════════════════════════╝");

    printf("\033[0m");
    fflush(stdout);
}

/* ── Input ────────────────────────────────────────────────────── */

int check_input_hit(void)
{
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

void get_player_input(void)
{
    extern int game_paused;
    char ch;

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == 27) {
            usleep(1000);
            if (check_input_hit()) {
                char next;
                if (read(STDIN_FILENO, &next, 1) == 1 && next == '[') {
                    char arrow;
                    if (read(STDIN_FILENO, &arrow, 1) == 1) {
                        if (num_players == 2) {
                            switch (arrow) {
                                case 'A': player2.dir = DIR_UP;    break;
                                case 'B': player2.dir = DIR_DOWN;  break;
                                case 'C': player2.dir = DIR_RIGHT; break;
                                case 'D': player2.dir = DIR_LEFT;  break;
                            }
                        }
                    }
                }
            } else {
                game_active = 0; /* standalone ESC = quit */
            }
        } else if (ch == 'p' || ch == 'P') {
            game_paused = !game_paused;
            if (game_paused) render_pause_overlay();
            else             render_static_board();
        } else {
            switch (ch) {
                case 'w': case 'W': player.dir = DIR_UP;    break;
                case 's': case 'S': player.dir = DIR_DOWN;  break;
                case 'a': case 'A': player.dir = DIR_LEFT;  break;
                case 'd': case 'D': player.dir = DIR_RIGHT; break;
            }
        }
    }
}
