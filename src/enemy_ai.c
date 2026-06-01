#include <math.h>
#include <limits.h>
#include <stddef.h>
#include "enemy_ai.h"
#include "physics.h"
#include "common.h"

void update_enemy_ai(void) {
    if (!game_active) return;
    
    // Game pacing constraint: Enemy moves every 2 loop ticks
    static int tick_counter = 0;
    tick_counter++;
    if (tick_counter % 2 != 0) {
        return;
    }
    
    // Check 4 directional candidates: UP, DOWN, LEFT, RIGHT
    Position moves[4] = {
        {0, -1}, // UP
        {0, 1},  // DOWN
        {-1, 0}, // LEFT
        {1, 0}   // RIGHT
    };
    
    for (int e = 0; e < enemy_count; e++) {
        if (!enemies[e].active) continue;
        
        double min_distance = 1e9; // Initialize with a large double value
        Position best_pos = enemies[e].pos;
        int move_found = 0;
        
        for (int i = 0; i < 4; i++) {
            Position candidate;
            candidate.x = enemies[e].pos.x + moves[i].x;
            candidate.y = enemies[e].pos.y + moves[i].y;
            
            // Ensure the move is within boundaries and does not collide with obstacles/walls
            if (check_collision(candidate)) {
                continue;
            }
            
            // Prevent the Beast from stepping on another active Beast (Mutual Avoidance)
            int occupied_by_other_beast = 0;
            for (int other_e = 0; other_e < enemy_count; other_e++) {
                if (other_e != e && enemies[other_e].active) {
                    if (candidate.x == enemies[other_e].pos.x && candidate.y == enemies[other_e].pos.y) {
                        occupied_by_other_beast = 1;
                        break;
                    }
                }
            }
            if (occupied_by_other_beast) {
                continue;
            }
            
            // Prevent the Beast from sitting on the Escape Gate
            if (escape_gate.active && candidate.x == escape_gate.pos.x && candidate.y == escape_gate.pos.y) {
                continue;
            }
            
            // Calculate Euclidean distance to Caveman (Player)
            double dx = candidate.x - player.pos.x;
            double dy = candidate.y - player.pos.y;
            double distance = sqrt((dx * dx) + (dy * dy));
            
            if (distance < min_distance) {
                min_distance = distance;
                best_pos = candidate;
                move_found = 1;
            }
        }
        
        if (move_found) {
            enemies[e].pos = best_pos;
        }
        
        // Evaluate if Beast caught the Caveman after moving
        if (enemies[e].pos.x == player.pos.x && enemies[e].pos.y == player.pos.y) {
            game_active = 0;
            player_won = 0;
        }
    }
}
