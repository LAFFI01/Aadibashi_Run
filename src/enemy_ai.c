#include <math.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
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
    
    for (int e = 0; e < enemy_count; e++) {
        if (!enemies[e].active) continue;
        
        // Use Breadth-First Search (BFS) pathfinder to determine the next optimal coordinate
        Position start = enemies[e].pos;
        Position target = player.pos;
        
        // Static structures for BFS coordinate queuing (Grid is 60 x 20 = 1200 nodes)
        static Position queue[1200];
        static int visited[GRID_WIDTH][GRID_HEIGHT];
        static Position parent[GRID_WIDTH][GRID_HEIGHT];
        
        memset(visited, 0, sizeof(visited));
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                parent[x][y] = (Position){-1, -1};
            }
        }
        
        int head = 0;
        int tail = 0;
        
        // Push start node
        queue[tail++] = start;
        visited[start.x][start.y] = 1;
        
        Position moves[4] = {
            {0, -1}, // UP
            {0, 1},  // DOWN
            {-1, 0}, // LEFT
            {1, 0}   // RIGHT
        };
        
        int path_found = 0;
        
        while (head < tail) {
            Position curr = queue[head++];
            
            if (curr.x == target.x && curr.y == target.y) {
                path_found = 1;
                break;
            }
            
            for (int i = 0; i < 4; i++) {
                Position next = {curr.x + moves[i].x, curr.y + moves[i].y};
                
                // Check boundary & obstacle collisions
                if (check_collision(next)) continue;
                
                // Avoid tiles occupied by other active beasts (Mutual Avoidance)
                int occupied_by_other_beast = 0;
                for (int other_e = 0; other_e < enemy_count; other_e++) {
                    if (other_e != e && enemies[other_e].active) {
                        if (next.x == enemies[other_e].pos.x && next.y == enemies[other_e].pos.y) {
                            occupied_by_other_beast = 1;
                            break;
                        }
                    }
                }
                if (occupied_by_other_beast) continue;
                
                // Avoid blocking escape gate
                if (escape_gate.active && next.x == escape_gate.pos.x && next.y == escape_gate.pos.y) {
                    continue;
                }
                
                if (!visited[next.x][next.y]) {
                    visited[next.x][next.y] = 1;
                    parent[next.x][next.y] = curr;
                    queue[tail++] = next;
                }
            }
        }
        
        // Trace back the shortest path to find the immediate next step from start position
        if (path_found) {
            Position curr = target;
            Position prev_step = target;
            while (curr.x != start.x || curr.y != start.y) {
                prev_step = curr;
                curr = parent[curr.x][curr.y];
            }
            enemies[e].pos = prev_step;
        } else {
            // Fallback to standard greedy vector if BFS somehow finds no path
            double min_distance = 1e9;
            Position best_pos = start;
            for (int i = 0; i < 4; i++) {
                Position next = {start.x + moves[i].x, start.y + moves[i].y};
                if (check_collision(next)) continue;
                
                double dx = next.x - target.x;
                double dy = next.y - target.y;
                double dist = sqrt(dx*dx + dy*dy);
                if (dist < min_distance) {
                    min_distance = dist;
                    best_pos = next;
                }
            }
            enemies[e].pos = best_pos;
        }
        
        // Evaluate if Beast caught the Caveman after moving
        if (enemies[e].pos.x == player.pos.x && enemies[e].pos.y == player.pos.y) {
            game_active = 0;
            player_won = 0;
        }
    }
}
