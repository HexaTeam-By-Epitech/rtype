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

## 🐧 Linux (Debian/Ubuntu) Build

### Install Prerequisites

```bash
# Update package list
sudo apt update

# Install build tools and dependencies
sudo apt install -y build-essential cmake git curl zip unzip tar pkg-config \
                    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev \
                    libxi-dev libgl1-mesa-dev
```

### Build & Run

```bash
# Clone
git clone --recursive https://github.com/HexaTeam-By-Epitech/rtype.git
cd rtype

# Build Debug
make debug

# Run server
./build/linux-debug/server/r-type_server

# Run client (in another terminal)
./build/linux-debug/client/r-type_client

# Run tests
make tests
```

> 💡 **First build takes 5-10 minutes** (vcpkg downloads all dependencies automatically)

## 🪟 Windows Build

### Step 1: Install Prerequisites

#### A. Install Chocolatey (Package Manager)

Open **PowerShell as Administrator** and run:
```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

#### B. Install Tools via Chocolatey

In the same **PowerShell (Admin)**, run:
```powershell
choco install cmake git make -y
```

Close and reopen PowerShell after installation.

#### C. Install Visual Studio Build Tools (Manual)

> ⚠️ **This one is NOT via Chocolatey!**

1. Download [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)
2. Run the installer
3. Select **"Desktop development with C++"** workload
4. **Important**: Make sure the following are checked:
   - ✅ MSVC v143 - VS 2022 C++ x64/x86 build tools
   - ✅ Windows 11 SDK (or Windows 10 SDK)
   - ✅ C++ CMake tools for Windows
5. Click **"Install"** (~7GB download)

### Step 2: Build & Run

**Open any terminal (CMD or PowerShell):**

```cmd
# Clone
git clone --recursive https://github.com/HexaTeam-By-Epitech/rtype.git
cd rtype

# Build Debug
make debug

# Run server
cd build\windows-debug\server\Debug
r-type_server.exe

# Run client (in another terminal)
cd build\windows-debug\client\Debug
r-type_client.exe

# Or use make targets (from project root)
make run-server
make run-client

# Run tests
make tests
```

> 💡 **First build takes 5-10 minutes** (vcpkg downloads all dependencies automatically)
> 
> **Note**: With MSVC, executables are in `build\windows-debug\server\Debug\` and `build\windows-debug\client\Debug\`

### Troubleshooting

**Error: "Unable to find a valid Visual Studio instance"**
- Open **Visual Studio Installer** (search in Start menu)
- Click **"Modify"** next to "Build Tools 2022"
- Verify **"Desktop development with C++"** is fully checked with all sub-components
- Click **"Modify"** to complete installation

**Error: "cmake: command not found" or "make: command not found"**
- Close and reopen your terminal after installing Chocolatey packages
- Or restart your computer to refresh PATH

**Error: vcpkg fails to install packages**
- Make sure you have a stable internet connection
- First build takes time, be patient (~5-10 minutes)

