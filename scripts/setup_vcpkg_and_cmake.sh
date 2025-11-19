#!/usr/bin/env bash
# Script to bootstrap vcpkg, install manifest deps for a chosen triplet, and configure+build CMake.
# Usage: ./scripts/setup_vcpkg_and_cmake.sh [--triplet TRIPLET] [--vcpkg-root PATH] [--no-bootstrap] [--no-install] [--no-build]

set -euo pipefail

# Defaults
SCRIPT_DIR="$(cd "$(dirname "${0}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR/.."
DEFAULT_VCPKG_ROOT="$HOME/vcpkg"
TRIPLET=""
VCPKG_ROOT=""
DO_BOOTSTRAP=1
DO_INSTALL=1
DO_BUILD=1

print_help() {
  cat <<EOF
Usage: $0 [options]
Options:
  -t, --triplet TRIPLET    Triplet to use (e.g. arm64-osx, x64-osx, x64-linux, x64-windows)
  -r, --vcpkg-root PATH     Path to vcpkg root (default: $DEFAULT_VCPKG_ROOT)
      --no-bootstrap       Skip bootstrapping vcpkg (assume it is already bootstrapped)
      --no-install         Skip installing manifest dependencies
      --no-build           Skip configuring/building the project
  -h, --help               Show this help and exit

This script will:
  - clone vcpkg to the vcpkg-root if missing
  - run the bootstrap script appropriate for the host
  - install packages declared in vcpkg.json in project root for the given triplet (manifest mode)
  - run CMake configure/build with vcpkg toolchain

Examples:
  # Automatic triplet detection for macOS (Apple Silicon) and build
  $0 --triplet arm64-osx

  # Specify vcpkg root and only configure (no install/build)
  $0 -t x64-linux -r ~/vcpkg --no-build --no-install
EOF
}

# Simple arg parse
while [[ $# -gt 0 ]]; do
  case "$1" in
    -t|--triplet)
      TRIPLET="$2"; shift 2;;
    -r|--vcpkg-root)
      VCPKG_ROOT="$2"; shift 2;;
    --no-bootstrap)
      DO_BOOTSTRAP=0; shift;;
    --no-install)
      DO_INSTALL=0; shift;;
    --no-build)
      DO_BUILD=0; shift;;
    -h|--help)
      print_help; exit 0;;
    *)
      echo "Unknown option: $1"; print_help; exit 1;;
  esac
done

# Resolve defaults
if [[ -z "$VCPKG_ROOT" ]]; then
  if [[ -n "${VCPKG_ROOT:-}" ]]; then
    VCPKG_ROOT="$VCPKG_ROOT"
  else
    VCPKG_ROOT="$DEFAULT_VCPKG_ROOT"
  fi
fi

# Triplet detection if not provided
if [[ -z "$TRIPLET" ]]; then
  UNAME_OUT="$(uname -s)"
  case "$UNAME_OUT" in
    Darwin)
      ARCH="$(uname -m)"
      if [[ "$ARCH" == "arm64" ]]; then
        TRIPLET="arm64-osx"
      else
        TRIPLET="x64-osx"
      fi
      ;;
    Linux)
      TRIPLET="x64-linux";
      ;;
    MINGW*|MSYS*|CYGWIN*|Windows_NT)
      TRIPLET="x64-windows";
      ;;
    *)
      echo "Unsupported OS: $UNAME_OUT"; exit 1;;
  esac
  echo "Auto-detected triplet: $TRIPLET"
fi

# Supported triplets hint
SUPPORTED=("arm64-osx" "x64-osx" "x64-linux" "x64-windows")
if [[ ! " ${SUPPORTED[@]} " =~ " ${TRIPLET} " ]]; then
  echo "Warning: Triplet '$TRIPLET' is not in the common-supplied list: ${SUPPORTED[*]}"
fi

echo "Using vcpkg root: $VCPKG_ROOT"
echo "Using triplet: $TRIPLET"

# Clone vcpkg if missing
if [[ ! -x "$VCPKG_ROOT/vcpkg" ]]; then
  if [[ -d "$VCPKG_ROOT" ]]; then
    echo "vcpkg dir exists but vcpkg executable not found; attempting bootstrap if allowed..."
  else
    echo "Cloning vcpkg into $VCPKG_ROOT"
    git clone https://github.com/microsoft/vcpkg.git "$VCPKG_ROOT"
  fi
fi

# Bootstrap according to host
if [[ $DO_BOOTSTRAP -eq 1 ]]; then
  echo "Bootstrapping vcpkg in $VCPKG_ROOT"
  if [[ ! -f "$VCPKG_ROOT/bootstrap-vcpkg.sh" && ! -f "$VCPKG_ROOT/bootstrap-vcpkg.bat" ]]; then
    echo "vcpkg sources missing in $VCPKG_ROOT"; exit 1
  fi

  UNAME_OUT="$(uname -s)"
  case "$UNAME_OUT" in
    Darwin|Linux)
      (cd "$VCPKG_ROOT" && ./bootstrap-vcpkg.sh)
      ;;
    MINGW*|MSYS*|CYGWIN*|Windows_NT)
      echo "Running Windows bootstrap. If you run this script from WSL/MSYS that may not work; run the batch file from Windows cmd/powershell:"
      echo "  $VCPKG_ROOT\\bootstrap-vcpkg.bat"
      (cd "$VCPKG_ROOT" && cmd.exe /c bootstrap-vcpkg.bat)
      ;;
    *)
      echo "Cannot determine bootstrap method for OS: $UNAME_OUT"; exit 1;;
  esac
else
  echo "Skipping bootstrap (--no-bootstrap)"
fi

# Install manifest dependencies (run from project root so vcpkg picks up vcpkg.json)
if [[ $DO_INSTALL -eq 1 ]]; then
  if [[ ! -f "$PROJECT_ROOT/vcpkg.json" ]]; then
    echo "No vcpkg.json found in project root ($PROJECT_ROOT). Skipping manifest install."
  else
    echo "Installing manifest dependencies for triplet $TRIPLET"
    (cd "$PROJECT_ROOT" && "$VCPKG_ROOT/vcpkg" install --triplet "$TRIPLET")
  fi
else
  echo "Skipping vcpkg install (--no-install)"
fi

# Configure and build using CMake with the vcpkg toolchain
if [[ $DO_BUILD -eq 1 ]]; then
  TOOLCHAIN="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
  if [[ ! -f "$TOOLCHAIN" ]]; then
    echo "Toolchain file not found at $TOOLCHAIN"; exit 1
  fi

  echo "Configuring project with CMake (build directory: $PROJECT_ROOT/build)"
  cmake -S "$PROJECT_ROOT" -B "$PROJECT_ROOT/build" -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" -DVCPKG_TARGET_TRIPLET="$TRIPLET" -DCMAKE_BUILD_TYPE=Debug
  echo "Building project"
  cmake --build "$PROJECT_ROOT/build" -- -j$(nproc 2>/dev/null || echo 4)
  echo "Running tests with ctest"
  ctest --test-dir "$PROJECT_ROOT/build" -T test --output-on-failure
else
  echo "Skipping CMake configure/build (--no-build)"
fi

echo "Done. If you want to reuse these settings in CLion, add the following CMake options:"
cat <<EOF
  -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
  -DVCPKG_TARGET_TRIPLET=$TRIPLET
EOF
