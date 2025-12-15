# Welcome to RType !

[![Build Status](https://github.com/HexaTeam-By-Epitech/rtype/actions/workflows/build-and-tests.yml/badge.svg)](https://github.com/HexaTeam-By-Epitech/rtype/actions/workflows/build-and-tests.yml)
[![Windows Build](https://github.com/HexaTeam-By-Epitech/rtype/actions/workflows/windows-build.yml/badge.svg)](https://github.com/HexaTeam-By-Epitech/rtype/actions/workflows/windows-build.yml)
[![Documentation](https://img.shields.io/badge/docs-online-brightgreen)](https://hexateam-by-epitech.github.io/rtype/html)
[![Coverage](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/mael-app/b7449f68c0857ebff4c2c848415dc9f8/raw/rtype-coverage.json?v=1)](https://github.com/HexaTeam-By-Epitech/rtype/actions)

## Multi-Platform Support

This project compiles and runs on:
- 🐧 **Linux** (GCC 7+ / Clang 6+)
- 🍎 **macOS** (Apple Clang / ARM64 & x86_64)
- 🪟 **Windows** (MSVC 2019+)

## 🪟 Quick Start - Windows Build

### Prerequisites
- [Visual Studio 2019+](https://visualstudio.microsoft.com/) with C++ Desktop Development workload
- [CMake 3.20+](https://cmake.org/download/)
- [Make for Windows](https://gnuwin32.sourceforge.net/packages/make.htm) or use Git Bash

### Build & Run

```cmd
# Clone the project
git clone --recursive https://github.com/HexaTeam-By-Epitech/rtype.git
cd rtype

# Build Debug (downloads dependencies automatically)
make debug

# Or build Release
make release

# Run server
make run-server

# Run client  
make run-client

# Run tests
make tests
```

💡 **Tip**: The first build will take longer as vcpkg downloads and compiles dependencies (raylib, enet, capnproto, gtest).

### Available Make Targets
- `make debug` - Build in Debug mode
- `make release` - Build in Release mode
- `make server` - Build only the server
- `make client` - Build only the client
- `make tests` - Build and run all tests
- `make clean` - Clean build artifacts
- `make fclean` - Full clean (including vcpkg cache)

