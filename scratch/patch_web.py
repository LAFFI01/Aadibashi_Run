import os

path = "web/game.js"
with open(path, "r") as f:
    content = f.read()

# Add player2 definition
content = content.replace("let player = { x: 2, y: 2, dir: 'NONE' };", "let player = { x: 2, y: 2, dir: 'NONE' };\nlet player2 = { x: 3, y: 2, dir: 'NONE' };")

# Update startGame
start_game_old = """    player.x = 2;
    player.y = 2;
    player.dir = 'NONE';"""
start_game_new = """    player.x = 2;
    player.y = 2;
    player.dir = 'NONE';
    player2.x = 3;
    player2.y = 2;
    player2.dir = 'NONE';"""
content = content.replace(start_game_old, start_game_new)

# handleDirectionInput
handle_dir_old = """function handleDirectionInput(dir) {
    if (!gameActive) return;
    player.dir = dir;
}"""
handle_dir_new = """function handleDirectionInput(dir, playerNum) {
    if (!gameActive) return;
    if (playerNum === 1) player.dir = dir;
    else if (playerNum === 2) player2.dir = dir;
}"""
content = content.replace(handle_dir_old, handle_dir_new)

# keydown listener
keydown_old = """    // Support WASD + Arrows
    switch (e.key) {
        case 'w': case 'W': case 'ArrowUp':
            handleDirectionInput('UP');
            break;
        case 's': case 'S': case 'ArrowDown':
            handleDirectionInput('DOWN');
            break;
        case 'a': case 'A': case 'ArrowLeft':
            handleDirectionInput('LEFT');
            break;
        case 'd': case 'D': case 'ArrowRight':
            handleDirectionInput('RIGHT');
            break;
        case 'Escape':
            gameOver(false);
            break;
    }"""
keydown_new = """    // Support WASD + Arrows
    switch (e.key) {
        case 'w': case 'W':
            handleDirectionInput('UP', 1);
            break;
        case 'ArrowUp':
            handleDirectionInput('UP', 2);
            break;
        case 's': case 'S':
            handleDirectionInput('DOWN', 1);
            break;
        case 'ArrowDown':
            handleDirectionInput('DOWN', 2);
            break;
        case 'a': case 'A':
            handleDirectionInput('LEFT', 1);
            break;
        case 'ArrowLeft':
            handleDirectionInput('LEFT', 2);
            break;
        case 'd': case 'D':
            handleDirectionInput('RIGHT', 1);
            break;
        case 'ArrowRight':
            handleDirectionInput('RIGHT', 2);
            break;
        case 'Escape':
            gameOver(false);
            break;
    }"""
content = content.replace(keydown_old, keydown_new)

# Mobile controls -> default to player 1
mobile_btn_old = "handleDirectionInput('UP');"
mobile_btn_new = "handleDirectionInput('UP', 1);"
content = content.replace(mobile_btn_old, mobile_btn_new)
content = content.replace("handleDirectionInput('DOWN');", "handleDirectionInput('DOWN', 1);")
content = content.replace("handleDirectionInput('LEFT');", "handleDirectionInput('LEFT', 1);")
content = content.replace("handleDirectionInput('RIGHT');", "handleDirectionInput('RIGHT', 1);")

# Game tick loop updates
tick_p1_old = """    let nextX = player.x;
    let nextY = player.y;

    if (player.dir === 'UP') nextY--;
    else if (player.dir === 'DOWN') nextY++;
    else if (player.dir === 'LEFT') nextX--;
    else if (player.dir === 'RIGHT') nextX++;

    if (player.dir !== 'NONE') {
        if (!checkCollision(nextX, nextY)) {
            player.x = nextX;
            player.y = nextY;
            if (frameTick % 2 === 0) playRetroSound('move');
        } else {
            player.dir = 'NONE'; // Block and rest at wall boundaries
        }
    }"""
tick_p1_new = """    let nextX = player.x;
    let nextY = player.y;

    if (player.dir === 'UP') nextY--;
    else if (player.dir === 'DOWN') nextY++;
    else if (player.dir === 'LEFT') nextX--;
    else if (player.dir === 'RIGHT') nextX++;

    if (player.dir !== 'NONE') {
        if (!checkCollision(nextX, nextY)) {
            player.x = nextX;
            player.y = nextY;
            if (frameTick % 2 === 0) playRetroSound('move');
        } else {
            player.dir = 'NONE'; // Block and rest at wall boundaries
        }
    }
    
    let nextX2 = player2.x;
    let nextY2 = player2.y;

    if (player2.dir === 'UP') nextY2--;
    else if (player2.dir === 'DOWN') nextY2++;
    else if (player2.dir === 'LEFT') nextX2--;
    else if (player2.dir === 'RIGHT') nextX2++;

    if (player2.dir !== 'NONE') {
        if (!checkCollision(nextX2, nextY2)) {
            player2.x = nextX2;
            player2.y = nextY2;
            if (frameTick % 2 === 0) playRetroSound('move');
        } else {
            player2.dir = 'NONE';
        }
    }"""
content = content.replace(tick_p1_old, tick_p1_new)

# Item logic
content = content.replace("if (food.active && player.x === food.x && player.y === food.y) {", "if (food.active && ((player.x === food.x && player.y === food.y) || (player2.x === food.x && player2.y === food.y))) {")
content = content.replace("if (axe.active && player.x === axe.x && player.y === axe.y) {", "if (axe.active && ((player.x === axe.x && player.y === axe.y) || (player2.x === axe.x && player2.y === axe.y))) {")
content = content.replace("if (escapeGate.active && player.x === escapeGate.x && player.y === escapeGate.y) {", "if (escapeGate.active && ((player.x === escapeGate.x && player.y === escapeGate.y) || (player2.x === escapeGate.x && player2.y === escapeGate.y))) {")

# Beast proximity AI update
content = content.replace("let nextStep = findBfsNextStep(enemy, player, e);", """
            // AI chooses the closer player
            let dist1 = Math.sqrt(Math.pow(enemy.x - player.x, 2) + Math.pow(enemy.y - player.y, 2));
            let dist2 = Math.sqrt(Math.pow(enemy.x - player2.x, 2) + Math.pow(enemy.y - player2.y, 2));
            let targetPlayer = (dist1 < dist2) ? player : player2;
            let nextStep = findBfsNextStep(enemy, targetPlayer, e);""")

content = content.replace("if (enemy.x === player.x && enemy.y === player.y) {", "if ((enemy.x === player.x && enemy.y === player.y) || (enemy.x === player2.x && enemy.y === player2.y)) {")

# Distances for shaking
content = content.replace("""        let dx = enemy.x - player.x;
        let dy = enemy.y - player.y;
        let dist = Math.sqrt(dx * dx + dy * dy);
        if (dist <= 5.0) {""", """        let dx1 = enemy.x - player.x;
        let dy1 = enemy.y - player.y;
        let dist1_shake = Math.sqrt(dx1 * dx1 + dy1 * dy1);
        let dx2 = enemy.x - player2.x;
        let dy2 = enemy.y - player2.y;
        let dist2_shake = Math.sqrt(dx2 * dx2 + dy2 * dy2);
        if (dist1_shake <= 5.0 || dist2_shake <= 5.0) {""")

# Final player caught check
content = content.replace("if (enemy.active && enemy.x === player.x && enemy.y === player.y) {", "if (enemy.active && ((enemy.x === player.x && enemy.y === player.y) || (enemy.x === player2.x && enemy.y === player2.y))) {")

# Draw player 2
draw_p1 = "drawCaveman(player.x * cellWidth + cellWidth / 2, player.y * cellHeight + cellHeight / 2, player.dir, frameTick);"
draw_p2 = draw_p1 + "\n    drawCaveman(player2.x * cellWidth + cellWidth / 2, player2.y * cellHeight + cellHeight / 2, player2.dir, frameTick);"
content = content.replace(draw_p1, draw_p2)

# Handle weapon visual on both (Optional, but let's just draw weapon on whoever has it? 
# Actually game just tracks "playerHasAxe", meaning ANY player has it. 
# We'll draw the axe over both or let player 1 have it. 
# Let's see how axe is drawn:
# if (playerHasAxe) {
#     ctx.font = '20px Arial';
#     ctx.fillText('🪓', player.x * cellWidth + cellWidth / 2 + 5, player.y * cellHeight + cellHeight / 2 - 5);
# }
# Wait, let's look for how it's drawn and change it.
content = content.replace("ctx.fillText('🪓', player.x * cellWidth + cellWidth / 2 + 5, player.y * cellHeight + cellHeight / 2 - 5);", "ctx.fillText('🪓', player.x * cellWidth + cellWidth / 2 + 5, player.y * cellHeight + cellHeight / 2 - 5);\n        ctx.fillText('🪓', player2.x * cellWidth + cellWidth / 2 + 5, player2.y * cellHeight + cellHeight / 2 - 5);")

with open(path, "w") as f:
    f.write(content)

print("Web patched")
