import os

# patch include/common.h
path = "include/common.h"
with open(path, "r") as f:
    content = f.read()
if "extern Player player2;" not in content:
    content = content.replace("extern Player player;\n", "extern Player player;\nextern Player player2;\n")
    with open(path, "w") as f:
        f.write(content)

# patch src/display.h
path = "include/display.h"
with open(path, "r") as f:
    content = f.read()
content = content.replace("Direction get_player_input(void);", "void get_player_input(void);")
with open(path, "w") as f:
    f.write(content)

# patch src/main.c
path = "src/main.c"
with open(path, "r") as f:
    content = f.read()
content = content.replace("Position prev_player = player.pos;", "Position prev_player = player.pos;\n        Position prev_player2 = player2.pos;")
content = content.replace("prev_player = player.pos;", "prev_player = player.pos;\n            prev_player2 = player2.pos;")
content = content.replace("player.dir = get_player_input();", "get_player_input();")
content = content.replace("render_delta(prev_player, prev_enemies);", "render_delta(prev_player, prev_player2, prev_enemies);")
with open(path, "w") as f:
    f.write(content)

# patch src/display.c
path = "src/display.c"
with open(path, "r") as f:
    content = f.read()
content = content.replace("void render_delta(Position prev_player, Position prev_enemies[MAX_ENEMIES]) {", "void render_delta(Position prev_player, Position prev_player2, Position prev_enemies[MAX_ENEMIES]) {")
content = content.replace("gotoXY(prev_player.x, prev_player.y);\n    putchar(' ');", "gotoXY(prev_player.x, prev_player.y);\n    putchar(' ');\n    gotoXY(prev_player2.x, prev_player2.y);\n    putchar(' ');")

# Add drawing player2
player_draw_code = """    // 4. Draw Caveman (Player) walking animation in Green (Multi-state running cycle)
    printf("\\033[1;32m");
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
    
    // Draw Player 2
    printf("\\033[1;36m"); // Cyan color for player 2
    gotoXY(player2.pos.x, player2.pos.y);
    if (p_state == 0) {
        putchar('P');
    } else if (p_state == 2) {
        printf("☻");
    } else {
        switch (player2.dir) {
            case DIR_UP:    printf("▲"); break;
            case DIR_DOWN:  printf("▼"); break;
            case DIR_LEFT:  printf("◄"); break;
            case DIR_RIGHT: printf("►"); break;
            default:        printf("☻"); break; 
        }
    }"""
content = content.replace("    // 4. Draw Caveman (Player) walking animation in Green (Multi-state running cycle)\n    printf(\"\\033[1;32m\");\n    gotoXY(player.pos.x, player.pos.y);\n    int p_state = frame_tick % 4;\n    if (p_state == 0) {\n        putchar('C'); // Signature base character\n    } else if (p_state == 2) {\n        printf(\"☻\"); // Idle smiley state\n    } else {\n        // Walk direction arrows indicating active heading\n        switch (player.dir) {\n            case DIR_UP:    printf(\"▲\"); break;\n            case DIR_DOWN:  printf(\"▼\"); break;\n            case DIR_LEFT:  printf(\"◄\"); break;\n            case DIR_RIGHT: printf(\"►\"); break;\n            default:        printf(\"☻\"); break; \n        }\n    }", player_draw_code)

# Calculate distance to Caveman for both players
content = content.replace("double dx = enemies[i].pos.x - player.pos.x;\n            double dy = enemies[i].pos.y - player.pos.y;\n            double distance = sqrt((dx * dx) + (dy * dy));", """double dx1 = enemies[i].pos.x - player.pos.x;
            double dy1 = enemies[i].pos.y - player.pos.y;
            double dist1 = sqrt((dx1 * dx1) + (dy1 * dy1));
            double dx2 = enemies[i].pos.x - player2.pos.x;
            double dy2 = enemies[i].pos.y - player2.pos.y;
            double dist2 = sqrt((dx2 * dx2) + (dy2 * dy2));
            double distance = dist1 < dist2 ? dist1 : dist2;""")

# Update get_player_input
content = content.replace("Direction get_player_input(void)", "void get_player_input(void)")
input_code = """void get_player_input(void) {
    char ch;
    
    // Process and flush all pending inputs in the queue
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
                            case 'A': player2.dir = DIR_UP; break;
                            case 'B': player2.dir = DIR_DOWN; break;
                            case 'C': player2.dir = DIR_RIGHT; break;
                            case 'D': player2.dir = DIR_LEFT; break;
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
                case 'w': case 'W': player.dir = DIR_UP; break;
                case 's': case 'S': player.dir = DIR_DOWN; break;
                case 'a': case 'A': player.dir = DIR_LEFT; break;
                case 'd': case 'D': player.dir = DIR_RIGHT; break;
                default: break;
            }
        }
    }
}"""
content = content.split("void get_player_input(void) {")[0] + input_code
with open(path, "w") as f:
    f.write(content)

# patch src/display.h again to fix my previous display.h modifying just in case
path = "include/display.h"
with open(path, "r") as f:
    content = f.read()
content = content.replace("void render_delta(Position prev_player, Position prev_enemies[MAX_ENEMIES]);", "void render_delta(Position prev_player, Position prev_player2, Position prev_enemies[MAX_ENEMIES]);")
with open(path, "w") as f:
    f.write(content)

# patch src/physics.c
path = "src/physics.c"
with open(path, "r") as f:
    content = f.read()

content = content.replace("Player player;\n", "Player player;\nPlayer player2;\n")
content = content.replace("player.pos = (Position){2, 2};\n    player.dir = DIR_NONE;", "player.pos = (Position){2, 2};\n    player.dir = DIR_NONE;\n    player2.pos = (Position){2, 3};\n    player2.dir = DIR_NONE;")
content = content.replace("player.pos = (Position){2, 2};\n        player.dir = DIR_NONE;", "player.pos = (Position){2, 2};\n        player.dir = DIR_NONE;\n        player2.pos = (Position){2, 3};\n        player2.dir = DIR_NONE;")

# update spawn_food check
content = content.replace("if (temp_pos.x == player.pos.x && temp_pos.y == player.pos.y) {\n            continue;\n        }", "if ((temp_pos.x == player.pos.x && temp_pos.y == player.pos.y) || (temp_pos.x == player2.pos.x && temp_pos.y == player2.pos.y)) {\n            continue;\n        }")
content = content.replace("rand_pos.x != player.pos.x || rand_pos.y != player.pos.y", "(rand_pos.x != player.pos.x || rand_pos.y != player.pos.y) && (rand_pos.x != player2.pos.x || rand_pos.y != player2.pos.y)")

# update physics logic
physics_logic_old = """    Position next_player_pos = player.pos;
    
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
    
    player.pos = next_player_pos;"""
physics_logic_new = """    Position next_player_pos = player.pos;
    switch (player.dir) {
        case DIR_UP:    next_player_pos.y--; break;
        case DIR_DOWN:  next_player_pos.y++; break;
        case DIR_LEFT:  next_player_pos.x--; break;
        case DIR_RIGHT: next_player_pos.x++; break;
        default: break;
    }
    if (player.dir != DIR_NONE && check_collision(next_player_pos)) {
        next_player_pos = player.pos;
        player.dir = DIR_NONE;
    }
    player.pos = next_player_pos;

    Position next_player2_pos = player2.pos;
    switch (player2.dir) {
        case DIR_UP:    next_player2_pos.y--; break;
        case DIR_DOWN:  next_player2_pos.y++; break;
        case DIR_LEFT:  next_player2_pos.x--; break;
        case DIR_RIGHT: next_player2_pos.x++; break;
        default: break;
    }
    if (player2.dir != DIR_NONE && check_collision(next_player2_pos)) {
        next_player2_pos = player2.pos;
        player2.dir = DIR_NONE;
    }
    player2.pos = next_player2_pos;"""
content = content.replace(physics_logic_old, physics_logic_new)

content = content.replace("if (player.pos.x == food.pos.x && player.pos.y == food.pos.y) {", "if ((player.pos.x == food.pos.x && player.pos.y == food.pos.y) || (player2.pos.x == food.pos.x && player2.pos.y == food.pos.y)) {")
content = content.replace("if (escape_gate.active && player.pos.x == escape_gate.pos.x && player.pos.y == escape_gate.pos.y) {", "if (escape_gate.active && ((player.pos.x == escape_gate.pos.x && player.pos.y == escape_gate.pos.y) || (player2.pos.x == escape_gate.pos.x && player2.pos.y == escape_gate.pos.y))) {")
content = content.replace("if (enemies[i].active && player.pos.x == enemies[i].pos.x && player.pos.y == enemies[i].pos.y) {", "if (enemies[i].active && ((player.pos.x == enemies[i].pos.x && player.pos.y == enemies[i].pos.y) || (player2.pos.x == enemies[i].pos.x && player2.pos.y == enemies[i].pos.y))) {")
with open(path, "w") as f:
    f.write(content)

# patch src/enemy_ai.c
path = "src/enemy_ai.c"
with open(path, "r") as f:
    content = f.read()

# Choose target based on distance
ai_target_old = "Position target = player.pos;"
ai_target_new = """double d1 = sqrt(pow(enemies[e].pos.x - player.pos.x, 2) + pow(enemies[e].pos.y - player.pos.y, 2));
        double d2 = sqrt(pow(enemies[e].pos.x - player2.pos.x, 2) + pow(enemies[e].pos.y - player2.pos.y, 2));
        Position target = (d1 < d2) ? player.pos : player2.pos;"""
content = content.replace(ai_target_old, ai_target_new)
content = content.replace("if (enemies[e].pos.x == player.pos.x && enemies[e].pos.y == player.pos.y) {", "if ((enemies[e].pos.x == player.pos.x && enemies[e].pos.y == player.pos.y) || (enemies[e].pos.x == player2.pos.x && enemies[e].pos.y == player2.pos.y)) {")
with open(path, "w") as f:
    f.write(content)

print("C patched")
