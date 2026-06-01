# A MAJOR PROJECT PROPOSAL ON CONSOLE CAVEMAN ESCAPE GAME

<br>

<div align="center">

**A MAJOR PROJECT PROPOSAL**

**ON**

### **CONSOLE CAVEMAN ESCAPE GAME: A REAL-TIME AI CHASING & EVASION ENGINE IN C**

<br>

**Submitted to:**
**Department of Bachelor of Computer Applications (BCA)**
**Nepal College of Information Technology (NCIT)**
**Pokhara University**

<br>

*In partial fulfillment of the requirements for the degree of Bachelor of Computer Applications (BCA)*

<br>

**Submitted by:**
**Sandesh Khatri (Exam Roll No: 26030145)**
**Aarush Shah (Exam Roll No: 26030148)**
**Gautam KC (Exam Roll No: 26030152)**

<br>

**Under the Supervision of:**
**Department of BCA Faculty Members**

<br>

**June 2026**

</div>

---

## TABLE OF CONTENTS
- [TITLE PAGE](#title-page)
- [CHAPTER 1: INTRODUCTION](#chapter-1-introduction)
  - [1.1 Background of the Study](#11-background-of-the-study)
  - [1.2 Problem Statement](#12-problem-statement)
  - [1.3 Objectives of the Study](#13-objectives-of-the-study)
  - [1.4 Project Schedule (Gantt Chart)](#14-project-schedule-gantt-chart)
- [CHAPTER 2: LITERATURE REVIEW](#chapter-2-literature-review)
  - [2.1 Review of Related Work](#21-review-of-related-work)
  - [2.2 Comparison Matrix of Architectural Approaches](#22-comparison-matrix-of-architectural-approaches)
- [CHAPTER 3: METHODOLOGY AND DESIGN](#chapter-3-methodology-and-design)
  - [3.1 Feasibility Study](#31-feasibility-study)
  - [3.2 Algorithmic Design](#32-algorithmic-design)
  - [3.3 Architectural Flowchart](#33-architectural-flowchart)
  - [3.4 Data Mechanics & Data Dictionary](#34-data-mechanics--data-dictionary)
- [CHAPTER 4: IMPLEMENTATION AND VERIFICATION](#chapter-4-implementation-and-verification)
  - [4.1 Code Repository Architecture](#41-code-repository-architecture)
  - [4.2 Verification and QA Testing Matrix](#42-verification-and-qa-testing-matrix)
- [CHAPTER 5: EXPECTED OUTCOMES & REFERENCES](#chapter-5-expected-outcomes--references)
  - [5.1 Deliverables and Target Performance Metrics](#51-deliverables-and-target-performance-metrics)
  - [5.2 References (IEEE Format)](#52-references-ieee-format)

---

## CHAPTER 1: INTRODUCTION

### 1.1 Background of the Study
The intersection of low-overhead real-time programming and automated pathfinding algorithms represents a core computational discipline in systems and game engineering. In the modern software engineering paradigm, dynamic graphic subsystems are heavily abstracted by monolithic game development engines such as Unity or Unreal. While convenient, these high-level frameworks mask the fundamental operational mechanics of CPU execution scheduling, standard input polling, thread synchronization, and direct memory-mapped buffer translation. Developing a raw console-based real-time game directly inside the Command Line Interface (CLI) using standard ANSI C (C99) strips away these layers of abstraction, exposing students to lower-level POSIX terminal behaviors, standard I/O driver modification, and resource-bounded procedural AI pathfinding.

The "Console Caveman Escape Game" is a deterministic real-time grid simulation that models a survival scenario. The player coordinates a primitive human entity (the "Caveman") across a two-dimensional grid populated by rigid obstacles, dynamic food items, and an automated predator entity (the "Beast"). To survive, the Caveman must navigate around stationary stone obstacles, collect sparse energy items to unlock the boundary exit, and evade the Beast, which is guided by a real-time pathfinding engine. The coordinate mechanics operate on a discrete Cartesian plane governed by the linear equation:

$$P = (x, y) \quad \text{where} \quad 0 < x < W - 1, \; 0 < y < H - 1$$

where $W$ and $H$ define the boundary width and height of the terminal playing grid, respectively. Implementing this simulation in pure ANSI C without relying on external system-level UI engines requires solving core academic computer science challenges: implementing non-blocking asynchronous keyboard events, compiling discrete coordinate vectors, and engineering a selective low-latency drawing subsystem.

### 1.2 Problem Statement
Developing real-time, interactive terminal applications in C99 using native standard libraries (`stdio.h`, `stdlib.h`) presents several critical limitations that degrade program stability and visual quality:

1. **Synchronous Standard Input Blockage**: Standard C functions such as `scanf()`, `getchar()`, and `fgets()` are fundamentally synchronous and blocking. Upon execution, they suspend the primary system thread, pausing the game loop until the user strikes the `Enter` key. In a dynamic arcade environment, the simulation's tick rate must remain constant; the Beast must chase and obstacles must trigger collisions continuously, regardless of whether the user provides keyboard inputs.
2. **Subprocess Invocation and Flicker Latency**: Traditionally, terminal refreshes are executed using primitive OS-level calls such as `system("clear")` or `system("cls")`. These calls fork the execution path, creating a separate shell subprocess that flushes the operating system's standard output pipeline. This heavy operations pipeline introduces massive CPU overhead, leading to visible horizontal flicker and frame rate drops that make fluid real-time interactions impossible.
3. **Automated Chase Pathfinding in Obstacle Grids**: Designing a low-overhead, automated chasing AI that calculates coordinate vectors toward a dynamic player while avoiding stationary obstacles is computationally demanding in memory-constrained environments. Naive pathfinders can easily enter infinite loops or get trapped behind simple obstacles when constrained to a discrete two-dimensional space.
4. **Volatility of High Scores**: Game configurations and high-score rankings are kept in volatile system RAM during runtime. Without a direct-mapped binary serialization framework that saves and loads data to a persistent non-volatile format, game records are permanently lost when the user exits the game or the program terminates unexpectedly.

This project addresses these technical challenges by developing custom, non-blocking POSIX input listeners, selective ANSI delta-rendering algorithms, a deterministic vector-shift chase AI, and direct structure binary disk serialization.

### 1.3 Objectives of the Study
The primary and secondary objectives of this project are outlined as follows:

#### Primary Objective
The central objective of this research project is to design, implement, and verify a highly responsive, resource-optimized, non-blocking real-time "Console Caveman Escape Game" in ANSI C (C99). The application will feature zero-flicker delta-rendering, deterministic coordinate-shift predator AI, and binary file score serialization under a POSIX-compliant Linux architecture.

#### Secondary Technical Objectives
* **Raw POSIX Input Overrides**: Implement terminal input control using the POSIX `termios` subsystem. By disabling canonical mode (`ICANON`) and input echo (`ECHO`), the program will process standard input keys instantly without requiring trailing newline triggers or pausing the main loop.
* **Flicker-Free Selective Rendering**: Program custom screen drawing modules using direct, raw ANSI escape sequences (`\033[y;xH`) to update only the changed coordinate cells (the previous positions of the player and enemy) instead of wiping the entire display buffer, achieving zero-flicker visuals.
* **Vector Pathfinding with Collision Evasion**: Engineer a deterministic, matrix-based pathfinding algorithm for the Beast. The AI will evaluate potential delta coordinate moves:

$$\Delta \vec{d} \in \{(0,1), (0,-1), (1,0), (-1,0)\}$$

and select the shift that minimizes the Euclidean distance to the player without violating boundary or obstacle matrices.
* **Robust File Serialization**: Design a binary save/load subsystem using direct memory-block serialization (`fwrite()`, `fread()`) to persist high-score lists and player achievements in a dedicated datastore (`high_scores.bin`).

### 1.4 Project Schedule (Gantt Chart)
The project will be executed over a 10-week cycle following an agile software development life cycle (SDLC):

| Week | Phase | Deliverables / Key Milestones | Sandesh | Aarush | Gautam |
|---|---|---|:---:|:---:|:---:|
| **W1** | **Requirement Gathering** | Define coordinate domains, trace ANSI capability matrix, refine project scope. | 📝 | 📝 | 📝 |
| **W2** | **Software Architecture** | Model common headers, map out data structures, outline interface bounds. | 📐 | 📐 | 🛠️ |
| **W3** | **Input Listener Dev** | Develop raw terminal modifications (`termios` non-blocking keyboard polling). | 🛠️ | 📐 | 📝 |
| **W4** | **Selective Renderer Dev** | Write the ANSI delta renderer, program precise cursor-positioning escapes. | 📐 | 🛠️ | 🛠️ |
| **W5** | **Core Game Loop Integr.**| Construct the primary execution clock, coordinate input and rendering ticks. | 🛠️ | 🛠️ | 🛠️ |
| **W6** | **Physics & Collisions** | Code Cartesian physics, detect collisions with walls, obstacles, and items. | 📐 | 🛠️ | 📐 |
| **W7** | **Predator AI Dev** | Program the vector distance calculator, implement pathfinding and evasion. | 🛠️ | 📐 | 🛠️ |
| **W8** | **Binary Records Subsys**| Construct the binary flat-file serialization system using raw struct streams. | 📐 | 🛠️ | 🛠️ |
| **W9** | **System QA & Debugging** | Perform unit test cases, run valgrind memory leak checks, profile performance.| 🧪 | 🧪 | 🧪 |
| **W10** | **Finalization & Review** | Draft the formal thesis document, prepare for final presentation and defense. | 🎓 | 🎓 | 🎓 |

*Legend: 📝 Requirement Analysis & Scope Specification | 📐 Architecture Design | 🛠️ Code Implementation | 🧪 Quality Verification & Profiling | 🎓 Thesis & Review*

---

## CHAPTER 2: LITERATURE REVIEW

### 2.1 Review of Related Work
Developing real-time, terminal-based applications is a well-established discipline in systems programming. Historically, standard terminal interaction is stream-buffered. As defined by **Kernighan and Ritchie (1988)** in *The C Programming Language*, standard input operations capture streams into local memory buffers that are only parsed once a trailing carriage return is detected. While this design is suitable for text processing tools (such as filters, compilers, and parsers), it cannot support real-time user input loop interfaces.

Historically, programmers bypassed these canonical input constraints on MS-DOS systems using compiler-specific, non-standard headers like `conio.h`. However, this approach is non-portable and incompatible with modern Unix-like systems. On modern operating systems, the standard approach is the `ncurses` (new curses) library. While `ncurses` provides sophisticated terminal control, windowing, and input mapping, it is a heavy runtime dependency. According to **Pressman and Maxim (2020)** in *Software Engineering: A Practitioner's Approach*, limiting external dependencies is crucial for code security and portability across various systems. By writing custom terminal configurations directly using the POSIX standard `termios.h`, our project eliminates these heavy external dependencies. This ensures that the application remains extremely lightweight, compiling and executing with zero installation requirements.

Low-level screen rendering performance has also been studied extensively. Traditional dynamic drawing methods rely on periodic terminal flushes, which introduce significant latency. Research demonstrates that writing raw, formatted ANSI character strings (conforming to the ECMA-48 standard) directly to the system's standard output pipeline allows the application to reposition the cursor and modify screen characters directly in local memory. This technique achieves high refresh rates with a minimal memory footprint.

Procedural chasing AI algorithms are a foundational topic in game design. **Millington (2019)**, in *AI for Games*, explains that while complex pathfinding algorithms like $A^*$ are necessary for complex maps, they introduce unnecessary overhead when applied to simple, discrete coordinate grids. For a low-overhead terminal application, a deterministic delta-vector calculation combined with local obstacle avoidance provides a highly efficient pathfinder. This approach achieves the desired chasing behavior while using negligible CPU and RAM resources.

### 2.2 Comparison Matrix of Architectural Approaches
The table below compares the technical dimensions of the proposed "Console Caveman Escape Game" with alternative game architectures:

| Technical Dimension | Graphic Engine-Based Game (SDL2 / OpenGL in C) | Web Canvas Architecture (HTML5 / JavaScript) | Proposed Console Engine (ANSI C99 / POSIX) |
|---|---|---|---|
| **RAM Footprint** | Moderate to High (~50 MB - 150 MB RAM) | High (~100 MB - 300 MB RAM) | **Extremely Low (< 2 MB RAM)** |
| **Host CPU Overhead**| Low (delegates rendering to GPU) | Moderate to High (relies on browser engine) | **Negligible (< 1% CPU utilization)** |
| **Input Capture Mode**| Native OS window event queue polling | Browser asynchronous event callbacks | **Direct non-blocking raw standard input polling via termios** |
| **Screen Refresh** | Full-buffer double swapping at 60Hz | Virtual DOM drawing / browser refresh loop | **Direct ANSI coordinate escape sequences** |
| **System Dependencies**| Dynamic library structures (.so, .dll) | Heavy web browsers and runtime environments | **None (a single self-contained executable binary)** |
| **Data Persistence** | SQLite databases or custom binary files | localStorage or IndexedDB database structures | **Direct flat-file structure binary serialization (`fwrite()`)** |

---

## CHAPTER 3: METHODOLOGY AND DESIGN

### 3.1 Feasibility Study
Before initiating development, we performed a thorough 4-point feasibility study to confirm the project's viability:

1. **Technical Feasibility**: The system utilizes standard C99 structures and native compiler libraries (`gcc` or `clang`). Operating exclusively via standard terminal interfaces, it requires no external libraries, ensuring high technical feasibility.
2. **Economic Feasibility**: The application is developed entirely using free, open-source software (GCC compiler, GDB debugger, Git version control, Linux host). It requires no expensive software licenses, posing zero economic risk.
3. **Operational Feasibility**: The game runs directly in any POSIX-compliant console interface. Since standard terminals are pre-installed on Linux, macOS, and WSL platforms, operational compatibility is high.
4. **Schedule Feasibility**: The project is divided into distinct, manageable milestones across 10 weeks. This timeline provides a realistic schedule with a comfortable margin for testing and academic documentation.

### 3.2 Algorithmic Design
The program's execution logic is guided by two deterministic algorithms: the Main Game Loop with Asynchronous Polling, and the Enemy Pathfinding Matrix.

#### Algorithm 1: Main Game Loop with Replay and Asynchronous Polling
```
================================================================================
ALGORITHM 1: Main Game Loop with Replay and Asynchronous Polling
================================================================================
Input: None
Output: Shell exit code (0 on success, >0 on failure)

1. SYSTEM INITIALIZATION:
   a. Query current terminal settings via tcgetattr() on STDIN_FILENO.
   b. Clone settings into BackupTermios to preserve original settings.
   c. Copy backup to ActiveTermios and disable ICANON and ECHO flags.
   d. Set ActiveTermios read limits: VMIN = 0, VTIME = 0 (non-blocking read).
   e. Apply ActiveTermios configurations using tcsetattr().
   f. Print ANSI escape sequence to hide console cursor (\033[?25l).
   g. Load highest record from "high_score.dat" into global high_score and high_score_name.
   h. Set play_again = TRUE.

2. RUNTIME REPLAY LOOP (While play_again is TRUE):
   a. STATE INITIALIZATION:
      i. Set game_active = TRUE, player_won = FALSE, current_score = 0, target_level_score = 30.
      ii. Set Player Position = (2, 2), Enemy Position = (18, 58).
      iii. Generate static stone obstacles matrix, escape gate at right wall.
      iv. Randomly generate initial Food gold star coordinate (★) avoiding obstacles.
      v. Clear terminal screen buffer and draw static retro Unicode box borders (╔, ║, ═, ╚, ╝).
      vi. Draw all static stone obstacles as shaded block glyphs (▓).

   b. EXECUTE SIMULATION ENGINE (While game_active is TRUE):
      i. Input Poll (Asynchronous):
         A. Query STDIN_FILENO state using select() with timeout = 0.
         B. If character is buffered: read key, map to direction vector.
      ii. Physics Step:
          A. Store previous coordinates: prev_player = player.pos, prev_enemy = enemy.pos.
          B. Update player position based on active movement direction.
      iii. Collision Evaluation:
           A. If player touches box boundaries or obstacle matrices: block movement, set player.dir = DIR_NONE.
           B. Verify if player is intercepted by beast -> set game_active = FALSE.
       iv. Target Check & Level Transition:
           A. If player.pos matches food.pos: increment current_score by 10.
           B. If current_score >= current_level * TARGET_SCORE: set escape_gate.active = TRUE (draw portal '▒').
           C. Generate new food gold star (★) avoiding obstacles and player/enemy coordinates.
           D. If escape_gate.active is TRUE and player.pos matches escape_gate.pos:
              1. LEVEL UP: Increment current_level by 1.
              2. Add 1 Beast: Increment enemy_count by 1.
              3. Reset coordinates: Place player at (2, 2) and deactivate escape gate.
              4. Respawn and reposition all active enemies in strategic corners.
              5. Refresh the entire static terminal board.
       v. Enemy AI Execution Tick:
          A. Execute Algorithm 2 for all active enemies: Enemy Pathfinding Matrix.
      vi. Selective Redraw (Delta-Rendering / Frame-Clock Animation):
          A. Erase prev_player and all prev_enemies coordinates with space cells (' ').
          B. Draw food gold star (★) and portal (▒) at their active coordinates.
          C. Draw player: alternate green directional arrow (▲, ▼, ◄, ►) or smiley (☻) and signature 'C'.
          D. Draw all active enemies: alternate red monster horns/claw (Ψ) and signature 'B' on their respective coordinates.
          E. Draw dashboard interface at bottom row with active stats.
          F. Flush standard output stream.
      vii. Frame Clock Delay:
           A. Pause execution thread for TICK_DURATION_MS (e.g., 150ms).

   c. STAGE TEARDOWN & PERSISTENCE:
      i. Restore default terminal canonical mode & echo attributes temporarily.
      ii. If current_score > high_score:
          A. Prompt user name, capture input from stdin.
          B. Save new high score record to binary persistence "high_score.dat".
      iii. Re-enable terminal raw mode & cursor overrides.
      iv. Draw static borders and overlay centered game over modal box (render_game_over_overlay).
      v. Poll raw standard input:
         A. If user presses 'Y' or 'y': set play_again = TRUE, break wait.
         B. If user presses 'N', 'n', or ESC: set play_again = FALSE, break wait.

3. FINAL SYSTEM TEARDOWN AND TERMINATION:
   a. Re-enable standard terminal settings via BackupTermios.
   b. Print ANSI escape sequence to restore cursor visibility (\033[?25h).
   c. Print final credits screen, high score records, and return 0.
================================================================================
```

#### Algorithm 2: Enemy Pathfinding Matrix
```
================================================================================
ALGORITHM 2: Enemy Pathfinding Matrix (Minimal Vector Shifts with Mutual Avoidance)
================================================================================
Input: enemies[MAX_ENEMIES] (current beast index e), player.pos (Px, Py),
       obstacle_matrix[MAX_OBSTACLES], GRID_WIDTH, GRID_HEIGHT, active enemy_count
Output: Updated coordinate for enemies[e]

1. Define candidate shift array Moves containing 4 direction vectors:
   Moves[0] = (0, -1)   [UP]
   Moves[1] = (0, 1)    [DOWN]
   Moves[2] = (-1, 0)   [LEFT]
   Moves[3] = (1, 0)    [RIGHT]

2. Set min_distance = INFINITY, optimal_move = (0, 0)

3. For each vector shift V in Moves:
   a. Compute candidate position: Cx = Ex + V.x, Cy = Ey + V.y.
   b. Verify boundary collisions:
      If Cx <= 0 Or Cx >= GRID_WIDTH-1 Or Cy <= 0 Or Cy >= GRID_HEIGHT-1:
         Skip to next vector.
   c. Verify obstacle collisions:
      If (Cx, Cy) matches any coordinate in obstacle_matrix:
         Skip to next vector.
   d. Verify mutual enemy collision avoidance:
      If (Cx, Cy) matches any other active enemies[other_e].pos (where other_e != e):
         Skip to next vector.
   e. Calculate Euclidean distance to player:
      Distance = sqrt( (Cx - Px)^2 + (Cy - Py)^2 )
   f. If Distance < min_distance:
      min_distance = Distance
      optimal_move = V

4. Update current enemy position:
   enemies[e].pos.x = enemies[e].pos.x + optimal_move.x
   enemies[e].pos.y = enemies[e].pos.y + optimal_move.y

5. Return
================================================================================
```

### 3.3 Architectural Flowchart
The application's structural control flow and modular subsystems are mapped in the flowchart below:

```
+-------------------------------------------------------------------------+
|                          SYSTEM INITIALIZATION                          |
|  - Load termios.h raw overrides (disable canonical input & echo)        |
|  - Emit ANSI hide cursor code (\033[?25l)                               |
|  - Load binary high score from persistent datastore "high_score.dat"    |
|  - Populate coordinate arrays and register standard exit handlers      |
+-------------------------------------------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|                            LEVEL STAGE SETUP                            |
|  - Position Caveman (2, 2) and Beast (GRID_HEIGHT-2, GRID_WIDTH-2)      |
|  - Generate static obstacles matrix, assign coordinate targets (Food)   |
|  - Render fixed border layout and grid obstacles onto console           |
+-------------------------------------------------------------------------+
                                     |
                                     v
                        ===========================
                      //    PRIMARY GAME LOOP      \\
                      =========================== <-----------------------+
                                     |                                    |
                                     v                                    |
                      +-----------------------------+                     |
                      |  POLL KEYBOARD INTERRUPTS   |                     |
                      | (Non-blocking POSIX select) |                     |
                      +-----------------------------+                     |
                                     |                                    |
                      /=============\/=============\                      |
                     /    Is keyboard hit ready?    \                     |
                     \                              /                     |
                      \=============\/=============\                      |
                              |               |                           |
                      YES     |               | NO                        |
                              v               |                           |
                +---------------------------+ |                           |
                | Parse key, apply direction| |                           |
                | (Filter out opposite turn)| |                           |
                +---------------------------+ |                           |
                              |               |                           |
                              +------->-------+                           |
                                      |                                   |
                                      v                                   |
                        +---------------------------+                     |
                        |   PHYSICS UPDATE TICK     |                     |
                        |  - Update player coord    |                     |
                        |  - Store previous state   |                     |
                        +---------------------------+                     |
                                      |                                   |
                                      v                                   |
                        /=============\/=============\                    |
                       /     Check collisions?      \                   |
                       \    - Outer Wall Matrix     /                    |
                       \    - Obstacles Map         /                    |
                       \    - Beast Predator Check  /                    |
                        /=============\/=============\                    |
                              |               |                           |
                      YES     |               | NO                        |
                              v               |                           |
                        [GAME OVER]           v                           |
                              |        /=============\/=============\     |
                              |       /      Is Food collected?      \    |
                              |       \                             /     |
                              |        /=============\/=============\     |
                              |               |               |           |
                              |       YES     |               | NO        |
                              |               v               |           |
                              |        +------------+         |           |
                              |        | Score + 10 |         |           |
                              |        | Spawn Food |         |           |
                              |        | Verify Gate|         |           |
                              |        +------------+         |           |
                              |               |               |           |
                              |               +------->-------+           |
                              |                       |                   |
                              v                       v                   |
                              |         +---------------------------+     |
                              |         |   BEAST PATHFINDING AI    |     |
                              |         | (Compute coordinate shift) |    |
                              |         +---------------------------+     |
                              |                       |                   |
                              v                       v                   |
                              |         +---------------------------+     |
                              |         |   SELECTIVE REDRAW TICK   |     |
                              |         | - Overwrite old tail      |     |
                              |         | - Print target & head     |     |
                              |         | - Update Score Header     |     |
                              |         +---------------------------+     |
                              |                       |                   |
                              v                       v                   |
                              |         +---------------------------+     |
                              |         |   GAME ENGINE CLOCK DELAY |     |
                              |         | (Sleep for 150ms duration)  |     |
                              |         +---------------------------+     |
                              |                       |                   |
                              |                       +--------->---------+
                              |
                              v
+-------------------------------------------------------------------------+
|                            SYSTEM SHUTDOWN                              |
|  - If current_score > high_score: save score to binary file             |
|  - Restore terminal original canonical state and input echo             |
|  - Emit ANSI escape sequence to restore cursor (\033[?25h)               |
|  - Print final stats summary and exit program cleanly                   |
+-------------------------------------------------------------------------+
```

### 3.4 Data Mechanics & Data Dictionary
To ensure exact alignment and predictable memory sizes across varying 32-bit and 64-bit platforms, all core structures and state parameters are mapped in the following data dictionary:

| Variable Name | Native Data Type | Size (Bytes) | Scope | Functional Description |
|---|---|---|---|---|
| `player` | `Player` (Struct) | 12 bytes | Global | Manages the player's Cartesian position coordinates `x` and `y`, along with the current movement direction. |
| `enemy` | `Enemy` (Struct) | 8 bytes | Global | Manages the dynamic coordinate vector of the predator, along with its active state flag. |
| `food` | `Target` (Struct) | 8 bytes | Global | Manages the coordinate vector of the active food target, along with its active state flag. |
| `escape_gate` | `Target` (Struct) | 8 bytes | Global | Manages the exit coordinate vector, which becomes active when score requirements are met. |
| `obstacles[50]` | `Obstacle` Array | 400 bytes | Global | Stores coordinates for stationary obstacles (rocks). Used for obstacle collision and pathfinder checks. |
| `current_score` | `unsigned int` | 4 bytes | Global | Tracks the player's active score. Increments by 10 points for each collected food item. |
| `high_score` | `unsigned int` | 4 bytes | Global | Stores the high score loaded from the persistent binary file. Used to evaluate new high scores. |
| `orig_termios` | `struct termios` | ~60 bytes | Global | Stores the system's original terminal parameters. Used to restore canonical settings on program exit. |
| `saveData` | `ScoreRecord` (Struct) | 36 bytes | Local | Structured variable containing the player's name and high score, used for direct binary serialization. |

---

## CHAPTER 4: IMPLEMENTATION AND VERIFICATION

### 4.1 Code Repository Architecture
To maintain a clean separation of concerns and enforce modular code design, the repository is structured into distinct source and header files:

```
caveman/
├── Makefile
├── common.h
├── physics.h
├── physics.c
├── enemy_ai.h
├── enemy_ai.c
├── display.h
├── display.c
├── records.h
├── records.c
└── main.c
```

#### Modular File Roles:
* **`common.h`**: The central header. Defines global constants (`GRID_WIDTH`, `GRID_HEIGHT`, `TICK_DURATION`), enums, data structures, and declares shared global variables.
* **`physics.h` / `physics.c`**: Implements basic Cartesian mechanics. Manages coordinate updates, boundary checks, and collision detection logic for walls and obstacles.
* **`enemy_ai.h` / `enemy_ai.c`**: Implements pathfinding mechanics. Calculates optimal coordinate shifts toward player coordinates while evading obstacles.
* **`display.h` / `display.c`**: Manages cursor positioning and board drawing. Uses optimized ANSI escape sequences to perform selective, flicker-free rendering.
* **`records.h` / `records.c`**: Handles persistent storage. Saves and loads score records directly to and from a binary flat-file.
* **`main.c`**: The system's main entry point. Coordinates initialization, drives the primary execution loop, and handles terminal restoration on exit.

### 4.2 Verification and QA Testing Matrix
To verify that the system runs reliably across target platforms, we have established a rigorous QA validation matrix comprising five critical test scenarios:

| Test ID | Subsystem | Action / Input Condition | Expected System Behavior | Pass Criteria |
|---|---|---|---|---|
| **TC-001** | **Input & System Init** | Launch game executable. | Terminal transitions to raw mode. Input characters do not print to the terminal, and key presses are captured instantly without needing `Enter`. | Keystrokes are captured instantly. Terminal settings are restored correctly on exit. |
| **TC-002** | **Wall Boundary** | Set Caveman position to $x = 1$, movement direction to `DIR_LEFT`. | The collision detection routine flags the boundary intersection, stops active movement by setting `player.dir = DIR_NONE`, and maintains coordinates. | The player is safely blocked at the boundary; the game loop remains active, preventing out-of-bounds movement. |
| **TC-003** | **Obstacle Collision** | Position Caveman adjacent to an obstacle, move directly into it. | The collision detection routine flags the obstacle contact, stops active movement by setting `player.dir = DIR_NONE`, and maintains coordinates. | The player is blocked by the obstacle; the game loop remains active, preventing coordinate overlap. |
| **TC-004** | **Input Filter** | Set current direction to `DIR_RIGHT`. Input key `a` (which maps to `DIR_LEFT`). | The input handler intercepts the key, identifies a 180-degree turn conflict, ignores the input, and maintains `DIR_RIGHT`. | The player continues moving to the right. The player's coordinate movement remains stable. |
| **TC-005** | **Binary Serialization** | Complete a game session with a score higher than the active high score. | The storage module opens `high_score.dat` in write-binary (`wb`) mode, serializes the new high score, and saves it to disk. | Subsequent executions read the binary file, load the new high score, and display it correctly on startup. |

---

## CHAPTER 5: EXPECTED OUTCOMES & REFERENCES

### 5.1 Deliverables and Target Performance Metrics
The system is designed to provide a responsive, flicker-free console game experience while maintaining minimal resource usage. The table below lists our target performance metrics:

| Performance Metric | Target Metric Requirement | Verification Methodology |
|---|---|---|
| **RAM Footprint** | **< 2.0 Megabytes (MB)** | Profile memory usage using standard system tools (`valgrind`, `top`, or `pmap`). |
| **Host CPU Overhead** | **< 1% Host CPU Overhead** | Verify CPU utilization cycles under full loop ticks using CPU monitors (`htop`). |
| **Visual Refresh Flicker Rate** | **0% Flicker (Complete Elimination)** | Verify delta rendering updates visually and check for redraw calls in output streams. |
| **Input Response Latency** | **< 5 Milliseconds (ms)** | Measure elapsed time from input capture to player position coordinate updates. |
| **Binary Storage Payload Size** | **36 Bytes (Fixed Struct Format)** | Inspect the file size of the generated binary `high_score.dat` file. |
| **Static Executable Size** | **< 64 Kilobytes (KB)** | Check the size of the compiled static ELF binary under `-O2` optimization. |

### 5.2 References (IEEE Format)
* [1] B. W. Kernighan and D. M. Ritchie, *The C Programming Language*, 2nd ed. Englewood Cliffs, NJ: Prentice Hall, 1988.
* [2] R. S. Pressman and B. R. Maxim, *Software Engineering: A Practitioner's Approach*, 9th ed. New York, NY: McGraw-Hill Education, 2020.
* [3] IEEE Standard for Information Technology - Portable Operating System Interface (POSIX(R)) Base Specifications, Issue 7, IEEE Std 1003.1-2017, Jan. 2018.
* [4] International Organization for Standardization, *Programming Languages — C*, ISO/IEC 9899:1999 (C99), Dec. 1999.
* [5] W. R. Stevens and S. A. Rago, *Advanced Programming in the UNIX Environment*, 3rd ed. Boston, MA: Addison-Wesley, 2013.
* [6] ECMA International, *Control Functions for Coded Character Sets*, Standard ECMA-48, 5th ed., June 1991.
* [7] A. Robbins, *Linux Programming by Example: The Fundamentals*, Upper Saddle River, NJ: Prentice Hall, 2004.
