import os

# --- 1. common.h ---
with open("include/common.h", "r") as f: content = f.read()
if "extern int player_lives;" not in content:
    content = content.replace("extern int num_players;", "extern int num_players;\nextern int player_lives;\nextern Target axe;\nextern int team_has_axe;")
    with open("include/common.h", "w") as f: f.write(content)

# --- 2. physics.h ---
with open("include/physics.h", "r") as f: content = f.read()
if "void reset_positions(void);" not in content:
    content = content.replace("void init_game(void);", "void init_game(void);\nvoid reset_positions(void);\nvoid handle_player_caught(int enemy_idx);")
    with open("include/physics.h", "w") as f: f.write(content)

# --- 3. physics.c ---
with open("src/physics.c", "r") as f: content = f.read()
if "int player_lives = 3;" not in content:
    content = content.replace("int num_players = 1;", "int num_players = 1;\nint player_lives = 3;\nTarget axe;\nint team_has_axe = 0;")
    
    reset_fn = """void reset_positions(void) {
    player.pos = (Position){2, 2};
    player.dir = DIR_NONE;
    if (num_players == 2) {
        player2.pos = (Position){2, 3};
        player2.dir = DIR_NONE;
    } else {
        player2.pos = (Position){-1, -1};
    }
    for (int i = 0; i < enemy_count; i++) {
        if (!enemies[i].active) continue;
        if (i == 0) enemies[i].pos = (Position){GRID_WIDTH - 3, GRID_HEIGHT - 2};
        else if (i == 1) enemies[i].pos = (Position){GRID_WIDTH - 3, 2};
        else if (i == 2) enemies[i].pos = (Position){2, GRID_HEIGHT - 2};
        else if (i == 3) enemies[i].pos = (Position){GRID_WIDTH / 2, GRID_HEIGHT - 2};
        else if (i == 4) enemies[i].pos = (Position){GRID_WIDTH / 2, 2};
        else {
            int valid_spawn = 0;
            while (!valid_spawn) {
                Position rand_pos = {1 + rand() % (GRID_WIDTH - 2), 1 + rand() % (GRID_HEIGHT - 2)};
                if (!check_collision(rand_pos) && 
                    (rand_pos.x != player.pos.x || rand_pos.y != player.pos.y) &&
                    (rand_pos.x != player2.pos.x || rand_pos.y != player2.pos.y)) {
                    enemies[i].pos = rand_pos;
                    valid_spawn = 1;
                }
            }
        }
    }
    extern void render_static_board(void);
    render_static_board();
}

void handle_player_caught(int enemy_idx) {
    if (team_has_axe) {
        enemies[enemy_idx].active = 0;
        team_has_axe = 0;
        return;
    }
    player_lives--;
    if (player_lives <= 0) {
        game_active = 0;
        player_won = 0;
    } else {
        reset_positions();
    }
}

void spawn_axe(void) {
    int valid = 0;
    Position temp_pos;
    while (!valid) {
        temp_pos.x = 1 + rand() % (GRID_WIDTH - 2);
        temp_pos.y = 1 + rand() % (GRID_HEIGHT - 2);
        if (check_collision(temp_pos)) continue;
        if ((temp_pos.x == player.pos.x && temp_pos.y == player.pos.y) || (temp_pos.x == player2.pos.x && temp_pos.y == player2.pos.y)) continue;
        if (temp_pos.x == food.pos.x && temp_pos.y == food.pos.y) continue;
        if (temp_pos.x == escape_gate.pos.x && temp_pos.y == escape_gate.pos.y) continue;
        int spawn_on_enemy = 0;
        for (int i = 0; i < enemy_count; i++) {
            if (enemies[i].active && temp_pos.x == enemies[i].pos.x && temp_pos.y == enemies[i].pos.y) {
                spawn_on_enemy = 1; break;
            }
        }
        if (spawn_on_enemy) continue;
        valid = 1;
    }
    axe.pos = temp_pos;
    axe.active = 1;
}"""
    content = content.replace("static void generate_obstacles(void) {", reset_fn + "\n\nstatic void generate_obstacles(void) {")
    
    # modify init_game
    content = content.replace("current_level = 1;", "current_level = 1;\n    player_lives = 3;\n    team_has_axe = 0;\n    axe.active = 0;")
    
    # replace level progression enemies spawn code with reset_positions()
    level_prog_old = """        // Reset positions for next level progression wave
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
                    if (!check_collision(rand_pos) && (rand_pos.x != player.pos.x || rand_pos.y != player.pos.y) && (rand_pos.x != player2.pos.x || rand_pos.y != player2.pos.y)) {
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
        render_static_board();"""
        
    level_prog_new = """        escape_gate.active = 0;
        for (int i = 0; i < enemy_count; i++) { enemies[i].active = 1; }
        
        team_has_axe = 0;
        if (current_level % 2 == 0 && current_level >= 4) {
            spawn_axe();
        } else {
            axe.active = 0;
        }
        
        spawn_food();
        reset_positions();"""
    content = content.replace(level_prog_old, level_prog_new)
    
    # Check axe pickup
    axe_pickup = """    if (axe.active && ((player.pos.x == axe.pos.x && player.pos.y == axe.pos.y) || (num_players == 2 && player2.pos.x == axe.pos.x && player2.pos.y == axe.pos.y))) {
        team_has_axe = 1;
        axe.active = 0;
    }"""
    content = content.replace("    // Evaluate Escape Success", axe_pickup + "\n\n    // Evaluate Escape Success")
    
    # Catch player logic
    catch_old = """    // Evaluate Predator Collision across all active enemies
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].active && ((player.pos.x == enemies[i].pos.x && player.pos.y == enemies[i].pos.y) || (num_players == 2 && player2.pos.x == enemies[i].pos.x && player2.pos.y == enemies[i].pos.y))) {
            game_active = 0;
            player_won = 0;
            return;
        }
    }"""
    catch_new = """    // Evaluate Predator Collision across all active enemies
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].active && ((player.pos.x == enemies[i].pos.x && player.pos.y == enemies[i].pos.y) || (num_players == 2 && player2.pos.x == enemies[i].pos.x && player2.pos.y == enemies[i].pos.y))) {
            handle_player_caught(i);
            return;
        }
    }"""
    content = content.replace(catch_old, catch_new)
    
    with open("src/physics.c", "w") as f: f.write(content)

# --- 4. enemy_ai.c ---
with open("src/enemy_ai.c", "r") as f: content = f.read()
if "handle_player_caught" not in content:
    catch_old = """        // Evaluate if Beast caught the Caveman after moving
        if ((enemies[e].pos.x == player.pos.x && enemies[e].pos.y == player.pos.y) || (num_players == 2 && enemies[e].pos.x == player2.pos.x && enemies[e].pos.y == player2.pos.y)) {
            game_active = 0;
            player_won = 0;
        }"""
    catch_new = """        // Evaluate if Beast caught the Caveman after moving
        if ((enemies[e].pos.x == player.pos.x && enemies[e].pos.y == player.pos.y) || (num_players == 2 && enemies[e].pos.x == player2.pos.x && enemies[e].pos.y == player2.pos.y)) {
            handle_player_caught(e);
        }"""
    content = content.replace(catch_old, catch_new)
    with open("src/enemy_ai.c", "w") as f: f.write(content)

# --- 5. display.c ---
with open("src/display.c", "r") as f: content = f.read()
if "Meat [🍖]" not in content:
    # 1. Wipe previous axe location
    content = content.replace("    for (int i = 0; i < enemy_count; i++) {", "    if (axe.active) {\n        gotoXY(axe.pos.x, axe.pos.y);\n        putchar(' ');\n    }\n    for (int i = 0; i < enemy_count; i++) {")

    # 2. Draw Food item in Shiny Gold/Yellow Star (Dynamic Twinkling Star)
    # replace ★ with 🍖
    content = content.replace("★", "🍖")
    content = content.replace("☆", "🍖")
    
    # 3. Draw Axe
    draw_axe = """    // 3b. Draw Axe
    if (axe.active) {
        printf("\\033[1;36m");
        gotoXY(axe.pos.x, axe.pos.y);
        printf("🪓");
    }"""
    content = content.replace("    // 3. Draw Escape Gate", draw_axe + "\n\n    // 3. Draw Escape Gate")
    
    # Update UI to match
    ui_old = """    gotoXY(2, GRID_HEIGHT + 2);
    printf("Caveman [C/☻] | Beast [B/Ψ] | Food [★] | Portal [▒] (Goal: 10 Stars per level)");"""
    ui_new = """    gotoXY(2, GRID_HEIGHT + 2);
    printf("Caveman [C/☻] | Beast [B/Ψ] | Meat [🍖] | Axe [🪓] | Portal [▒] (Goal: 10 Meat/lvl)");"""
    content = content.replace(ui_old, ui_new)
    
    ui_old2 = """    gotoXY(2, GRID_HEIGHT + 3);
    printf("Level: \\033[1;36m%d\\033[0m | Score: \\033[1;32m%d\\033[0m | High Score: \\033[1;33m%d (%s)\\033[0m  ", current_level, current_score, high_score, high_score_name);"""
    ui_new2 = """    gotoXY(2, GRID_HEIGHT + 3);
    printf("Level: \\033[1;36m%d\\033[0m | Score: \\033[1;32m%d\\033[0m | Lives: \\033[1;31m%d\\033[0m | Weapon: \\033[1;36m%s\\033[0m | Best: \\033[1;33m%d (%s)\\033[0m  ", 
        current_level, current_score, player_lives, team_has_axe ? "AXE" : "NONE", high_score, high_score_name);"""
    content = content.replace(ui_old2, ui_new2)
    
    ui_old3 = "printf(\"Collect Food to open the Escape Gate...                     \");"
    ui_new3 = "printf(\"Collect Meat to open the Escape Gate...                     \");"
    content = content.replace(ui_old3, ui_new3)

    with open("src/display.c", "w") as f: f.write(content)

print("Features added to C.")
