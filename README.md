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

## 🪟 Windows Build

### Prerequisites to Install

1. **[Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)**
   - Select "Desktop development with C++" workload (~7GB)
   
2. **[CMake 3.20+](https://cmake.org/download/)**
   - Check "Add CMake to system PATH" during installation

3. **[Git for Windows](https://git-scm.com/download/win)** (includes `make` command)

### Build & Run

> 🚨 **CRITICAL**: You **MUST** use **"Developer Command Prompt for VS 2022"**, NOT PowerShell or CMD!
> 
> **How to open it:**
> 1. Press Windows key
> 2. Type: `Developer Command Prompt for VS 2022`
> 3. Click on it
> 
> If you can't find it, run this in PowerShell **ONCE**:
> ```powershell
> "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
> ```

**In Developer Command Prompt:**

```cmd
# Clone
git clone --recursive https://github.com/HexaTeam-By-Epitech/rtype.git
cd rtype

# Build
make debug

# Run
make run-server
make run-client

# Tests
make tests
```

### Troubleshooting

**Error: "Unable to find a valid Visual Studio instance"**
→ Make sure you installed "Desktop development with C++" workload in Visual Studio Build Tools.
→ Try opening "x64 Native Tools Command Prompt for VS 2022" instead.

💡 **First build takes 5-10 minutes** (vcpkg downloads dependencies automatically)

