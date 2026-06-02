# 📖 Console Caveman Escape Game: User Manual

Welcome to the official User Manual for the **Console Caveman Escape Game**! This guide will walk you through cloning the source repository from GitHub, installing prerequisites, compiling the game, playing, and troubleshooting common terminal parameters.

---

## 💻 1. System Requirements & Platform Compatibility

The game engine is built on standard C99 structures and utilizes POSIX-compliant terminal manipulation headers (`<termios.h>`, `<unistd.h>`, `<sys/select.h>`). 

### 🖥️ Supported Environments:
*   **Linux**: Ubuntu, Debian, Arch Linux, Fedora, CentOS, etc. (Native support).
*   **macOS**: macOS Terminal, iTerm2 (Native support).
*   **Windows**: 100% supported through compatibility layers (WSL / MSYS2 / Git Bash). *Does not run natively inside raw Command Prompt (cmd.exe) or PowerShell.*

### 📋 Prerequisites & Build Tool Installation

To pull, compile, and run the game, you need a C compiler and standard makefile tooling installed for your platform.

#### 🐧 Linux (Native)
Install development packages using your standard package manager:
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

#### 🍏 macOS (Native)
Install command line utilities via your terminal:
```bash
xcode-select --install
```

#### 🪟 Windows Setup (Three Support Options)

##### Option A: Windows Subsystem for Linux (WSL / WSL2) (Highly Recommended 🌟)
WSL provides a native Linux kernel and environment inside Windows.
1. Open PowerShell as Administrator and install WSL:
   ```powershell
   wsl --install
   ```
2. Restart your computer if prompted.
3. Open your newly installed Linux app (e.g. **Ubuntu**) from the Start menu.
4. Run standard commands to install compilation tools:
   ```bash
   sudo apt update && sudo apt install build-essential git
   ```
5. Clone and build the game as normal!

##### Option B: MSYS2 / MinGW-w64 (POSIX Translation Emulation)
MSYS2 compiles POSIX Unix code into native Windows `.exe` binaries:
1. Download and install [MSYS2](https://www.msys2.org/).
2. Open the **MSYS2 UCRT64** terminal from the Start menu.
3. Run the following command to update and install compilers and tools:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc make git
   ```
4. Clone and compile the game:
   ```bash
   git clone https://github.com/LAFFI01/Aadibashi_Run.git
   cd Aadibashi_Run
   make clean && make
   ./bin/caveman_escape
   ```

##### Option C: Git Bash
1. Install [Git for Windows](https://git-scm.com/) (which comes prepackaged with Git Bash).
2. Open the **Git Bash** shell.
3. If MinGW or GCC tools are linked in your system PATH, you can run `make clean && make` to compile and play the game directly inside Git Bash.

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

### Step 2.4: Alternative - Launch the Web Arcade Edition
If you prefer not to compile native binary C code, or want to play instantly inside any modern web browser (fully compatible with laptops, tablets, and mobile touchscreens):
1. Navigate to the `web/` directory.
2. Double-click or open the [`web/index.html`](../web/index.html) file directly inside any web browser.
3. Alternatively, you can spin up a quick, lightweight local static server using Python:
   ```bash
   # Run a quick local static server
   python3 -m http.server 8000
   ```
   Then navigate to `http://localhost:8000/web/` in your web browser to enjoy full CRT simulated graphics and Web Audio synth sound effects!

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
