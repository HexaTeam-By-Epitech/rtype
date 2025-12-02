#!/usr/bin/env bash

# Make the script stricter
set -euo pipefail

# =====================
# Configuration
# =====================
BUILD_DIR="build"
# Allow overriding compilers via environment variables if needed
: "${CC:=gcc}"
: "${CXX:=g++}"

# =====================
# Utility functions
# =====================
log() {
  printf '[build] %s\n' "$*"
}

fatal() {
  printf '[build][ERROR] %s\n' "$*" >&2
  exit 1
}

# =====================
# Prerequisites
# =====================
command -v cmake >/dev/null 2>&1 || fatal "cmake not found in PATH"
command -v "$CC" >/dev/null 2>&1 || fatal "C compiler not found: $CC"
command -v "$CXX" >/dev/null 2>&1 || fatal "C++ compiler not found: $CXX"

# =====================
# Build configuration
# =====================
log "Creating build directory: $BUILD_DIR"
mkdir -p "$BUILD_DIR" || fatal "Failed to create directory $BUILD_DIR"
cd "$BUILD_DIR" || fatal "Failed to enter directory $BUILD_DIR"

log "Configuring CMake (CC=$CC, CXX=$CXX)"
cmake -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" .. || fatal "CMake configuration failed"

# =====================
# Build
# =====================
log "Building project"
cmake --build . || fatal "Build failed"

log "Build completed successfully."
