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
- [Ninja](https://github.com/ninja-build/ninja/releases) (optional, for faster builds)

### Build Steps

1. **Clone the repository**
```cmd
git clone --recursive https://github.com/HexaTeam-By-Epitech/rtype.git
cd rtype
```

2. **Bootstrap vcpkg** (first time only)
```cmd
cd vcpkg
bootstrap-vcpkg.bat
cd ..
```

3. **Configure CMake** (choose Debug or Release)
```cmd
:: Debug build
cmake --preset windows-debug

:: OR Release build
cmake --preset windows-release
```

4. **Build the project**
```cmd
:: Debug build
cmake --build build/windows-debug --config Debug --parallel

:: OR Release build
cmake --build build/windows-release --config Release --parallel
```

5. **Run the executables**
```cmd
:: Run server (Debug)
build\windows-debug\server\r-type_server.exe

:: Run client (Debug)
build\windows-debug\client\r-type_client.exe
```

### Run Tests (Debug only)
```cmd
cd build\windows-debug
ctest --output-on-failure -C Debug
```

💡 **Tip**: The first build will take 5-10 minutes as vcpkg downloads and compiles all dependencies (raylib, enet, capnproto, gtest). Subsequent builds are much faster!

