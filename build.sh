#!/bin/bash

SRC="enniolimpiadi.c"
OUT="enniolimpiadi"
BUILD_DIR="build"

# -Werror?
CFLAGS="-fcolor-diagnostics -fansi-escape-codes -fsanitize=address -std=c99 -Wall -Wconversion -pedantic -g -O0"
PKG_FLAGS=$(pkg-config --cflags --libs raylib)

# Parse flags
PREPROCESS_ONLY=false
if [[ "$1" == "-p" ]]; then
    PREPROCESS_ONLY=true
    shift  # Remove -p from arguments
fi

# Show help message
show_help() {
    echo "Usage: $0 [-p] [enniolimpiadi|tests|clean]"
    echo
    echo "  -p               Run preprocessor only (outputs .i file)"
    echo
    echo "  enniolimpiadi    Compile $SRC -> $OUT"
    echo "  tests            Compile tests.c -> tests"
    echo "  clean            Remove the build directory"
    echo
    echo "Examples:"
    echo "  $0 tests         # Compile tests"
    echo "  $0 -p tests      # Preprocess tests only"
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
    enniolimpiadi)
        if [ "$PREPROCESS_ONLY" = true ]; then
            echo "Running preprocessor on $SRC -> $OUT.i"
            clang -E $SRC -o $BUILD_DIR/$OUT.i $PKG_FLAGS
        else
            echo "Compiling $SRC -> $OUT"
            clang $CFLAGS $SRC -o $BUILD_DIR/$OUT $PKG_FLAGS
        fi
        ;;
    tests)
        if [ "$PREPROCESS_ONLY" = true ]; then
            echo "Running preprocessor on tests.c -> tests.i"
            clang -E tests/tests.c -o $BUILD_DIR/tests.i $PKG_FLAGS
        else
            echo "Compiling tests.c -> tests"
            clang $CFLAGS tests/tests.c -o $BUILD_DIR/tests $PKG_FLAGS
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