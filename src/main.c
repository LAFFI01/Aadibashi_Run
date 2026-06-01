#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "physics.h"
#include "enemy_ai.h"
#include "display.h"
#include "records.h"

int main(void) {
    // 1. Load the initial persistent high score from disk
    load_high_score();
    
    // 2. Configure POSIX raw mode terminal
    init_terminal();
    
    int play_again = 1;
    while (play_again) {
        // 3. Initialize game states
        init_game();
        
        // 4. Perform a complete static render of unicode double walls & stone grid obstacles
        render_static_board();
        
        Position prev_player = player.pos;
        Position prev_enemies[MAX_ENEMIES];
        for (int i = 0; i < MAX_ENEMIES; i++) {
            prev_enemies[i] = (Position){0, 0};
        }
        
        // 5. Primary Arcade Loop
        while (game_active) {
            prev_player = player.pos;
            for (int i = 0; i < enemy_count; i++) {
                prev_enemies[i] = enemies[i].pos;
            }
            
            // Asynchronous Input Polling
            if (check_input_hit()) {
                player.dir = get_player_input();
            }
            
            // Core Physics Engine step
            update_physics();
            
            // Chasing AI Vector step
            update_enemy_ai();
            
            // Delta-rendering refresh step (100% flicker-free)
            render_delta(prev_player, prev_enemies);
            
            // Increment clock tick for rendering dynamic frame animations
            frame_tick++;
            
            // Frame rate timing loop clock
            usleep(TICK_DURATION_MS * 1000);
        }
        
        // 6. Gameplay ended. Restore terminal parameters to prompt username if record is broken
        restore_terminal();
        
        int is_new_record = 0;
        char temp_player_name[32] = "NCIT Student";
        
        if (current_score > high_score) {
            is_new_record = 1;
            high_score = current_score;
            
            printf("\n\033[1;33m  ╔══════════════════════════════════╗\033[0m\n");
            printf("\033[1;33m  ║      *** NEW HIGH SCORE! ***     ║\033[0m\n");
            printf("\033[1;33m  ╚══════════════════════════════════╝\033[0m\n\n");
            printf("  Enter your name: ");
            fflush(stdout);
            
            if (fgets(temp_player_name, sizeof(temp_player_name), stdin) != NULL) {
                temp_player_name[strcspn(temp_player_name, "\n")] = '\0';
                if (strlen(temp_player_name) == 0) {
                    strcpy(temp_player_name, "NCIT Student");
                }
            }
            
            save_high_score(temp_player_name, current_score);
        }
        
        // 7. Re-enable terminal raw mode to draw the arcade overlays and capture menus
        init_terminal();
        
        // Draw the static board once more so overlay is painted cleanly over it
        render_static_board();
        
        // Render the centered, beautiful modal box overlay
        render_game_over_overlay(player_won, is_new_record, high_score_name);
        
        // 8. Poll for menu selections inside raw mode
        int waiting_for_input = 1;
        while (waiting_for_input) {
            if (check_input_hit()) {
                char ch;
                if (read(STDIN_FILENO, &ch, 1) == 1) {
                    if (ch == 'y' || ch == 'Y') {
                        play_again = 1;
                        waiting_for_input = 0;
                    } else if (ch == 'n' || ch == 'N' || ch == 27) { // 27 = ESC
                        play_again = 0;
                        waiting_for_input = 0;
                    }
                }
            }
            usleep(50000); // 50ms polling limit (limits CPU cycles)
        }
    }
    
    // 9. Shutdown cleanly, restore default attributes, and draw final credits
    restore_terminal();
    
    printf("\033[2J\033[H"); // Clear screen
    printf("\033[1;32m");
    printf("============================================================\n");
    printf("          THANK YOU FOR PLAYING CAVEMAN ESCAPE GAME         \n");
    printf("============================================================\n");
    printf("\033[0m");
    printf("  Record Holder: \033[1;33m%s\033[0m (Score: \033[1;32m%d\033[0m)\n", high_score_name, high_score);
    printf("  NCIT, Pokhara University - BCA Department Semester Project\n");
    printf("  Author Credits: Sandesh Khatri, Aarush Shah, Gautam KC.\n");
    printf("============================================================\n\n");
    
    return 0;
}
