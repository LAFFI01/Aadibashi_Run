#include <stdlib.h>
#include <time.h>
#include "physics.h"
#include "common.h"

// Define Global State variables declared in common.h
int game_active = 0;
int player_won = 0;
unsigned int current_score = 0;
unsigned int high_score = 0;
char high_score_name[32] = "NCIT Student";
unsigned int frame_tick = 0;
Player player;
Enemy enemies[MAX_ENEMIES];
int enemy_count = 1;
unsigned int current_level = 1;
Target food;
Target escape_gate;
Obstacle obstacles[MAX_OBSTACLES];
int obstacle_count = 0;

static void generate_obstacles(void) {
    obstacle_count = 0;
    
    // Barrier Wall 1: Column 15, top and bottom gaps
    for (int y = 2; y <= 7 && obstacle_count < MAX_OBSTACLES; y++) {
        obstacles[obstacle_count++].pos = (Position){15, y};
    }
    for (int y = 12; y <= 17 && obstacle_count < MAX_OBSTACLES; y++) {
        obstacles[obstacle_count++].pos = (Position){15, y};
    }
    
    // Barrier Wall 2: Column 30, central wall with top/bottom escapes
    for (int y = 5; y <= 14 && obstacle_count < MAX_OBSTACLES; y++) {
        obstacles[obstacle_count++].pos = (Position){30, y};
    }
    
    // Barrier Wall 3: Column 45, top and bottom gaps
    for (int y = 2; y <= 7 && obstacle_count < MAX_OBSTACLES; y++) {
        obstacles[obstacle_count++].pos = (Position){45, y};
    }
    for (int y = 12; y <= 17 && obstacle_count < MAX_OBSTACLES; y++) {
        obstacles[obstacle_count++].pos = (Position){45, y};
    }
}

int check_collision(Position pos) {
    // 1. Boundary Wall Collisions
    if (pos.x <= 0 || pos.x >= GRID_WIDTH - 1 || pos.y <= 0 || pos.y >= GRID_HEIGHT - 1) {
        return 1; // Collision detected
    }
    
    // 2. Obstacle Collisions
    for (int i = 0; i < obstacle_count; i++) {
        if (obstacles[i].pos.x == pos.x && obstacles[i].pos.y == pos.y) {
            return 1; // Collision detected
        }
    }
    return 0; // Clear
}

void spawn_food(void) {
    int valid = 0;
    Position temp_pos;
    
    while (!valid) {
        // Generate within grid boundaries [1, GRID_WIDTH-2] and [1, GRID_HEIGHT-2]
        temp_pos.x = 1 + rand() % (GRID_WIDTH - 2);
        temp_pos.y = 1 + rand() % (GRID_HEIGHT - 2);
        
        // Ensure food does not spawn inside walls/obstacles
        if (check_collision(temp_pos)) {
            continue;
        }
        
        // Ensure food does not spawn on player
        if (temp_pos.x == player.pos.x && temp_pos.y == player.pos.y) {
            continue;
        }
        
        // Ensure food does not spawn on any active enemies
        int spawn_on_enemy = 0;
        for (int i = 0; i < enemy_count; i++) {
            if (temp_pos.x == enemies[i].pos.x && temp_pos.y == enemies[i].pos.y) {
                spawn_on_enemy = 1;
                break;
            }
        }
        if (spawn_on_enemy) {
            continue;
        }
        
        // Ensure food does not spawn on escape gate
        if (temp_pos.x == escape_gate.pos.x && temp_pos.y == escape_gate.pos.y) {
            continue;
        }
        
        valid = 1;
    }
    food.pos = temp_pos;
    food.active = 1;
}

void init_game(void) {
    srand((unsigned int)time(NULL));
    
    game_active = 1;
    player_won = 0;
    current_score = 0;
    frame_tick = 0;
    
    // Initialize Player in top-left zone
    player.pos = (Position){2, 2};
    player.dir = DIR_NONE;
    
    current_level = 1;
    enemy_count = 1;
    
    // Initialize Enemy (Beast) in bottom-right zone
    enemies[0].pos = (Position){GRID_WIDTH - 3, GRID_HEIGHT - 2};
    enemies[0].active = 1;
    
    // Generate the static cave layout of rocks
    generate_obstacles();
    
    // Initialize Escape Gate in middle-right zone (will be marked active after score milestone)
    escape_gate.pos = (Position){GRID_WIDTH - 2, GRID_HEIGHT / 2};
    escape_gate.active = 0;
    
    // Spawn initial target item
    spawn_food();
}

void update_physics(void) {
    if (!game_active) return;
    
    Position next_player_pos = player.pos;
    
    switch (player.dir) {
        case DIR_UP:    next_player_pos.y--; break;
        case DIR_DOWN:  next_player_pos.y++; break;
        case DIR_LEFT:  next_player_pos.x--; break;
        case DIR_RIGHT: next_player_pos.x++; break;
        default: break; // DIR_NONE: do not move
    }
    
    // Evaluate Obstacle or Border collision: Blocks movement instead of causing death
    if (player.dir != DIR_NONE && check_collision(next_player_pos)) {
        // Cancel movement coordinates and reset heading to idle state
        next_player_pos = player.pos;
        player.dir = DIR_NONE;
    }
    
    player.pos = next_player_pos;
    
    // Evaluate Item Collection
    if (player.pos.x == food.pos.x && player.pos.y == food.pos.y) {
        current_score += 10;
        
        // Activate Escape Gate if target score is met (10 stars = TARGET_SCORE score increase)
        if (current_score >= current_level * TARGET_SCORE) {
            escape_gate.active = 1;
        }
        spawn_food();
    }
    
    // Evaluate Escape Success (Level Transition)
    if (escape_gate.active && player.pos.x == escape_gate.pos.x && player.pos.y == escape_gate.pos.y) {
        current_level++;
        
        // Add 1 Beast on each level progression up to the maximum constraints
        if (enemy_count < MAX_ENEMIES) {
            enemy_count++;
        }
        
        // Reset positions for next level progression wave
        player.pos = (Position){2, 2};
        player.dir = DIR_NONE;
        escape_gate.active = 0;
        
        // Initialize all active enemies strategically scattered around the arena
        for (int i = 0; i < enemy_count; i++) {
            enemies[i].active = 1;
            if (i == 0) {
                enemies[i].pos = (Position){GRID_WIDTH - 3, GRID_HEIGHT - 2}; // Bottom-Right
            } else if (i == 1) {
                enemies[i].pos = (Position){GRID_WIDTH - 3, 2}; // Top-Right
            } else if (i == 2) {
                enemies[i].pos = (Position){2, GRID_HEIGHT - 2}; // Bottom-Left
            } else if (i == 3) {
                enemies[i].pos = (Position){GRID_WIDTH / 2, GRID_HEIGHT - 2}; // Bottom-Middle
            } else if (i == 4) {
                enemies[i].pos = (Position){GRID_WIDTH / 2, 2}; // Top-Middle
            } else {
                // Random spawn coordinate inside grid avoiding obstacles and player position
                int valid_spawn = 0;
                while (!valid_spawn) {
                    Position rand_pos;
                    rand_pos.x = 1 + rand() % (GRID_WIDTH - 2);
                    rand_pos.y = 1 + rand() % (GRID_HEIGHT - 2);
                    if (!check_collision(rand_pos) && (rand_pos.x != player.pos.x || rand_pos.y != player.pos.y)) {
                        enemies[i].pos = rand_pos;
                        valid_spawn = 1;
                    }
                }
            }
        }
        
        // Spawn first food star target for new level wave
        spawn_food();
        
        // Trigger full static redrawing of arena walls and obstacle elements
        extern void render_static_board(void);
        render_static_board();
        return;
    }
    
    // Evaluate Predator Collision across all active enemies
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].active && player.pos.x == enemies[i].pos.x && player.pos.y == enemies[i].pos.y) {
            game_active = 0;
            player_won = 0;
            return;
        }
    }
}
