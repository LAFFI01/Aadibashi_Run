# 🎮 Console Caveman Escape Game

> A Real-Time AI Chasing & Evasion Engine in Modular ANSI C (C99).

```text
  ____                               _
 / ___|__ ___   _____ _ __ ___   ___| |__
| |   / _` \ \ / / _ \ '_ ` _ \ / _ \ '_ \
| |__| (_| |\ V /  __/ | | | | |  __/ | | |
 \____\__,_| \_/ \___|_| |_| |_|\___|_| |_|
  _____                               ____
 | ____|___  ___ __ _ _ __   ___     / ___| __ _ _ __ ___   ___
 |  _| / __|/ __/ _` | '_ \ / _ \   | |  _ / _` | '_ ` _ \ / _ \
 | |___\__ \ (_| (_| | |_) |  __/   | |_| | (_| | | | | | |  __/
 |_____|___/\___\__,_| .__/ \___|    \____|\__,_|_| |_| |_|\___|
                     |_|
```

This repository houses a modular, real-time arcade console game written in standard **C99**. The engine operates on terminal-level delta rendering, raw Unix input queue flushing, and dynamic vector-based pathfinding, providing a fully functional, high-performance, and flicker-free gameplay loop right inside your bash shell.

Developed as an academic **Major Project Proposal** for a Bachelor of Computer Application (BCA) program at **NCIT, Pokhara University**.

---

## ✨ Features & Systems Engineering

*   **⚡ Zero-Latency Input Queue Flushing**: Fully resolves keypress buffering lag in standard raw Unix STDIN. Polling loops consume and flush the input buffer, keeping only the single most recent steering command for exceptionally crisp and responsive player control.
*   **🧠 Intelligent Multi-Beast Chasing AI**: Each active beast dynamically tracks the Caveman using localized Euclidean distance vector optimization. 
*   **🛡️ Mutual Beast Collision Avoidance**: Beasts evaluate coordinates of other active beasts and skip overlapping squares. This forces the beast pack to naturally fan out, split up corridors, and surround the player, eliminating coordinate stacking issues.
*   **🌊 Dynamic Level-Up & Horde Progression**: Once you collect **10 food stars** on the board, the blinking magenta escape gate `▒` activates on the right wall. Stepping through triggers a level up: your position resets, the map refreshes, and **one additional chaser beast is added to the arena** (up to 20 maximum).
*   **🌟 Premium Terminal Animations & Visuals**:
    *   **Sparkling Stars**: Targets pulse between hollow `☆` and filled `★` shapes to create a glittering effect.
    *   **Liquid Energy Portal**: The escape gateway shifts through dynamic Unix shading densities (`░` $\rightarrow$ `▒` $\rightarrow$ `▓` $\rightarrow$ `▒` $\rightarrow$ `░`), mimicking a flowing energy shield.
    *   **4-State Player Running Cycle**: Caveman cycles through base character `C`, dynamic movement arrows (`▲`, `▼`, `◄`, `►`), and idle smileys `☻`.
    *   **🚨 Proximity Rage Mode Alert**: When a beast closes within a **5-tile radius** of the Caveman, it enters **Rage Mode**—flashing at double frequency between its character and a blinking red danger skull symbol `☠`, giving the player a highly intuitive tactical warning.
*   **💾 High Score Binary Persistence**: Dynamic serialization keeps track of the global high score and the record-holder's name, persisting records across runs using a compact binary `.dat` file.

---

## 📂 Production Folder Structure

The project has been architected under industry standard modular guidelines:

```text
.
├── Makefile                # Production build system Makefile
├── .gitignore              # Ignores build artifacts and score files
├── README.md               # Visual repository documentation (this file)
├── bin/                    # Production binary executables directory
│   └── caveman_escape      # Recompiled standalone game executable
├── build/                  # Intermediate object compilation files (*.o)
├── data/                   # Dynamic gameplay local files
│   └── high_score.dat      # Persisted binary high score records
├── docs/                   # Academic proposal papers and docs
│   └── proposal.md         # Formal NCIT BCA Major Project Proposal
├── include/                # Centralized C header interfaces (*.h)
│   ├── common.h
│   ├── display.h
│   ├── enemy_ai.h
│   ├── physics.h
│   └── records.h
└── src/                    # Modular source file implementations (*.c)
    ├── display.c
    ├── enemy_ai.c
    ├── main.c
    ├── physics.c
    └── records.c
```

---

## 🛠️ Quickstart (Compile & Run)

### 📋 Prerequisites

*   A Linux environment (Ubuntu, Debian, Fedora, Arch, etc.) or macOS.
*   A C compiler (e.g., `gcc` or `clang`).
*   Standard terminal utilities (`make` and standard POSIX system libraries).

### ⚙️ Compilation

To clean previous builds and compile the workspace:

```bash
make clean && make
```

This creates the intermediate compiler objects in `build/` and links them into a single executable `bin/caveman_escape`.

### 🚀 Running the Game

Launch the executable directly from your terminal:

```bash
./bin/caveman_escape
```

---

## 🎮 How to Play

*   **Move**: Use **`WASD`** keys or **Arrow Keys** to steer the Caveman.
*   **Objective**: Collect **10 twinkling stars** `★` to unlock the cave.
*   **Escape**: Once unlocked, run to the blinking portal `▒` on the right wall to level up and advance.
*   **Survive**: Avoid the chaser beasts (`B`, `M`, `D`, `H`, etc.). Watch out when their icons turn into flashing skulls `☠`—they are in pouncing proximity!
*   **Exit**: Press the standalone **`Esc`** key during gameplay to quit immediately.

---

## 📝 Academic Project Details

*   **Course**: Bachelor of Computer Application (BCA), Semester Major Project
*   **University**: National College of Information Technology (NCIT), Pokhara University
*   **Title**: *Console Caveman Escape Game: A Real-Time AI Chasing & Evasion Engine in C*
*   **Authors**:
    *   **Sandesh Khatri**
    *   **Aarush Shah**
    *   **Gautam KC**
*   **Date**: June 2026

*The complete formal project proposal paper detailing mathematical algorithms, terminal latency solutions, asynchronous select-based loops, and structural coordinate algorithms is documented in [docs/proposal.md](docs/proposal.md).*

---

## 📄 License

This project is open-source and licensed under the **MIT License**.
