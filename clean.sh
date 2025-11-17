#!/usr/bin/env bash

# Make the script stricter
set -euo pipefail

# =====================
# Configuration
# =====================
BUILD_DIR="${BUILD_DIR:-build}"

# =====================
# Utility functions
# =====================
log() {
  printf '[clean] %s\n' "$*"
}

fatal() {
  printf '[clean][ERROR] %s\n' "$*" >&2
  exit 1
}

# =====================
# Safety checks
# =====================
if [[ -z "$BUILD_DIR" || "$BUILD_DIR" == "/" ]]; then
  fatal "Refusing to remove an empty or root directory (BUILD_DIR='$BUILD_DIR')"
fi

if [[ ! -d "$BUILD_DIR" ]]; then
  log "Nothing to clean: directory '$BUILD_DIR' does not exist."
  exit 0
fi

# =====================
# Clean
# =====================
log "Removing build directory: $BUILD_DIR"
rm -rf -- "$BUILD_DIR" || fatal "Failed to remove directory '$BUILD_DIR'"

log "Clean completed successfully."
