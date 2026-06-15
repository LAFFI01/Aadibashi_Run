import os

# --- C GAME ---
# 1. include/common.h
with open("include/common.h", "r") as f: content = f.read()
if "extern int num_players;" not in content:
    content = content.replace("extern Player player2;", "extern Player player2;\nextern int num_players;")
    with open("include/common.h", "w") as f: f.write(content)

# 2. src/physics.c
with open("src/physics.c", "r") as f: content = f.read()
if "int num_players = 1;" not in content:
    content = content.replace("Player player2;", "Player player2;\nint num_players = 1;")
    
    init_old = "player2.pos = (Position){2, 3};\n    player2.dir = DIR_NONE;"
    init_new = "if (num_players == 2) {\n        player2.pos = (Position){2, 3};\n        player2.dir = DIR_NONE;\n    } else {\n        player2.pos = (Position){-1, -1};\n    }"
    content = content.replace(init_old, init_new)
    
    # physics logic
    phys_old = "Position next_player2_pos = player2.pos;"
    phys_new = "if (num_players == 2) {\n        Position next_player2_pos = player2.pos;"
    content = content.replace(phys_old, phys_new)
    
    content = content.replace("player2.pos = next_player2_pos;", "player2.pos = next_player2_pos;\n    }")

    # For collision checks, if num_players == 1, player2.pos is (-1,-1) which is outside grid and safe.
    # No need to change the food/gate/enemy checks since (-1,-1) won't collide.
    with open("src/physics.c", "w") as f: f.write(content)

# 3. src/enemy_ai.c
with open("src/enemy_ai.c", "r") as f: content = f.read()
if "if (num_players == 1)" not in content:
    ai_old = "double d1 = sqrt(pow(enemies[e].pos.x - player.pos.x, 2) + pow(enemies[e].pos.y - player.pos.y, 2));\n        double d2 = sqrt(pow(enemies[e].pos.x - player2.pos.x, 2) + pow(enemies[e].pos.y - player2.pos.y, 2));\n        Position target = (d1 < d2) ? player.pos : player2.pos;"
    ai_new = "Position target = player.pos;\n        if (num_players == 2) {\n            double d1 = sqrt(pow(enemies[e].pos.x - player.pos.x, 2) + pow(enemies[e].pos.y - player.pos.y, 2));\n            double d2 = sqrt(pow(enemies[e].pos.x - player2.pos.x, 2) + pow(enemies[e].pos.y - player2.pos.y, 2));\n            if (d2 < d1) target = player2.pos;\n        }"
    content = content.replace(ai_old, ai_new)
    with open("src/enemy_ai.c", "w") as f: f.write(content)

# 4. src/display.c
with open("src/display.c", "r") as f: content = f.read()
if "if (num_players == 2)" not in content:
    # render
    draw_p2_old = "printf(\"\\033[1;36m\"); // Cyan color for player 2\n    gotoXY(player2.pos.x, player2.pos.y);"
    draw_p2_new = "if (num_players == 2) {\n    printf(\"\\033[1;36m\"); // Cyan color for player 2\n    gotoXY(player2.pos.x, player2.pos.y);"
    content = content.replace(draw_p2_old, draw_p2_new)
    
    close_brace_old = "default:        printf(\"☻\"); break; \n        }\n    }"
    close_brace_new = "default:        printf(\"☻\"); break; \n        }\n    }\n    }"
    content = content.replace(close_brace_old, close_brace_new, 1) # Only replace the second occurrence ideally
    # Wait, simple replace might break. Let's do a more robust replace for the closing brace.
    content = content.replace("""        switch (player2.dir) {
            case DIR_UP:    printf("▲"); break;
            case DIR_DOWN:  printf("▼"); break;
            case DIR_LEFT:  printf("◄"); break;
            case DIR_RIGHT: printf("►"); break;
            default:        printf("☻"); break; 
        }
    }""", """        switch (player2.dir) {
            case DIR_UP:    printf("▲"); break;
            case DIR_DOWN:  printf("▼"); break;
            case DIR_LEFT:  printf("◄"); break;
            case DIR_RIGHT: printf("►"); break;
            default:        printf("☻"); break; 
        }
    }
    }""")
    
    # erase p2
    erase_p2_old = "gotoXY(prev_player2.x, prev_player2.y);\n    putchar(' ');"
    erase_p2_new = "if (num_players == 2) {\n        gotoXY(prev_player2.x, prev_player2.y);\n        putchar(' ');\n    }"
    content = content.replace(erase_p2_old, erase_p2_new)
    
    # input
    input_p2_old = "case 'A': player2.dir = DIR_UP; break;"
    input_p2_new = "case 'A': if(num_players==2) player2.dir = DIR_UP; break;"
    content = content.replace(input_p2_old, input_p2_new)
    content = content.replace("case 'B': player2.dir = DIR_DOWN; break;", "case 'B': if(num_players==2) player2.dir = DIR_DOWN; break;")
    content = content.replace("case 'C': player2.dir = DIR_RIGHT; break;", "case 'C': if(num_players==2) player2.dir = DIR_RIGHT; break;")
    content = content.replace("case 'D': player2.dir = DIR_LEFT; break;", "case 'D': if(num_players==2) player2.dir = DIR_LEFT; break;")
    
    with open("src/display.c", "w") as f: f.write(content)

# 5. src/main.c
with open("src/main.c", "r") as f: content = f.read()
if "SELECT NUMBER OF PLAYERS" not in content:
    menu_code = """        // Show player selection menu
        printf("\\033[2J\\033[H");
        printf("\\033[1;36m");
        printf("============================================================\\n");
        printf("                SELECT NUMBER OF PLAYERS                    \\n");
        printf("============================================================\\n\\n");
        printf("             [1] Single Player (WASD)\\n");
        printf("             [2] Two Players (WASD & Arrows)\\n\\n");
        printf("             [ESC] Quit\\n");
        printf("============================================================\\n");
        fflush(stdout);

        int waiting_for_players = 1;
        while (waiting_for_players) {
            if (check_input_hit()) {
                char ch;
                if (read(STDIN_FILENO, &ch, 1) == 1) {
                    if (ch == '1') {
                        num_players = 1;
                        waiting_for_players = 0;
                    } else if (ch == '2') {
                        num_players = 2;
                        waiting_for_players = 0;
                    } else if (ch == 27) { // ESC
                        return 0; // Exit game
                    }
                }
            }
            usleep(50000);
        }
        
        // 3. Initialize game states
        init_game();"""
    content = content.replace("// 3. Initialize game states\n        init_game();", menu_code)
    with open("src/main.c", "w") as f: f.write(content)

print("C patched.")

# --- WEB GAME ---
# 1. index.html
with open("web/index.html", "r") as f: content = f.read()
if "id=\"btn1P\"" not in content:
    html_old = """                                <div id="usernameInputContainer" class="form-group">
                                    <label for="usernameInput" class="form-label">ENTER YOUR INITIALS:</label>
                                    <input type="text" id="usernameInput" class="retro-input" maxlength="15" placeholder="NCIT HERO" value="Aadibashi">
                                </div>"""
    html_new = html_old + """

                                <div class="form-group" style="margin-top: 15px;">
                                    <label class="form-label">PLAYERS:</label>
                                    <div style="display: flex; gap: 10px; margin-top: 5px;">
                                        <button id="btn1P" class="retro-btn" style="flex: 1; border: 2px solid #32ff32; background: rgba(50,255,50,0.2);">1 PLAYER</button>
                                        <button id="btn2P" class="retro-btn" style="flex: 1; border: 2px solid transparent; opacity: 0.7;">2 PLAYERS</button>
                                    </div>
                                </div>"""
    content = content.replace(html_old, html_new)
    with open("web/index.html", "w") as f: f.write(content)

# 2. game.js
with open("web/game.js", "r") as f: content = f.read()
if "let numPlayers = 1;" not in content:
    content = content.replace("let playerLives = 3;", "let playerLives = 3;\nlet numPlayers = 1;")
    
    js_init_old = "player2.dir = 'NONE';"
    js_init_new = "player2.dir = 'NONE';\n    if(numPlayers === 1) { player2.x = -1; player2.y = -1; }"
    content = content.replace(js_init_old, js_init_new)
    
    js_loop_old = "let nextX2 = player2.x;"
    js_loop_new = "if (numPlayers === 2) {\n    let nextX2 = player2.x;"
    content = content.replace(js_loop_old, js_loop_new)
    
    js_loop_old2 = """            player2.dir = 'NONE';
        }
    }"""
    js_loop_new2 = """            player2.dir = 'NONE';
        }
    }
    }"""
    content = content.replace(js_loop_old2, js_loop_new2)
    
    # draw p2 check
    draw_p2_old = "drawCaveman(player2.x * cellWidth + cellWidth / 2, player2.y * cellHeight + cellHeight / 2, player2.dir, frameTick);"
    draw_p2_new = "if (numPlayers === 2) drawCaveman(player2.x * cellWidth + cellWidth / 2, player2.y * cellHeight + cellHeight / 2, player2.dir, frameTick);"
    content = content.replace(draw_p2_old, draw_p2_new)
    
    # buttons setup
    btn_setup = """// Overlay start buttons
document.getElementById('btn1P').addEventListener('click', () => {
    numPlayers = 1;
    document.getElementById('btn1P').style.border = '2px solid #32ff32';
    document.getElementById('btn1P').style.background = 'rgba(50,255,50,0.2)';
    document.getElementById('btn1P').style.opacity = '1';
    document.getElementById('btn2P').style.border = '2px solid transparent';
    document.getElementById('btn2P').style.background = 'transparent';
    document.getElementById('btn2P').style.opacity = '0.7';
});

document.getElementById('btn2P').addEventListener('click', () => {
    numPlayers = 2;
    document.getElementById('btn2P').style.border = '2px solid #32ff32';
    document.getElementById('btn2P').style.background = 'rgba(50,255,50,0.2)';
    document.getElementById('btn2P').style.opacity = '1';
    document.getElementById('btn1P').style.border = '2px solid transparent';
    document.getElementById('btn1P').style.background = 'transparent';
    document.getElementById('btn1P').style.opacity = '0.7';
});

document.getElementById('startBtn').addEventListener('click', () => {"""
    content = content.replace("// Overlay start buttons\ndocument.getElementById('startBtn').addEventListener('click', () => {", btn_setup)

    with open("web/game.js", "w") as f: f.write(content)

print("Web patched.")
