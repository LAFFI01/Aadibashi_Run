#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <math.h>
#include "display.h"
#include "common.h"

// Definition of standard termios backup
struct termios orig_termios;

void init_terminal(void) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    
    // Disable canonical input buffering and echoing of characters
    raw.c_lflag &= ~(ICANON | ECHO);
    
    // Set read timeouts: return immediately if no characters are pending
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    
    // Clear screen and hide cursor using ANSI escape sequences
    printf("\033[2J\033[H\033[?25l");
    fflush(stdout);
}

void restore_terminal(void) {
    // Restore default terminal mode and show cursor
    printf("\033[0m\033[?25h\033[%d;1H\n", GRID_HEIGHT + 6);
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

static void gotoXY(int x, int y) {
    // ANSI uses 1-indexed coordinates: (row = y + 1, col = x + 1)
    printf("\033[%d;%dH", y + 1, x + 1);
}

void render_static_board(void) {
    printf("\033[2J\033[H"); // Complete clear screen
    
    // Draw boundary walls in Bold Blue double-line unicode box frames
    printf("\033[1;34m");
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (y == 0 || y == GRID_HEIGHT - 1 || x == 0 || x == GRID_WIDTH - 1) {
                gotoXY(x, y);
                if (y == 0 && x == 0) printf("╔");
                else if (y == 0 && x == GRID_WIDTH - 1) printf("╗");
                else if (y == GRID_HEIGHT - 1 && x == 0) printf("╚");
                else if (y == GRID_HEIGHT - 1 && x == GRID_WIDTH - 1) printf("╝");
                else if (y == 0 || y == GRID_HEIGHT - 1) printf("═");
                else printf("║");
            }
        }
    }
    
    // Draw rigid stone obstacles in Dark Gray/Cyan shaded rock glyphs
    printf("\033[1;36m");
    for (int i = 0; i < obstacle_count; i++) {
        gotoXY(obstacles[i].pos.x, obstacles[i].pos.y);
        printf("▓");
    }
    
    printf("\033[0m"); // Reset colors
    fflush(stdout);
}

void render_delta(Position prev_player, Position prev_enemies[MAX_ENEMIES]) {
    // 1. Wipe previous dynamic coordinate locations with a standard space cell
    gotoXY(prev_player.x, prev_player.y);
    putchar(' ');
    
    for (int i = 0; i < enemy_count; i++) {
        gotoXY(prev_enemies[i].x, prev_enemies[i].y);
        putchar(' ');
    }
    
    // 2. Draw Food item in Shiny Gold/Yellow Star (Dynamic Twinkling Star)
    if (food.active) {
        printf("\033[1;33m");
        gotoXY(food.pos.x, food.pos.y);
        if (frame_tick % 2 == 0) {
            printf("★"); // Solid Twinkle Star
        } else {
            printf("☆"); // Hollow Twinkle Star (Unicode \u2606)
        }
    }
    
    // 3. Draw Escape Gate in Magenta Shaded Portal if active (Liquid Wave Pulse vortex)
    if (escape_gate.active) {
        gotoXY(escape_gate.pos.x, escape_gate.pos.y);
        int density = frame_tick % 4;
        if (density == 0) {
            printf("\033[1;35m░\033[0m"); // Light Shading
        } else if (density == 1 || density == 3) {
            printf("\033[1;5;35m▒\033[0m"); // Medium Blinking Shading
        } else {
            printf("\033[1;35m▓\033[0m"); // Heavy Shading
        }
    }
    
    // 4. Draw Caveman (Player) walking animation in Green (Multi-state running cycle)
    printf("\033[1;32m");
    gotoXY(player.pos.x, player.pos.y);
    int p_state = frame_tick % 4;
    if (p_state == 0) {
        putchar('C'); // Signature base character
    } else if (p_state == 2) {
        printf("☻"); // Idle smiley state
    } else {
        // Walk direction arrows indicating active heading
        switch (player.dir) {
            case DIR_UP:    printf("▲"); break;
            case DIR_DOWN:  printf("▼"); break;
            case DIR_LEFT:  printf("◄"); break;
            case DIR_RIGHT: printf("►"); break;
            default:        printf("☻"); break; 
        }
    }
    
    // 5. Draw unique colored and stylized beasts with dynamic proximity rage animations
    const char* beast_colors[8] = {
        "\033[1;31m",      // 0: Bold Red
        "\033[1;33m",      // 1: Bold Yellow/Gold
        "\033[1;35m",      // 2: Bold Magenta/Purple
        "\033[1;36m",      // 3: Bold Cyan
        "\033[1;34m",      // 4: Bold Blue
        "\033[38;5;208m",  // 5: Bold Orange
        "\033[38;5;130m",  // 6: Bold Brown
        "\033[1;37m"       // 7: Bold White
    };
    const char beast_signatures[8] = {'B', 'M', 'D', 'H', 'Z', 'X', 'W', 'V'};
    const char* beast_claws[8] = {"Ψ", "Ω", "☠", "♦", "♣", "▲", "☼", "╬"};

    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].active) {
            int style_idx = i % 8;
            
            // Calculate distance to Caveman (Player) to trigger proximity rage mode
            double dx = enemies[i].pos.x - player.pos.x;
            double dy = enemies[i].pos.y - player.pos.y;
            double distance = sqrt((dx * dx) + (dy * dy));
            
            gotoXY(enemies[i].pos.x, enemies[i].pos.y);
            
            if (distance <= 5.0) {
                // RAGE MODE: Beast is within 5 tiles. Flash aggressively with red skull warning!
                if (frame_tick % 2 == 0) {
                    printf("\033[1;5;31m☠\033[0m"); // Red Blinking Warning Skull!
                } else {
                    printf("\033[1;31m%c\033[0m", beast_signatures[style_idx]); // Red beast warning letter
                }
            } else {
                // Normal Stalking Mode: Calm, smooth color-coded dynamic crawls
                printf("%s", beast_colors[style_idx]);
                if ((frame_tick / 2) % 2 == 0) {
                    putchar(beast_signatures[style_idx]);
                } else {
                    printf("%s", beast_claws[style_idx]);
                }
            }
        }
    }
    
    // 6. Draw Dashboard/HUD below the playing grid
    printf("\033[0m"); // Reset
    render_ui();
    
    fflush(stdout);
}

void render_ui(void) {
    // Draw status messages and score counters at fixed UI rows
    gotoXY(2, GRID_HEIGHT);
    printf("============================================================");
    
    gotoXY(2, GRID_HEIGHT + 1);
    printf("\033[1;37mNCIT POKHARA UNIVERSITY - BCA SEMESTER MAJOR PROJECT\033[0m");
    
    gotoXY(2, GRID_HEIGHT + 2);
    printf("Caveman [C/☻] | Beast [B/Ψ] | Food [★] | Portal [▒] (Goal: 10 Stars per level)");
    
    gotoXY(2, GRID_HEIGHT + 3);
    printf("Level: \033[1;36m%d\033[0m | Score: \033[1;32m%d\033[0m | High Score: \033[1;33m%d (%s)\033[0m  ", current_level, current_score, high_score, high_score_name);
    
    gotoXY(2, GRID_HEIGHT + 4);
    if (escape_gate.active) {
        printf("\033[1;35m[ALERT] ESCAPE GATE 'G' ACTIVATED AT THE RIGHT WALL! RUN!\033[0m      ");
    } else {
        printf("Collect Food to open the Escape Gate...                     ");
    }
    
    gotoXY(2, GRID_HEIGHT + 5);
    printf("Controls: WASD/Arrows to Navigate | ESC to Quit");
}

void render_game_over(int won) {
    // Clean screen and show final scores
    printf("\033[2J\033[H");
    printf("\033[1;37m");
    printf("============================================================\n");
    printf("                      GAME OVER                             \n");
    printf("============================================================\n\n");
    
    if (won) {
        printf("\033[1;32m   CONGRATULATIONS! You reached the gate and escaped the Beast!\n\033[0m");
    } else {
        printf("\033[1;31m   DEFEAT! The Beast caught you or you hit a rigid stone obstacle!\n\033[0m");
    }
    
    printf("\n   Final Score: %d\n", current_score);
    printf("   High Score:  %d\n\n", high_score);
    printf("============================================================\n");
    printf("Submitted by: Sandesh Khatri, Aarush Shah, Gautam KC (June 2026)\n");
    printf("============================================================\n");
    fflush(stdout);
}

void render_game_over_overlay(int won, int is_new_record, const char *record_holder) {
    int start_x = 12;
    int start_y = 4;
    
    // Draw outer boundary of overlay box in Bold White
    printf("\033[1;37m");
    
    gotoXY(start_x, start_y);
    printf("╔══════════════════════════════════╗");
    
    gotoXY(start_x, start_y + 1);
    printf("║");
    printf("\033[1;31m            GAME OVER             "); // High Intensity Red Title
    printf("\033[1;37m║");
    
    gotoXY(start_x, start_y + 2);
    printf("╠══════════════════════════════════╣");
    
    gotoXY(start_x, start_y + 3);
    printf("║");
    if (won) {
        printf("\033[1;32m  Result: ESCAPED SUCCESSFULLY!   "); // Green Victory Message
    } else {
        printf("\033[1;31m  Result: CAUGHT BY THE BEAST!    "); // Red Defeat Message
    }
    printf("\033[1;37m║");
    
    gotoXY(start_x, start_y + 4);
    printf("║  Final Score: \033[1;32m%-18d\033[1;37m║", current_score);
    
    gotoXY(start_x, start_y + 5);
    printf("║  High Score:  \033[1;33m%-18d\033[1;37m║", high_score);
    
    gotoXY(start_x, start_y + 6);
    printf("║");
    if (is_new_record) {
        printf("\033[1;33m  * RECORD HOLDER: %-15.15s* ", record_holder);
    } else {
        printf("  Holder: %-23.23s", record_holder);
    }
    printf("\033[1;37m║");
    
    gotoXY(start_x, start_y + 7);
    printf("╠══════════════════════════════════╣");
    
    gotoXY(start_x, start_y + 8);
    printf("║  \033[1;32mPress [Y] to Play Again        \033[1;37m║");
    
    gotoXY(start_x, start_y + 9);
    printf("║  \033[1;35mPress [N] or [ESC] to Exit     \033[1;37m║");
    
    gotoXY(start_x, start_y + 10);
    printf("╚══════════════════════════════════╝");
    
    printf("\033[0m"); // Reset ANSI colors
    fflush(stdout);
}

int check_input_hit(void) {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

Direction get_player_input(void) {
    Direction latest_dir = player.dir;
    char ch;
    
    // Process and flush all pending inputs in the queue, keeping only the latest valid steering command
    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == 27) { // ESC or arrow escape sequence
            // Wait a brief microsecond for the rest of the sequence to buffer if needed
            usleep(1000);
            if (check_input_hit()) {
                char next_ch;
                if (read(STDIN_FILENO, &next_ch, 1) == 1 && next_ch == '[') {
                    char arrow;
                    if (read(STDIN_FILENO, &arrow, 1) == 1) {
                        switch (arrow) {
                            case 'A': latest_dir = DIR_UP; break;
                            case 'B': latest_dir = DIR_DOWN; break;
                            case 'C': latest_dir = DIR_RIGHT; break;
                            case 'D': latest_dir = DIR_LEFT; break;
                        }
                    }
                }
            } else {
                // If it is a standalone ESC strike, set active game flag to false
                game_active = 0;
            }
        } else {
            // Process WASD commands
            switch (ch) {
                case 'w': case 'W': latest_dir = DIR_UP; break;
                case 's': case 'S': latest_dir = DIR_DOWN; break;
                case 'a': case 'A': latest_dir = DIR_LEFT; break;
                case 'd': case 'D': latest_dir = DIR_RIGHT; break;
                default: break;
            }
        }
    }
    
    return latest_dir;
}
