# Enniolimpiadi 2026

A tournament management application built with C, raylib, and Clay UI. Manage players, create events, and run single-elimination or group-stage tournaments with an interactive bracket system.

## Features

- **Dashboard** - Overview of events, players, and completed tournaments with medal standings
- **Player Management** - Add and manage participants
- **Event Management** - Create and configure tournament events
- **Tournament Formats**:
  - Single Elimination brackets
  - Group stage with round-robin matrices
- **Interactive Brackets** - Click to advance winners, right-click to undo

## Prerequisites

You need the following installed on your system:

### macOS (via Homebrew)

```bash
# Install Xcode Command Line Tools (includes clang)
xcode-select --install

# Install pkg-config and raylib
brew install pkg-config raylib
```

### Linux (Debian/Ubuntu)

```bash
sudo apt install clang pkg-config libraylib-dev
```

### Linux (Fedora)

```bash
sudo dnf install clang pkg-config raylib-devel
```

## Building

Clone the repository and run the build script:

```bash
git clone https://github.com/tomtom-95/enniolimpiadi2026.git
cd enniolimpiadi2026
./build.sh enniolimpiadi
```

The executable will be created in the `build/` directory.

### Build Commands

| Command | Description |
|---------|-------------|
| `./build.sh enniolimpiadi` | Compile the main application |
| `./build.sh tests` | Compile the test suite |
| `./build.sh clean` | Remove the build directory |
| `./build.sh -p enniolimpiadi` | Run preprocessor only (outputs .i file) |

## Running

After building, run the application:

```bash
./build/enniolimpiadi
```

## Project Structure

```
enniolimpiadi2026/
├── enniolimpiadi.c      # Main entry point
├── layout.c/.h          # UI layout and rendering (Clay)
├── players.c/.h         # Player and tournament entity management
├── arena.c/.h           # Custom arena memory allocator
├── string.c/.h          # Custom string utilities
├── core.h               # Core type definitions
├── clay.h               # Clay UI library (single-header)
├── raylib/              # Raylib renderer integration
├── resources/           # Fonts and assets
├── tests/               # Test suite
└── build.sh             # Build script
```

## Controls

- **Left-click** on a player name in a bracket to advance them to the next round
- **Right-click** on a player name to undo their advancement
- **Cmd +/-** while hovering over the bracket to zoom in/out
