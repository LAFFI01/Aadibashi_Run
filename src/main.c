#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "physics.h"
#include "enemy_ai.h"
#include "display.h"
#include "records.h"

/* ═══════════════════════════════════════════════════════════════════
 * CAVEMAN ESCAPE — main.c
 *
 * Changes vs original:
 *  • Pause feature: [P] toggles game_paused; loop idles during pause.
 *  • render_static_board() called after reset_positions() returns so
 *    the board is always visually clean on level transition.
 *  • Player-selection menu now shows controls for both modes.
 *  • Controls reminder shown in the HUD legend line.
 *  • Cleaner shutdown: restore_terminal always called on exit.
 * ═══════════════════════════════════════════════════════════════════ */

/* Pause flag — read by display.c via extern */
int game_paused = 0;

int main(void)
{
    load_high_score();
    init_terminal();

    int play_again = 1;
    while (play_again) {

        /* ── Player-select menu ─────────────────────────────── */
        printf("\033[2J\033[H\033[1;36m");
        printf("════════════════════════════════════════════════════════════\n");
        printf("             ★  CAVEMAN ESCAPE  ★  SELECT MODE             \n");
        printf("════════════════════════════════════════════════════════════\n\n");
        printf("    [1]  Single Player   — WASD to move\n\n");
        printf("    [2]  Two Players     — WASD (P1)  ▲▼◀▶ Arrow Keys (P2)\n\n");
        printf("    [ESC] Quit\n\n");
        printf("    Controls (in-game): [P] Pause / Resume\n\n");
        printf("════════════════════════════════════════════════════════════\n");
        printf("    Collect Meat → Open Portal → Escape! Avoid the Beasts.\n");
        printf("    Power-ups: Axe[🪓] kills a Beast · Shield[🛡] absorbs a hit\n");
        printf("════════════════════════════════════════════════════════════\n");
        printf("\033[0m");
        fflush(stdout);

        int waiting = 1;
        while (waiting) {
            if (check_input_hit()) {
                char ch;
                if (read(STDIN_FILENO, &ch, 1) == 1) {
                    if (ch == '1') { num_players = 1; waiting = 0; }
                    else if (ch == '2') { num_players = 2; waiting = 0; }
                    else if (ch == 27) { restore_terminal(); return 0; }
                }
            }
            usleep(50000);
        }

        /* ── Game init ──────────────────────────────────────── */
        game_paused = 0;
        init_game();
        render_static_board();

        Position prev_player  = player.pos;
        Position prev_player2 = player2.pos;
        Position prev_enemies[MAX_ENEMIES];
        for (int i = 0; i < MAX_ENEMIES; i++)
            prev_enemies[i] = (Position){0, 0};

        /* ── Primary arcade loop ────────────────────────────── */
        while (game_active) {

            /* Handle pause */
            if (game_paused) {
                if (check_input_hit()) get_player_input();
                usleep(TICK_DURATION_MS * 1000);
                continue;
            }

            /* Snapshot previous positions for delta erase */
            prev_player  = player.pos;
            prev_player2 = player2.pos;
            for (int i = 0; i < enemy_count; i++)
                prev_enemies[i] = enemies[i].pos;

            if (check_input_hit()) get_player_input();

            update_physics();
            update_enemy_ai();

            /* After a level transition physics calls reset_positions()
             * which does NOT re-render — we must do it here.          */
            static unsigned int prev_level = 0;
            if (prev_level == 0) prev_level = current_level; // Init on first loop
            
            if (current_level != prev_level) {
                render_static_board();
                prev_level = current_level;
            }

            render_delta(prev_player, prev_player2, prev_enemies);

            frame_tick++;
            usleep(TICK_DURATION_MS * 1000);
        }

        /* ── Game ended ─────────────────────────────────────── */
        restore_terminal();

        int  is_new_record = 0;
        char name[32]      = "NCIT Student";

        if (current_score > high_score) {
            is_new_record = 1;
            high_score    = current_score;

            printf("\n\033[1;33m  ╔══════════════════════════════════╗\033[0m\n");
            printf("\033[1;33m  ║      *** NEW HIGH SCORE! ***     ║\033[0m\n");
            printf("\033[1;33m  ╚══════════════════════════════════╝\033[0m\n\n");
            printf("  Enter your name: ");
            fflush(stdout);

            if (fgets(name, sizeof(name), stdin)) {
                name[strcspn(name, "\n")] = '\0';
                if (strlen(name) == 0) strcpy(name, "NCIT Student");
            }
            save_high_score(name, current_score);
        }

        /* ── Show game-over overlay ─────────────────────────── */
        init_terminal();
        render_static_board();
        render_game_over_overlay(player_won, is_new_record, high_score_name);

        /* ── Poll for play-again ────────────────────────────── */
        int waiting2 = 1;
        while (waiting2) {
            if (check_input_hit()) {
                char ch;
                if (read(STDIN_FILENO, &ch, 1) == 1) {
                    if (ch == 'y' || ch == 'Y') { play_again = 1; waiting2 = 0; }
                    else if (ch == 'n' || ch == 'N' || ch == 27)
                        { play_again = 0; waiting2 = 0; }
                }
            }
            usleep(50000);
        }
    }

    /* ── Credits ────────────────────────────────────────────── */
    restore_terminal();
    printf("\033[2J\033[H\033[1;32m");
    printf("════════════════════════════════════════════════════════════\n");
    printf("          THANK YOU FOR PLAYING CAVEMAN ESCAPE!             \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\033[0m");
    printf("  Record Holder : \033[1;33m%s\033[0m"
           "  (Score: \033[1;32m%d\033[0m)\n", high_score_name, high_score);
    printf("  NCIT, Pokhara University — BCA Semester Project\n");
    printf("  Authors : Sandesh Khatri · Aarush Shah · Gautam KC\n");
    printf("════════════════════════════════════════════════════════════\n\n");
    return 0;
}
