#!/bin/bash

# ------------------------------------------------------------------------------
# Common
# ------------------------------------------------------------------------------
SRC="enniolimpiadi.c"
BUILD_DIR="build"
DEBUG_OUT="enniolimpiadi-debug"
RELEASE_OUT="enniolimpiadi-release"

# ------------------------------------------------------------------------------
# Debug build (debug, tests)
# - AddressSanitizer enabled
# - No optimization, debug symbols
# - Dynamically links raylib (requires raylib installed)
# ------------------------------------------------------------------------------
DEBUG_CFLAGS="-fcolor-diagnostics -fansi-escape-codes -fsanitize=address -std=c99 -Wall -Wconversion -pedantic -g -O0"
DEBUG_RAYLIB=$(pkg-config --cflags --libs raylib)

# ------------------------------------------------------------------------------
# Release build (release)
# - Optimized, no debug symbols
# - Statically links raylib (portable executable)
# ------------------------------------------------------------------------------
RELEASE_CFLAGS="-std=c99 -Wall -Wconversion -pedantic -O2"
RELEASE_RAYLIB_INCLUDES=$(pkg-config --cflags raylib)
RELEASE_RAYLIB_STATIC="/opt/homebrew/lib/libraylib.a"
RELEASE_MACOS_FRAMEWORKS="-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL"

# Parse flags
PREPROCESS_ONLY=false
if [[ "$1" == "-p" ]]; then
    PREPROCESS_ONLY=true
    shift  # Remove -p from arguments
fi

# Show help message
show_help() {
    echo "Usage: $0 [-p] [debug|tests|release|clean]"
    echo
    echo "  -p               Run preprocessor only (outputs .i file)"
    echo
    echo "  debug            Compile $SRC -> $DEBUG_OUT (with AddressSanitizer)"
    echo "  tests            Compile tests.c -> tests"
    echo "  release          Compile $SRC -> $RELEASE_OUT (distributable, statically linked)"
    echo "  clean            Remove the build directory"
    echo
    echo "Examples:"
    echo "  $0 debug         # Compile debug build"
    echo "  $0 -p debug      # Preprocess only"
    echo "  $0 release       # Build distributable executable"
    echo
}

# No argument passed → show help
if [[ $# -eq 0 ]]; then
    echo "❌ No command provided."
    show_help
    exit 1
fi

# Handle 'clean' command first
if [[ "$1" == "clean" ]]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "🧹 Cleaned."
    exit 0
fi

# Ensure build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir "$BUILD_DIR"
fi

# Choose what to compile
case "$1" in
    debug)
        if [ "$PREPROCESS_ONLY" = true ]; then
            echo "Running preprocessor on $SRC -> $DEBUG_OUT.i"
            clang -E $SRC -o $BUILD_DIR/$DEBUG_OUT.i $DEBUG_RAYLIB
        else
            echo "Compiling $SRC -> $DEBUG_OUT"
            clang $DEBUG_CFLAGS $SRC -o $BUILD_DIR/$DEBUG_OUT $DEBUG_RAYLIB
        fi
        ;;
    tests)
        if [ "$PREPROCESS_ONLY" = true ]; then
            echo "Running preprocessor on tests.c -> tests.i"
            clang -E tests/tests.c -o $BUILD_DIR/tests.i $DEBUG_RAYLIB
        else
            echo "Compiling tests.c -> tests"
            clang $DEBUG_CFLAGS tests/tests.c -o $BUILD_DIR/tests $DEBUG_RAYLIB
        fi
        ;;
    release)
        echo "Compiling $SRC -> $RELEASE_OUT (release build, statically linked)"
        clang $RELEASE_CFLAGS $SRC -o $BUILD_DIR/$RELEASE_OUT $RELEASE_RAYLIB_INCLUDES $RELEASE_RAYLIB_STATIC $RELEASE_MACOS_FRAMEWORKS
        if [ $? -eq 0 ]; then
            echo "Checking dependencies..."
            otool -L $BUILD_DIR/$RELEASE_OUT | grep -v "/usr/lib\|/System"
            if [ $? -ne 0 ]; then
                echo "Only system libraries linked - executable is portable!"
            fi
        fi
        ;;
    *)
        echo "❌ Unknown command: $1"
        show_help
        exit 1
        ;;
esac

# Check result of the last command (compilation)
if [ $? -eq 0 ]; then
    echo "✅ Build successful."
else
    echo "❌ Build failed."
    exit 1
fi