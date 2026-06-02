// Game Engine Constants
const GRID_WIDTH = 75;
const GRID_HEIGHT = 30;
const TICK_DURATION_MS = 120; // Faster retro arcarde clock speed (120ms)
const TARGET_SCORE = 50; // 5 stars * 10 points
const MAX_ENEMIES = 20;

// Prehistoric Cavern Environment Themes (Green Moss -> Autumn Forest -> Frozen Ice -> Volcano Magma)
const CAVERN_THEMES = [
    {
        name: 'MOSS GREEN',
        floorBg: '#05140b',
        border: '#166534',
        crack: 'rgba(22, 101, 52, 0.18)',
        rockStart: '#15803d',
        rockEnd: '#064e3b'
    },
    {
        name: 'AUTUMN FOREST',
        floorBg: '#140f09',
        border: '#854d0e',
        crack: 'rgba(133, 77, 14, 0.18)',
        rockStart: '#a16207',
        rockEnd: '#451a03'
    },
    {
        name: 'FROZEN ICE',
        floorBg: '#051625',
        border: '#075985',
        crack: 'rgba(7, 89, 133, 0.18)',
        rockStart: '#0284c7',
        rockEnd: '#0c4a6e'
    },
    {
        name: 'VOLCANO MAGMA',
        floorBg: '#120505',
        border: '#991b1b',
        crack: 'rgba(153, 27, 27, 0.22)',
        rockStart: '#dc2626',
        rockEnd: '#450a0a'
    }
];

// Game State Variables
let playerLives = 3;

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
let playerHasAxe = false;
let axe = { x: 0, y: 0, active: false };

// Procedural Barrier Walls Generator (Perfect C99 logic match)
function generateObstacles() {
    obstacles = [];
    let col1 = Math.floor(GRID_WIDTH * 0.25);
    let col2 = Math.floor(GRID_WIDTH * 0.50);
    let col3 = Math.floor(GRID_WIDTH * 0.75);

    // Barrier Wall 1: Left Sector
    for (let y = 2; y <= Math.floor(GRID_HEIGHT * 0.35); y++) obstacles.push({ x: col1, y });
    for (let y = Math.floor(GRID_HEIGHT * 0.60); y <= Math.floor(GRID_HEIGHT * 0.85); y++) obstacles.push({ x: col1, y });
    
    // Barrier Wall 2: Middle Sector
    for (let y = Math.floor(GRID_HEIGHT * 0.25); y <= Math.floor(GRID_HEIGHT * 0.75); y++) obstacles.push({ x: col2, y });
    
    // Barrier Wall 3: Right Sector
    for (let y = 2; y <= Math.floor(GRID_HEIGHT * 0.35); y++) obstacles.push({ x: col3, y });
    for (let y = Math.floor(GRID_HEIGHT * 0.60); y <= Math.floor(GRID_HEIGHT * 0.85); y++) obstacles.push({ x: col3, y });
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

// Procedural Weapon Spawner (Spawn an axe in even rounds >= 4)
function spawnAxe() {
    let valid = false;
    let rx, ry;
    
    while (!valid) {
        rx = 1 + Math.floor(Math.random() * (GRID_WIDTH - 2));
        ry = 1 + Math.floor(Math.random() * (GRID_HEIGHT - 2));
        
        if (checkCollision(rx, ry)) continue;
        if (rx === player.x && ry === player.y) continue;
        if (rx === food.x && ry === food.y) continue;
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
    
    axe.x = rx;
    axe.y = ry;
    axe.active = true;
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
    let theme = CAVERN_THEMES[(currentLevel - 1) % 4];
    document.getElementById('hudLevel').innerText = `WAVE ${currentLevel} (${theme.name})`;
    document.getElementById('hudScore').innerText = currentScore;
    
    const progress = Math.min(5, Math.floor((currentScore - (currentLevel - 1) * 50) / 10));
    document.getElementById('hudStars').innerText = `${progress} / 5`;
    
    let hearts = '';
    for (let i = 0; i < playerLives; i++) {
        hearts += '❤️ ';
    }
    if (hearts === '') hearts = '💀 DEAD';
    document.getElementById('hudLives').innerText = hearts;
    
    document.getElementById('hudWeapon').innerText = playerHasAxe ? '🪓 STONE AXE' : 'NONE';
    
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
    playerLives = 3;
    currentScore = 0;
    currentLevel = 1;
    enemyCount = 1;
    frameTick = 0;

    player.x = 2;
    player.y = 2;
    player.dir = 'NONE';

    enemies = [{ x: GRID_WIDTH - 3, y: GRID_HEIGHT - 2, active: true, type: 'MAMMOTH' }];
    escapeGate.active = false;
    playerHasAxe = false;
    axe.active = false;

    // Reset monitor shake at start
    document.querySelector('.crt-bezel').classList.remove('crt-shake');

    generateObstacles();
    spawnFood();
    updateHud();

    document.getElementById('screenOverlay').classList.remove('active');

    // Run Asynchronous Clock Tick loops
    if (gameLoopTimer) clearInterval(gameLoopTimer);
    gameLoopTimer = setInterval(gameLoopTick, TICK_DURATION_MS);
}

// Breadth-First Search (BFS) for true shortest path planning (mirrors C99 engine)
function findBfsNextStep(start, target, enemyIdx) {
    let queue = [{ x: start.x, y: start.y }];
    
    // Visited grid maps
    let visited = Array(GRID_WIDTH).fill().map(() => Array(GRID_HEIGHT).fill(false));
    let parent = Array(GRID_WIDTH).fill().map(() => Array(GRID_HEIGHT).fill(null));
    
    visited[start.x][start.y] = true;
    let pathFound = false;
    
    const moves = [
        { dx: 0, dy: -1 }, // UP
        { dx: 0, dy: 1 },  // DOWN
        { dx: -1, dy: 0 }, // LEFT
        { dx: 1, dy: 0 }   // RIGHT
    ];
    
    while (queue.length > 0) {
        let curr = queue.shift();
        
        if (curr.x === target.x && curr.y === target.y) {
            pathFound = true;
            break;
        }
        
        for (let mv of moves) {
            let nx = curr.x + mv.dx;
            let ny = curr.y + mv.dy;
            
            if (checkCollision(nx, ny)) continue;
            
            // Mutual avoidance
            let occupied = false;
            for (let oe = 0; oe < enemies.length; oe++) {
                if (oe !== enemyIdx && enemies[oe].active && enemies[oe].x === nx && enemies[oe].y === ny) {
                    occupied = true;
                    break;
                }
            }
            if (occupied) continue;
            
            if (escapeGate.active && nx === escapeGate.x && ny === escapeGate.y) continue;
            
            if (!visited[nx][ny]) {
                visited[nx][ny] = true;
                parent[nx][ny] = curr;
                queue.push({ x: nx, y: ny });
            }
        }
    }
    
    if (pathFound) {
        let curr = { x: target.x, y: target.y };
        let prevStep = { x: target.x, y: target.y };
        
        while (curr.x !== start.x || curr.y !== start.y) {
            prevStep = { x: curr.x, y: curr.y };
            let p = parent[curr.x][curr.y];
            if (!p) break;
            curr = p;
        }
        return prevStep;
    }
    
    return start; // Fallback to stationary
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

    // 2b. Evaluate Axe weapon items collection
    if (axe.active && player.x === axe.x && player.y === axe.y) {
        playerHasAxe = true;
        axe.active = false;
        playRetroSound('star'); // weapon pickup chime
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
        let numDinos = (currentLevel >= 3) ? Math.floor((currentLevel - 3) / 2) + 1 : 0;
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
            
            // Assign type: Boss Dino for the first numDinos, normal Mammoth for rest
            let type = (i < numDinos) ? 'DINO' : 'MAMMOTH';
            enemies.push({ x: ex, y: ey, active: true, type: type });
        }

        spawnFood();
        
        // Spawn stone axe in even rounds >= 4 (Level 4, 6, 8...)
        playerHasAxe = false;
        if (currentLevel % 2 === 0 && currentLevel >= 4) {
            spawnAxe();
        } else {
            axe.active = false;
        }
        
        updateHud();
    }

    // 4. Update Automated Enemy AI hunting paths (Speed: Dino moves every tick, Mammoth moves every 2 ticks)
    let anyBeastClose = false;

    for (let e = 0; e < enemies.length; e++) {
        let enemy = enemies[e];
        if (!enemy.active) continue;

        let shouldMove = false;
        if (enemy.type === 'DINO') {
            shouldMove = (frameTick % 3 !== 0); // Slightly slower: moves 2 out of every 3 ticks (66.7% speed)
        } else {
            shouldMove = (frameTick % 2 === 0); // Every 2 ticks (Normal Speed: 50% speed)
        }

        if (shouldMove) {
            // Use Breadth-First Search (BFS) pathfinder
            let nextStep = findBfsNextStep(enemy, player, e);
            enemy.x = nextStep.x;
            enemy.y = nextStep.y;

            // Check if player caught during beast update
            if (enemy.x === player.x && enemy.y === player.y) {
                playerCaught();
                return;
            }
        }

        // Proximity shake checks (evaluated every tick)
        let dx = enemy.x - player.x;
        let dy = enemy.y - player.y;
        let dist = Math.sqrt(dx * dx + dy * dy);
        if (dist <= 5.0) {
            anyBeastClose = true;
        }
    }

    // Apply screen shake
    const bezel = document.querySelector('.crt-bezel');
    if (anyBeastClose && gameActive) {
        bezel.classList.add('crt-shake');
    } else {
        bezel.classList.remove('crt-shake');
    }

    // 5. Evaluate Caught checks
    for (let enemy of enemies) {
        if (enemy.active && enemy.x === player.x && enemy.y === player.y) {
            playerCaught();
            return;
        }
    }

    // 6. Draw CRT Screen frame
    renderScreen();
}

// CRT Terminal screen drawings
// Draw Caveman Human Player dynamically
function drawCaveman(cx, cy, dir, tick) {
    ctx.save();
    
    // Set a glowing shadow for the player
    ctx.shadowColor = '#32ff32';
    ctx.shadowBlur = 6;

    // Body (Brown Animal Skin Tunic)
    ctx.fillStyle = '#78350f'; // Dark Brown
    ctx.fillRect(cx - 5, cy, 10, 7);
    
    // Leopard spots
    ctx.fillStyle = '#ea580c';
    ctx.fillRect(cx - 3, cy + 2, 2, 2);
    ctx.fillRect(cx + 2, cy + 4, 1.5, 1.5);

    // Diagonal Sash Strap
    ctx.strokeStyle = '#451a03';
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    ctx.moveTo(cx - 5, cy);
    ctx.lineTo(cx + 5, cy + 5);
    ctx.stroke();

    // Head (Peach skin tone)
    ctx.fillStyle = '#f1c27d';
    ctx.beginPath();
    ctx.arc(cx, cy - 3.5, 4.5, 0, Math.PI * 2);
    ctx.fill();

    // Caveman Messy Hair (Bright Wild Orange)
    ctx.fillStyle = '#ea580c';
    ctx.beginPath();
    ctx.arc(cx, cy - 5.5, 4, Math.PI, 0);
    ctx.fill();
    
    // Messy tufts
    ctx.fillRect(cx - 5, cy - 6, 2, 3);
    ctx.fillRect(cx + 3, cy - 6, 2, 3);
    ctx.fillRect(cx - 1, cy - 7, 2, 2);

    // Beard
    ctx.fillStyle = '#ea580c';
    ctx.fillRect(cx - 3.5, cy - 1, 7, 2);

    // Eyes
    ctx.fillStyle = '#000000';
    ctx.fillRect(cx - 2, cy - 4.5, 1, 1.5);
    ctx.fillRect(cx + 1, cy - 4.5, 1, 1.5);

    // Limbs (Dynamic walking animations based on step tick)
    ctx.strokeStyle = '#f1c27d';
    ctx.lineWidth = 2.5;
    ctx.beginPath();

    let motion = (tick % 2 === 0) ? 2 : -2;

    // Legs
    ctx.moveTo(cx - 3, cy + 7);
    ctx.lineTo(cx - 3 - motion, cy + 11);
    
    ctx.moveTo(cx + 3, cy + 7);
    ctx.lineTo(cx + 3 + motion, cy + 11);

    // Arms swinging in direction of steering
    if (dir === 'LEFT' || dir === 'RIGHT') {
        ctx.moveTo(cx - 5, cy + 2);
        ctx.lineTo(cx - 9, cy + 2 + motion);
        ctx.moveTo(cx + 5, cy + 2);
        ctx.lineTo(cx + 9, cy + 2 - motion);
    } else {
        ctx.moveTo(cx - 5, cy + 2);
        ctx.lineTo(cx - 8, cy + 5 - motion);
        ctx.moveTo(cx + 5, cy + 2);
        ctx.lineTo(cx + 8, cy + 5 + motion);
    }
    ctx.stroke();

    ctx.restore();
}

// Draw terrifying early-age Mammoth dynamically
function drawMammoth(cx, cy, color, isRaged, tick) {
    ctx.save();
    
    // Fuzzy brown or angry red
    let bodyColor = isRaged ? '#ff1e1e' : '#7c2d12';
    ctx.fillStyle = bodyColor;
    
    ctx.shadowColor = bodyColor;
    ctx.shadowBlur = isRaged ? 12 : 5;

    // Strong stocky legs (walking animation)
    let legMotion = (tick % 2 === 0) ? 2 : -2;
    ctx.strokeStyle = bodyColor;
    ctx.lineWidth = 3.5;
    ctx.beginPath();
    // Front leg
    ctx.moveTo(cx - 3, cy + 3);
    ctx.lineTo(cx - 5, cy + 8 + legMotion);
    // Back leg
    ctx.moveTo(cx + 3, cy + 3);
    ctx.lineTo(cx + 5, cy + 8 - legMotion);
    ctx.stroke();

    // Massive Mammoth Torso (Egg-like with shoulder hump)
    ctx.beginPath();
    ctx.ellipse(cx, cy + 1, 7.5, 6, 0, 0, Math.PI * 2);
    ctx.fill();

    // Head (merges with hump)
    ctx.beginPath();
    ctx.arc(cx - 4, cy - 2, 4.5, 0, Math.PI * 2);
    ctx.fill();

    // Ear flaps (darker brown)
    ctx.fillStyle = '#451a03';
    ctx.beginPath();
    ctx.arc(cx - 1, cy - 3, 1.8, 0, Math.PI * 2);
    ctx.fill();

    // Massive Curved Tusks (White sweeping forward and up!)
    ctx.strokeStyle = '#f8fafc';
    ctx.lineWidth = 1.8;
    ctx.beginPath();
    ctx.moveTo(cx - 7, cy - 1);
    ctx.bezierCurveTo(cx - 13, cy - 2, cx - 12, cy - 8, cx - 7, cy - 9);
    ctx.stroke();

    // Glowing Eyes
    ctx.fillStyle = isRaged ? '#ffffff' : '#facc15';
    ctx.fillRect(cx - 7, cy - 4.5, 1.2, 1.2);

    // Prehensile Trunk (waving up and down)
    ctx.strokeStyle = bodyColor;
    ctx.lineWidth = 2.5;
    ctx.beginPath();
    ctx.moveTo(cx - 6, cy - 2);
    ctx.quadraticCurveTo(cx - 11, cy + 2 + legMotion, cx - 10 - legMotion, cy + 5);
    ctx.stroke();

    ctx.restore();
}

// Draw terrifying early-age Dino (larger, towering, super fast scaly predator!)
function drawDino(cx, cy, color, isRaged, tick) {
    ctx.save();
    
    // Dino scales are deep green/gold highlights or angry flashing red
    let baseColor = isRaged ? '#ff1e1e' : '#16a34a'; 
    let scaleColor = isRaged ? '#ffffff' : '#eab308'; // Gold scales
    
    ctx.fillStyle = baseColor;
    ctx.shadowColor = baseColor;
    ctx.shadowBlur = isRaged ? 16 : 8;

    let motion = (tick % 2 === 0) ? 2 : -2;

    // Strong Clawed bipedal legs (waving with claw toes!)
    ctx.strokeStyle = baseColor;
    ctx.lineWidth = 4;
    ctx.beginPath();
    ctx.moveTo(cx - 2, cy + 4);
    ctx.lineTo(cx - 5, cy + 10 + motion);
    ctx.lineTo(cx - 8, cy + 11 + motion); // Claw foot
    
    ctx.moveTo(cx + 2, cy + 4);
    ctx.lineTo(cx + 4, cy + 10 - motion);
    ctx.lineTo(cx + 1, cy + 11 - motion); // Claw foot
    ctx.stroke();

    // Large Dino Torso with gold plates on the back
    ctx.beginPath();
    ctx.ellipse(cx, cy + 1, 10, 7.5, -Math.PI / 12, 0, Math.PI * 2);
    ctx.fill();

    // Golden Scale Plates along the spine
    ctx.fillStyle = scaleColor;
    ctx.beginPath();
    ctx.arc(cx + 3, cy - 5, 2, 0, Math.PI * 2);
    ctx.arc(cx - 2, cy - 6, 2.2, 0, Math.PI * 2);
    ctx.arc(cx - 6, cy - 4, 1.8, 0, Math.PI * 2);
    ctx.fill();

    // Long powerful scaly neck
    ctx.lineWidth = 5.5;
    ctx.strokeStyle = baseColor;
    ctx.beginPath();
    ctx.moveTo(cx - 4, cy - 2);
    ctx.lineTo(cx - 8, cy - 9);
    ctx.stroke();

    // Snapping Snout / Head (Snaps open/shut based on motion tick!)
    ctx.fillStyle = baseColor;
    let snapAngle = (tick % 2 === 0) ? 0.2 : 0;
    
    // Draw Upper Jaw
    ctx.save();
    ctx.translate(cx - 8, cy - 9);
    ctx.rotate(-snapAngle);
    ctx.beginPath();
    ctx.ellipse(-2, -2, 5, 3.2, 0, 0, Math.PI * 2);
    ctx.fill();
    // Razor sharp teeth
    ctx.fillStyle = '#ffffff';
    ctx.fillRect(-6, 0, 1.2, 1.2);
    ctx.fillRect(-4, 0, 1.2, 1.2);
    ctx.restore();

    // Draw Lower Jaw
    ctx.save();
    ctx.translate(cx - 8, cy - 9);
    ctx.rotate(snapAngle);
    ctx.fillStyle = baseColor;
    ctx.beginPath();
    ctx.ellipse(-1.5, 1, 4.5, 2.5, 0, 0, Math.PI * 2);
    ctx.fill();
    // Lower Teeth
    ctx.fillStyle = '#ffffff';
    ctx.fillRect(-5, -1, 1.2, 1.2);
    ctx.fillRect(-3, -1, 1.2, 1.2);
    ctx.restore();

    // Glowing predator eye (flashes between yellow and red)
    ctx.fillStyle = isRaged ? '#ffffff' : '#f43f5e';
    ctx.beginPath();
    ctx.arc(cx - 10, cy - 11.5, 1.5, 0, Math.PI * 2);
    ctx.fill();

    // Long, thick, whipping tail (swings side to side)
    ctx.strokeStyle = baseColor;
    ctx.lineWidth = 4;
    ctx.beginPath();
    ctx.moveTo(cx + 8, cy + 1);
    ctx.quadraticCurveTo(cx + 15, cy + 2 + motion, cx + 19 + motion, cy - 4);
    ctx.stroke();
    
    // Spikes on the tail tip (Gold)
ctx.fillStyle = scaleColor;
    ctx.beginPath();
    ctx.arc(cx + 19 + motion, cy - 4, 1.8, 0, Math.PI * 2);
    ctx.fill();

    ctx.restore();
}

// Draw juicy roasted Caveman Meat (replaces simple Star symbol with 2D bone drumstick)
function drawMeat(cx, cy, tick) {
    ctx.save();
    
    // Glowing retro outline
    ctx.shadowColor = '#f97316'; // Neon orange glow
    ctx.shadowBlur = (tick % 2 === 0) ? 8 : 4;
    
    // Draw white bone shaft (diagonal angle)
    ctx.strokeStyle = '#f8fafc'; // Bone white
    ctx.lineWidth = 3.5;
    ctx.lineCap = 'round';
    ctx.beginPath();
    ctx.moveTo(cx - 5, cy + 5);
    ctx.lineTo(cx + 6, cy - 6);
    ctx.stroke();
    
    // Bone joints / double knobs on both ends
    ctx.fillStyle = '#f8fafc';
    ctx.beginPath();
    ctx.arc(cx - 5, cy + 5, 2.2, 0, Math.PI * 2);
    ctx.arc(cx - 3, cy + 7, 2.2, 0, Math.PI * 2);
    ctx.arc(cx + 6, cy - 6, 2.2, 0, Math.PI * 2);
    ctx.arc(cx + 8, cy - 4, 2.2, 0, Math.PI * 2);
    ctx.fill();
    
    // Draw Roasted juicy meat steak (brownish-red)
    ctx.fillStyle = '#ea580c'; // Sizzling orange-red roasted meat
    ctx.beginPath();
    ctx.ellipse(cx, cy, 7.5, 5.5, Math.PI / 4, 0, Math.PI * 2);
    ctx.fill();
    
    // Marbled interior details (fat ring highlight)
    ctx.strokeStyle = '#ffedd5'; // Light cream fat strip
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    ctx.arc(cx, cy, 2.2, 0, Math.PI * 2);
    ctx.stroke();
    
    ctx.restore();
}

// Draw Battle Axe Weapon (Glowing stone axe with wooden shaft and cyan double blade)
function drawAxe(cx, cy, tick) {
    ctx.save();
    
    // Glowing weapon aura
    ctx.shadowColor = '#06b6d4'; // Cyan neon axe glow
    ctx.shadowBlur = (tick % 2 === 0) ? 8 : 4;
    
    // Draw wooden shaft (brown diagonal handle)
    ctx.strokeStyle = '#78350f'; // Wood brown
    ctx.lineWidth = 3;
    ctx.lineCap = 'round';
    ctx.beginPath();
    ctx.moveTo(cx - 5, cy + 5);
    ctx.lineTo(cx + 4, cy - 4);
    ctx.stroke();
    
    // Draw double axe head (cyan scaly edge)
    ctx.fillStyle = '#06b6d4'; // Cyan steel blade
    ctx.beginPath();
    // Left crescent blade
    ctx.moveTo(cx + 2, cy - 2);
    ctx.quadraticCurveTo(cx - 3, cy - 8, cx + 5, cy - 9);
    ctx.quadraticCurveTo(cx + 3, cy - 3, cx + 2, cy - 2);
    // Right crescent blade
    ctx.moveTo(cx + 2, cy - 2);
    ctx.quadraticCurveTo(cx + 8, cy - 3, cx + 9, cy + 5);
    ctx.quadraticCurveTo(cx + 3, cy + 3, cx + 2, cy - 2);
    ctx.fill();
    
    // Blade edge highlights (silver/white)
    ctx.fillStyle = '#e2e8f0';
    ctx.beginPath();
    ctx.arc(cx + 5, cy - 9, 1.2, 0, Math.PI * 2);
    ctx.arc(cx + 9, cy + 5, 1.2, 0, Math.PI * 2);
    ctx.fill();
    
    ctx.restore();
}

// Draw beautifully animated swirling Concentric Vortex Escape Portal
function drawPortal(cx, cy, tick) {
    ctx.save();
    
    // Portal shifts through a spectrum of magic purple, magenta, and blue colors
    let hue = (tick * 6) % 360;
    ctx.shadowColor = `hsl(${hue}, 100%, 55%)`;
    ctx.shadowBlur = 16 + Math.sin(tick * 0.25) * 6;
    
    // Swirling concentric outer rings (larger, epic boundaries!)
    let numRings = 4;
    for (let r = 0; r < numRings; r++) {
        let radius = ((tick + r * 3.5) % 16) + 4.5;
        let opacity = 1.0 - (radius / 21);
        
        ctx.strokeStyle = `hsla(${(hue + r * 45) % 360}, 100%, 75%, ${opacity})`;
        ctx.lineWidth = 2.6;
        ctx.beginPath();
        // Shifting portal ellipse angle
        ctx.ellipse(cx, cy, radius * 0.8, radius * 1.25, tick * 0.04, 0, Math.PI * 2);
        ctx.stroke();
    }
    
    // Core event horizon (deep cosmic purple center, towering size!)
    ctx.fillStyle = '#0f051c';
    ctx.beginPath();
    ctx.ellipse(cx, cy, 6, 9.5, tick * 0.04, 0, Math.PI * 2);
    ctx.fill();
    
    // Rotating spark tracers around edge
    ctx.strokeStyle = `hsl(${hue}, 100%, 85%)`;
    ctx.lineWidth = 2.0;
    ctx.beginPath();
    ctx.arc(cx, cy, 6.2, tick * 0.08, tick * 0.08 + Math.PI, false);
    ctx.stroke();
    
    ctx.restore();
}

// CRT Terminal screen drawings
function renderScreen() {
    // Look up dynamic cavern theme details based on current wave level
    let theme = CAVERN_THEMES[(currentLevel - 1) % 4];

    // 1. Draw Textured Cavern slab stone tiles
    ctx.fillStyle = theme.floorBg;
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ctx.strokeStyle = theme.crack;
    ctx.lineWidth = 1.2;
    for (let x = 0; x < GRID_WIDTH; x += 4) {
        ctx.beginPath();
        ctx.moveTo(x * cellWidth, 0);
        ctx.lineTo(x * cellWidth + (x % 2 === 0 ? 8 : -8), canvas.height);
        ctx.stroke();
    }
    for (let y = 0; y < GRID_HEIGHT; y += 4) {
        ctx.beginPath();
        ctx.moveTo(0, y * cellHeight);
        ctx.lineTo(canvas.width, y * cellHeight + (y % 2 === 0 ? 8 : -8));
        ctx.stroke();
    }

    // 2. Draw Static Cavern boundary walls (themed color border)
    ctx.strokeStyle = theme.border;
    ctx.lineWidth = 2.5;
    ctx.strokeRect(cellWidth / 2, cellHeight / 2, canvas.width - cellWidth, canvas.height - cellHeight);

    // 3. Draw themed Rock Obstacles (layered with inner cracks)
    for (let obs of obstacles) {
        let rx = obs.x * cellWidth;
        let ry = obs.y * cellHeight;
        
        let rockGrad = ctx.createLinearGradient(rx, ry, rx + cellWidth, ry + cellHeight);
        rockGrad.addColorStop(0, theme.rockStart);
        rockGrad.addColorStop(1, theme.rockEnd);
        ctx.fillStyle = rockGrad;
        ctx.fillRect(rx + 1, ry + 1, cellWidth - 2, cellHeight - 2);
        
        // Inner fracture lines
        ctx.strokeStyle = theme.border;
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.moveTo(rx + 3, ry + 3);
        ctx.lineTo(rx + cellWidth - 3, ry + cellHeight - 3);
        ctx.moveTo(rx + cellWidth - 4, ry + 4);
        ctx.lineTo(rx + 4, ry + cellHeight - 4);
        ctx.stroke();
    }

    // 4. Draw Food Meat
    if (food.active) {
        drawMeat(food.x * cellWidth + cellWidth / 2, food.y * cellHeight + cellHeight / 2, frameTick);
    }

    // 5. Draw Battle Axe Weapon
    if (axe.active) {
        drawAxe(axe.x * cellWidth + cellWidth / 2, axe.y * cellHeight + cellHeight / 2, frameTick);
    }

    // 6. Draw swirling liquid Escape Portal
    if (escapeGate.active) {
        drawPortal(escapeGate.x * cellWidth + cellWidth / 2, escapeGate.y * cellHeight + cellHeight / 2, frameTick);
    }

    // Draw Caveman Human Player
    drawCaveman(player.x * cellWidth + cellWidth / 2, player.y * cellHeight + cellHeight / 2, player.dir, frameTick);

    // Draw Chaser Beasts (Dynamic Styles, Colors, and Proximity Rage Flashers)
    const beastColors = ['#ef4444', '#f59e0b', '#ec4899', '#06b6d4', '#3b82f6', '#f97316', '#a855f7', '#ffffff'];

    for (let i = 0; i < enemies.length; i++) {
        let enemy = enemies[i];
        if (!enemy.active) continue;

        // Calculate proximity distance to player
        let dx = enemy.x - player.x;
        let dy = enemy.y - player.y;
        let dist = Math.sqrt(dx * dx + dy * dy);

        let beastColor = beastColors[i % 8];
        let isRaged = (dist <= 5.0);

        if (enemy.type === 'DINO') {
            drawDino(enemy.x * cellWidth + cellWidth / 2, enemy.y * cellHeight + cellHeight / 2, beastColor, isRaged, frameTick);
        } else {
            drawMammoth(enemy.x * cellWidth + cellWidth / 2, enemy.y * cellHeight + cellHeight / 2, beastColor, isRaged, frameTick);
        }
    }
}

// Player Caught Helper (Supports 3 Lives & Stone Axe shield)
function playerCaught() {
    if (playerHasAxe) {
        // Find the beast that caught the player and defeat it!
        for (let enemy of enemies) {
            if (enemy.active && enemy.x === player.x && enemy.y === player.y) {
                enemy.active = false; // Kill only this one animal!
                break;
            }
        }
        playerHasAxe = false; // Consume the axe
        playRetroSound('levelup'); // Play success hit sound!
        updateHud();
        return; // Survives without losing a life!
    }

    playerLives--;
    playRetroSound('death');
    updateHud();

    if (playerLives > 0) {
        // Reset player coordinates to start position
        player.x = 2;
        player.y = 2;
        player.dir = 'NONE';
        
        // Reset all active beasts back to strategic positions to avoid instant respawn spawnkill
        for (let i = 0; i < enemies.length; i++) {
            let ex = GRID_WIDTH - 3;
            let ey = GRID_HEIGHT - 2;
            if (i === 1) { ex = GRID_WIDTH - 3; ey = 2; }
            else if (i === 2) { ex = 2; ey = GRID_HEIGHT - 2; }
            else if (i === 3) { ex = Math.floor(GRID_WIDTH / 2); ey = GRID_HEIGHT - 2; }
            else if (i === 4) { ex = Math.floor(GRID_WIDTH / 2); ey = 2; }
            enemies[i].x = ex;
            enemies[i].y = ey;
        }
    } else {
        gameOver(false);
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
