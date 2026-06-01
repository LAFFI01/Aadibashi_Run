# 📖 Console Caveman Escape Game: User Manual

Welcome to the official User Manual for the **Console Caveman Escape Game**! This guide will walk you through cloning the source repository from GitHub, installing prerequisites, compiling the game, playing, and troubleshooting common terminal parameters.

---

## 💻 1. System Requirements

The game engine is built on standard C99 structures and utilizes POSIX-compliant terminal manipulation headers (`<termios.h>`, `<unistd.h>`, `<sys/select.h>`). It is fully supported on the following operating systems:

*   **Linux**: Ubuntu, Debian, Arch Linux, Fedora, CentOS, etc. (Native support).
*   **macOS**: macOS Terminal, iTerm2 (Native support).
*   **Windows**: Windows Subsystem for Linux (WSL) or MSYS2/MinGW.

### 📋 Prerequisites

To pull, compile, and run the game, you need a C compiler and standard makefile tooling installed.

#### installing Build Tools:
*   **Debian/Ubuntu**:
    ```bash
    sudo apt update
    sudo apt install build-essential git
    ```
*   **Arch Linux**:
    ```bash
    sudo pacman -S base-devel git
    ```
*   **Fedora/RedHat**:
    ```bash
    sudo dnf groupinstall "Development Tools" && sudo dnf install git
    ```
*   **macOS** (Via Homebrew or Xcode command line tools):
    ```bash
    xcode-select --install
    ```

---

## 🚀 2. Getting Started (Cloning and Building)

### Step 2.1: Clone the Repository
Open a terminal window and execute the standard git clone routine:

```bash
git clone https://github.com/LAFFI01/Aadibashi_Run.git
```

Navigate inside the project directory:

```bash
cd Aadibashi_Run
```

### Step 2.2: Compile the Source
The project includes a robust production `Makefile` configured to automate intermediate compilation and linking:

```bash
make clean && make
```

*This cleans any leftover binary configurations and builds a clean warning-free executable under `bin/caveman_escape`.*

### Step 2.3: Launch the Game
Run the compiled executable directly inside your active shell:

```bash
./bin/caveman_escape
```

---

## 🕹️ 3. Gameplay Instructions & Controls

### ⌨️ Active Steering Controls
The low-latency input engine supports two layouts, processing navigation instantly with raw STDIN queue-flushing loops:

*   **WASD Layout**:
    *   `W` or `w`: Move Up
    *   `S` or `s`: Move Down
    *   `A` or `a`: Move Left
    *   `D` or `d`: Move Right
*   **Arrow Keys Layout**:
    *   `▲`: Move Up
    *   `▼`: Move Down
    *   `◄`: Move Left
    *   `►`: Move Right
*   **Escape/Quit**:
    *   Press the standalone **`Esc`** key during gameplay to trigger an instant canonical terminal restoration and safe exit.

---

## 🗺️ 4. Visual Legends and Mechanics

| Element Symbol | Name | Description |
   |:---:|:---:|---|
   | **`☻` / `C` / Arrows** | **Caveman (Player)** | Represents the player. Character alternates dynamically based on heading direction, frames, or standing ticks. |
   | **`★` / `☆`** | **Food Stars** | Twinkling stars scattered in the cave. Collect them to earn points (+10 score per star). |
   | **`B` / `M` / `D` / `H`** | **Chaser Beasts** | Intelligent colored monster chasers hunting you using minimal distance vectors. |
   | **`☠`** | **Rage Proximity Warning** | When a beast gets within a **5-tile radius**, its icon flashes aggressively as a blinking red skull `☠`, warning you of mortal danger! |
   | **`▒` / `░` / `▓`** | **Escape Gate (Portal)** | A liquid pulsing portal opening on the right wall once **10 stars** (100 score points) are collected. Entering it triggers a level up. |

### 🌊 Level Up Wave Progression
The game does not end after a single screen! Entering the active portal triggers a **Level Up Wave**:
1. Your character resets to the starting top-left corner `(2, 2)`.
2. The gate locks, and new food is generated.
3. **An additional beast is added to the horde** (up to 20 maximum). Active beasts spawn in opposite strategic corners of the arena (top-right, bottom-left, etc.) to try and surround you.
4. Each level requires collecting another **10 food stars** to unlock the exit portal.

---

## 🔧 5. Troubleshooting Guide

### ⚠️ Issue: Terminal Characters Look Jagged or Wrapped
*   **Cause**: The playing grid is `60` columns wide and the HUD scoreboard renders below row `20`. If your terminal window is too small, text wrapping will disrupt the border walls.
*   **Fix**: Simply stretch or maximize your terminal window (minimum size recommended: **80 columns by 25 rows**) and restart the game.

### ⚠️ Issue: The Terminal Cursor is Missing After Game Quits
*   **Cause**: The game hides the terminal cursor `\033[?25l` to provide a premium arcade feeling. If the game crashes, terminates abruptly, or is interrupted (e.g., via `Ctrl+C`), terminal overrides might not restore cleanly.
*   **Fix**: Simply type the standard shell restore command and press Enter:
    ```bash
    reset
    ```
    This instantly resets terminal configurations back to defaults.

### ⚠️ Issue: Compilation Fails with "math.h not found" or "gcc not found"
*   **Cause**: Developer compilers or math libraries are not linked or missing in your OS package manager.
*   **Fix**: Ensure `build-essential` (Linux) or Xcode Command Line Tools (macOS) are fully installed using the package manager commands listed in Section 1.
