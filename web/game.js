// Game Engine Constants
const GRID_WIDTH = 60;
const GRID_HEIGHT = 20;
const TICK_DURATION_MS = 150;
const TARGET_SCORE = 100; // 10 stars * 10 points
const MAX_ENEMIES = 20;

// Canvas Configuration
const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
const cellWidth = canvas.width / GRID_WIDTH;
const cellHeight = canvas.height / GRID_HEIGHT;

// Audio System (Self-Contained Web Audio API Synthesizer)
let audioCtx = null;
let soundEnabled = true;

function initAudio() {
    if (!audioCtx) {
        audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    }
}

function playRetroSound(type) {
    if (!soundEnabled) return;
    initAudio();
    if (audioCtx.state === 'suspended') {
        audioCtx.resume();
    }

    const osc = audioCtx.createOscillator();
    const gainNode = audioCtx.createGain();
    osc.connect(gainNode);
    gainNode.connect(audioCtx.destination);

    const now = audioCtx.currentTime;

    if (type === 'star') {
        // Star Collect: Short happy high-pitched chime arpeggio
        osc.type = 'sine';
        osc.frequency.setValueAtTime(587.33, now); // D5
        osc.frequency.setValueAtTime(880.00, now + 0.08); // A5
        gainNode.gain.setValueAtTime(0.15, now);
        gainNode.gain.exponentialRampToValueAtTime(0.01, now + 0.2);
        osc.start(now);
        osc.stop(now + 0.2);
    } else if (type === 'move') {
        // Stepping Tick: Low quick pulse
        osc.type = 'triangle';
        osc.frequency.setValueAtTime(110.00, now); // A2
        gainNode.gain.setValueAtTime(0.04, now);
        gainNode.gain.exponentialRampToValueAtTime(0.01, now + 0.05);
        osc.start(now);
        osc.stop(now + 0.05);
    } else if (type === 'unlock') {
        // Portal Unlock: Rising pitch sweep
        osc.type = 'sawtooth';
        osc.frequency.setValueAtTime(220, now);
        osc.frequency.exponentialRampToValueAtTime(880, now + 0.45);
        gainNode.gain.setValueAtTime(0.1, now);
        gainNode.gain.exponentialRampToValueAtTime(0.01, now + 0.5);
        osc.start(now);
        osc.stop(now + 0.5);
    } else if (type === 'levelup') {
        // Wave Level Up: Victory arpeggio chime
        osc.type = 'square';
        osc.frequency.setValueAtTime(523.25, now); // C5
        osc.frequency.setValueAtTime(659.25, now + 0.1); // E5
        osc.frequency.setValueAtTime(783.99, now + 0.2); // G5
        osc.frequency.setValueAtTime(1046.50, now + 0.3); // C6
        gainNode.gain.setValueAtTime(0.12, now);
        gainNode.gain.exponentialRampToValueAtTime(0.01, now + 0.55);
        osc.start(now);
        osc.stop(now + 0.55);
    } else if (type === 'death') {
        // Caught by Beast: Downward grinding crunch sweep
        osc.type = 'sawtooth';
        osc.frequency.setValueAtTime(261.63, now); // C4
        osc.frequency.linearRampToValueAtTime(40, now + 0.6);
        gainNode.gain.setValueAtTime(0.2, now);
        gainNode.gain.linearRampToValueAtTime(0.01, now + 0.65);
        osc.start(now);
        osc.stop(now + 0.65);
    }
}

// Game State Definitions
let gameActive = false;
let playerWon = false;
let currentLevel = 1;
let currentScore = 0;
let starsCollected = 0;
let highScore = 250;
let highScoreName = "NCIT Student";
let frameTick = 0;
let gameLoopTimer = null;

// Game Entities
let player = { x: 2, y: 2, dir: 'NONE' };
let enemies = [];
let enemyCount = 1;
let food = { x: 0, y: 0, active: false };
let escapeGate = { x: GRID_WIDTH - 2, y: Math.floor(GRID_HEIGHT / 2), active: false };
let obstacles = [];

// Procedural Barrier Walls Generator (Perfect C99 logic match)
function generateObstacles() {
    obstacles = [];
    // Barrier Wall 1: Column 15
    for (let y = 2; y <= 7; y++) obstacles.push({ x: 15, y });
    for (let y = 12; y <= 17; y++) obstacles.push({ x: 15, y });
    
    // Barrier Wall 2: Column 30
    for (let y = 5; y <= 14; y++) obstacles.push({ x: 30, y });
    
    // Barrier Wall 3: Column 45
    for (let y = 2; y <= 7; y++) obstacles.push({ x: 45, y });
    for (let y = 12; y <= 17; y++) obstacles.push({ x: 45, y });
}

// Collision Check Function
function checkCollision(x, y) {
    // 1. Boundary Wall Bounds
    if (x <= 0 || x >= GRID_WIDTH - 1 || y <= 0 || y >= GRID_HEIGHT - 1) {
        return true;
    }
    // 2. Procedural Rock Wall Obstacles
    for (let obs of obstacles) {
        if (obs.x === x && obs.y === y) return true;
    }
    return false;
}

// Spawning Star food Target (Clean collision avoidance)
function spawnFood() {
    let valid = false;
    let rx, ry;
    
    while (!valid) {
        rx = 1 + Math.floor(Math.random() * (GRID_WIDTH - 2));
        ry = 1 + Math.floor(Math.random() * (GRID_HEIGHT - 2));
        
        if (checkCollision(rx, ry)) continue;
        if (rx === player.x && ry === player.y) continue;
        if (rx === escapeGate.x && ry === escapeGate.y) continue;
        
        let spawnOnEnemy = false;
        for (let enemy of enemies) {
            if (enemy.active && enemy.x === rx && enemy.y === ry) {
                spawnOnEnemy = true;
                break;
            }
        }
        if (spawnOnEnemy) continue;
        
        valid = true;
    }
    
    food.x = rx;
    food.y = ry;
    food.active = true;
}

// Initialize Local Score cache from localStorage
function loadRecord() {
    const savedScore = localStorage.getItem('caveman_high_score');
    const savedName = localStorage.getItem('caveman_high_score_name');
    if (savedScore !== null) {
        highScore = parseInt(savedScore, 10);
        highScoreName = savedName || "NCIT Student";
    }
    updateHud();
}

function saveRecord(name, score) {
    localStorage.setItem('caveman_high_score', score);
    localStorage.setItem('caveman_high_score_name', name);
    highScore = score;
    highScoreName = name;
    updateHud();
}

// UI HUD Dashboard Syncs
function updateHud() {
    document.getElementById('hudLevel').innerText = `WAVE ${currentLevel}`;
    document.getElementById('hudScore').innerText = currentScore;
    
    const needed = currentLevel * 10;
    const progress = Math.min(10, Math.floor((currentScore - (currentLevel - 1) * 100) / 10));
    document.getElementById('hudStars').innerText = `${progress} / 10`;
    
    document.getElementById('hudRecordHolder').innerText = highScoreName;
    document.getElementById('hudHighScore').innerText = highScore;

    const banner = document.getElementById('alertBanner');
    if (escapeGate.active) {
        banner.classList.add('active');
    } else {
        banner.classList.remove('active');
    }
}

// Core Game Setup
function startGame() {
    gameActive = true;
    playerWon = false;
    currentScore = 0;
    currentLevel = 1;
    enemyCount = 1;
    frameTick = 0;

    player.x = 2;
    player.y = 2;
    player.dir = 'NONE';

    enemies = [{ x: GRID_WIDTH - 3, y: GRID_HEIGHT - 2, active: true }];
    escapeGate.active = false;

    generateObstacles();
    spawnFood();
    updateHud();

    document.getElementById('screenOverlay').classList.remove('active');

    // Run Asynchronous Clock Tick loops
    if (gameLoopTimer) clearInterval(gameLoopTimer);
    gameLoopTimer = setInterval(gameLoopTick, TICK_DURATION_MS);
}

// Core Loop Steps
function gameLoopTick() {
    if (!gameActive) return;

    frameTick++;

    // 1. Move Player coordinates
    let nextX = player.x;
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

    // 2. Evaluate Star Food items collection
    if (food.active && player.x === food.x && player.y === food.y) {
        currentScore += 10;
        playRetroSound('star');
        
        // Portal Gate open target checks
        if (currentScore >= currentLevel * TARGET_SCORE) {
            escapeGate.active = true;
            playRetroSound('unlock');
        }
        spawnFood();
        updateHud();
    }

    // 3. Evaluate Level Escapes transitions
    if (escapeGate.active && player.x === escapeGate.x && player.y === escapeGate.y) {
        currentLevel++;
        playRetroSound('levelup');
        
        if (enemyCount < MAX_ENEMIES) {
            enemyCount++;
        }

        // Reset positions
        player.x = 2;
        player.y = 2;
        player.dir = 'NONE';
        escapeGate.active = false;

        // repopulate enemies strategically in corners
        enemies = [];
        for (let i = 0; i < enemyCount; i++) {
            let ex = GRID_WIDTH - 3;
            let ey = GRID_HEIGHT - 2;
            if (i === 1) { ex = GRID_WIDTH - 3; ey = 2; }
            else if (i === 2) { ex = 2; ey = GRID_HEIGHT - 2; }
            else if (i === 3) { ex = Math.floor(GRID_WIDTH / 2); ey = GRID_HEIGHT - 2; }
            else if (i === 4) { ex = Math.floor(GRID_WIDTH / 2); ey = 2; }
            else if (i > 4) {
                // Random spawn avoiding obstacles and player coordinates
                let v = false;
                while(!v) {
                    ex = 1 + Math.floor(Math.random() * (GRID_WIDTH - 2));
                    ey = 1 + Math.floor(Math.random() * (GRID_HEIGHT - 2));
                    if (!checkCollision(ex, ey) && (ex !== player.x || ey !== player.y)) v = true;
                }
            }
            enemies.push({ x: ex, y: ey, active: true });
        }

        spawnFood();
        updateHud();
    }

    // 4. Update Automated Enemy AI hunting paths (Moves every 2 clock ticks)
    if (frameTick % 2 === 0) {
        const moves = [
            { dx: 0, dy: -1 }, // UP
            { dx: 0, dy: 1 },  // DOWN
            { dx: -1, dy: 0 }, // LEFT
            { dx: 1, dy: 0 }   // RIGHT
        ];

        for (let e = 0; e < enemies.length; e++) {
            let enemy = enemies[e];
            if (!enemy.active) continue;

            let minDistance = 1e9;
            let bestX = enemy.x;
            let bestY = enemy.y;

            for (let mv of moves) {
                let cx = enemy.x + mv.dx;
                let cy = enemy.y + mv.dy;

                if (checkCollision(cx, cy)) continue;

                // Mutual Chaser Avoidance logic: Skip tiles occupied by another active beast
                let occupiedByBeast = false;
                for (let oe = 0; oe < enemies.length; oe++) {
                    if (oe !== e && enemies[oe].active && enemies[oe].x === cx && enemies[oe].y === cy) {
                        occupiedByBeast = true;
                        break;
                    }
                }
                if (occupiedByBeast) continue;

                // Stop beast from sitting on the exit gate
                if (escapeGate.active && cx === escapeGate.x && cy === escapeGate.y) continue;

                // Euclidean distance optimization
                let dx = cx - player.x;
                let dy = cy - player.y;
                let dist = Math.sqrt(dx * dx + dy * dy);

                if (dist < minDistance) {
                    minDistance = dist;
                    bestX = cx;
                    bestY = cy;
                }
            }

            enemy.x = bestX;
            enemy.y = bestY;

            // Check if player caught during beast update
            if (enemy.x === player.x && enemy.y === player.y) {
                gameOver(false);
                return;
            }
        }
    }

    // 5. Evaluate Caught checks
    for (let enemy of enemies) {
        if (enemy.active && enemy.x === player.x && enemy.y === player.y) {
            gameOver(false);
            return;
        }
    }

    // 6. Draw CRT Screen frame
    renderScreen();
}

// CRT Terminal screen drawings
function renderScreen() {
    // Clear canvas
    ctx.fillStyle = '#03050c';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Draw Static Border Walls (Deep Blue CRT borders)
    ctx.strokeStyle = '#1e3a8a';
    ctx.lineWidth = 2;
    ctx.strokeRect(cellWidth / 2, cellHeight / 2, canvas.width - cellWidth, canvas.height - cellHeight);

    // Draw Obstacles (Gray rock blocks)
    ctx.fillStyle = '#475569';
    for (let obs of obstacles) {
        ctx.fillRect(obs.x * cellWidth + 1, obs.y * cellHeight + 1, cellWidth - 2, cellHeight - 2);
    }

    // Draw Food Star (Twinkling gold stars)
    if (food.active) {
        ctx.fillStyle = '#ffd200';
        ctx.shadowColor = '#ffd200';
        ctx.shadowBlur = 8;
        ctx.font = `bold ${cellHeight}px monospace`;
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        
        // Pulsing twinkle shapes
        const starGlyph = (frameTick % 2 === 0) ? '★' : '☆';
        ctx.fillText(starGlyph, food.x * cellWidth + cellWidth / 2, food.y * cellHeight + cellHeight / 2);
        ctx.shadowBlur = 0; // Reset glows
    }

    // Draw Escape Gate Portal (Liquid shifting waves)
    if (escapeGate.active) {
        ctx.fillStyle = '#ff00dc';
        ctx.shadowColor = '#ff00dc';
        ctx.shadowBlur = 10;
        ctx.font = `bold ${cellHeight}px monospace`;
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';

        const densities = ['░', '▒', '▓', '▒'];
        const portalGlyph = densities[frameTick % 4];
        ctx.fillText(portalGlyph, escapeGate.x * cellWidth + cellWidth / 2, escapeGate.y * cellHeight + cellHeight / 2);
        ctx.shadowBlur = 0;
    }

    // Draw Caveman Player (Vibrant Green)
    ctx.fillStyle = '#32ff32';
    ctx.shadowColor = '#32ff32';
    ctx.shadowBlur = 8;
    ctx.font = `bold ${cellHeight}px monospace`;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';

    let playerGlyph = '☻'; // Default standing
    const pState = frameTick % 4;
    
    if (pState === 0) playerGlyph = 'C';
    else if (pState === 2) playerGlyph = '☻';
    else {
        if (player.dir === 'UP') playerGlyph = '▲';
        else if (player.dir === 'DOWN') playerGlyph = '▼';
        else if (player.dir === 'LEFT') playerGlyph = '◄';
        else if (player.dir === 'RIGHT') playerGlyph = '►';
    }
    ctx.fillText(playerGlyph, player.x * cellWidth + cellWidth / 2, player.y * cellHeight + cellHeight / 2);
    ctx.shadowBlur = 0;

    // Draw Chaser Beasts (Dynamic Styles, Colors, and Proximity Rage Flashers)
    const beastColors = ['#ff3232', '#ffd200', '#ff00dc', '#00ffeb', '#38bdf8', '#fb923c', '#ca8a04', '#ffffff'];
    const beastSignatures = ['B', 'M', 'D', 'H', 'Z', 'X', 'W', 'V'];
    const beastClaws = ['Ψ', 'Ω', '☠', '♦', '♣', '▲', '☼', '╬'];

    for (let i = 0; i < enemies.length; i++) {
        let enemy = enemies[i];
        if (!enemy.active) continue;

        // Calculate proximity distance to player
        let dx = enemy.x - player.x;
        let dy = enemy.y - player.y;
        let dist = Math.sqrt(dx * dx + dy * dy);

        let beastColor = beastColors[i % 8];
        let beastGlyph = beastSignatures[i % 8];

        if (dist <= 5.0) {
            // PROXIMITY RAGE WARNING Skull (Flash intense red at high speed!)
            ctx.fillStyle = '#ff1e1e';
            ctx.shadowColor = '#ff1e1e';
            ctx.shadowBlur = 12;
            beastGlyph = (frameTick % 2 === 0) ? '☠' : beastSignatures[i % 8];
        } else {
            // Normal stalking styles
            ctx.fillStyle = beastColor;
            ctx.shadowColor = beastColor;
            ctx.shadowBlur = 5;
            beastGlyph = (Math.floor(frameTick / 2) % 2 === 0) ? beastSignatures[i % 8] : beastClaws[i % 8];
        }

        ctx.fillText(beastGlyph, enemy.x * cellWidth + cellWidth / 2, enemy.y * cellHeight + cellHeight / 2);
        ctx.shadowBlur = 0;
    }
}

// Game Over Teardown
function gameOver(won) {
    gameActive = false;
    clearInterval(gameLoopTimer);
    playRetroSound(won ? 'levelup' : 'death');

    const overlay = document.getElementById('screenOverlay');
    const title = document.getElementById('overlayTitle');
    const subtitle = document.getElementById('overlaySubtitle');
    const nameInputGroup = document.getElementById('usernameInputContainer');
    const startBtn = document.getElementById('startBtn');

    overlay.classList.add('active');

    if (currentScore > highScore) {
        // High score record beaten! Show name field
        title.innerText = '🏆 NEW HIGH RECORD!';
        title.style.color = '#ffd200';
        title.style.textShadow = 'var(--glow-yellow)';
        subtitle.innerText = `You set a new NCIT record score of ${currentScore}!`;
        nameInputGroup.classList.remove('hidden');
        startBtn.innerText = 'SAVE RECORD & PLAY';
    } else {
        // Normal game over
        title.innerText = 'GAME OVER';
        title.style.color = '#ff3232';
        title.style.textShadow = 'var(--glow-red)';
        subtitle.innerText = `You survived up to Wave ${currentLevel} with a score of ${currentScore}.`;
        nameInputGroup.classList.add('hidden');
        startBtn.innerText = 'RETRY ADVENTURE';
    }
}

// Action Event Hooks (Keys, Touch and Clicks)
function handleDirectionInput(dir) {
    if (!gameActive) return;
    player.dir = dir;
}

// Bind keyboard hooks
window.addEventListener('keydown', (e) => {
    if (!gameActive) return;
    
    // Support WASD + Arrows
    switch (e.key) {
        case 'w': case 'W': case 'ArrowUp':
            handleDirectionInput('UP');
            e.preventDefault();
            break;
        case 's': case 'S': case 'ArrowDown':
            handleDirectionInput('DOWN');
            e.preventDefault();
            break;
        case 'a': case 'A': case 'ArrowLeft':
            handleDirectionInput('LEFT');
            e.preventDefault();
            break;
        case 'd': case 'D': case 'ArrowRight':
            handleDirectionInput('RIGHT');
            e.preventDefault();
            break;
        case 'Escape':
            gameOver(false);
            break;
    }
});

// Bind On-Screen Touch D-pad buttons (for mobile compatibility)
document.getElementById('btnUp').addEventListener('touchstart', (e) => { handleDirectionInput('UP'); e.preventDefault(); });
document.getElementById('btnDown').addEventListener('touchstart', (e) => { handleDirectionInput('DOWN'); e.preventDefault(); });
document.getElementById('btnLeft').addEventListener('touchstart', (e) => { handleDirectionInput('LEFT'); e.preventDefault(); });
document.getElementById('btnRight').addEventListener('touchstart', (e) => { handleDirectionInput('RIGHT'); e.preventDefault(); });

document.getElementById('btnUp').addEventListener('mousedown', () => handleDirectionInput('UP'));
document.getElementById('btnDown').addEventListener('mousedown', () => handleDirectionInput('DOWN'));
document.getElementById('btnLeft').addEventListener('mousedown', () => handleDirectionInput('LEFT'));
document.getElementById('btnRight').addEventListener('mousedown', () => handleDirectionInput('RIGHT'));

// Overlay start buttons
document.getElementById('startBtn').addEventListener('click', () => {
    const nameInputGroup = document.getElementById('usernameInputContainer');
    
    if (!nameInputGroup.classList.contains('hidden')) {
        const inputName = document.getElementById('usernameInput').value.trim() || "NCIT Player";
        saveRecord(inputName, currentScore);
    }
    
    startGame();
});

// Sound toggler button
document.getElementById('soundToggleBtn').addEventListener('click', () => {
    soundEnabled = !soundEnabled;
    const btn = document.getElementById('soundToggleBtn');
    if (soundEnabled) {
        btn.innerText = '🔊 SOUND: ON';
        btn.style.color = '#94a3b8';
    } else {
        btn.innerText = '🔇 SOUND: OFF';
        btn.style.color = '#64748b';
    }
});

// First startup initial load
window.addEventListener('DOMContentLoaded', () => {
    loadRecord();
    generateObstacles();
    renderScreen(); // Draw background grid layout initial frames
});
