# 🚀 R-Type | HexaTeam

> **A high-performance, multiplayer space shooter engine based on a custom ECS architecture, developed for the Epitech
curriculum.**

---

## 📊 Project Status & CI/CD

[![Build Status](https://github.com/HexaTeam-By-Epitech/rtype/actions/workflows/build-and-tests.yml/badge.svg)](https://github.com/HexaTeam-By-Epitech/rtype/actions/workflows/build-and-tests.yml)
[![Windows Build](https://github.com/HexaTeam-By-Epitech/rtype/actions/workflows/windows-build.yml/badge.svg)](https://github.com/HexaTeam-By-Epitech/rtype/actions/workflows/windows-build.yml)
[![Documentation](https://img.shields.io/badge/docs-online-brightgreen)](https://hexateam-by-epitech.github.io/rtype/html)
[![Coverage](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/mael-app/b7449f68c0857ebff4c2c848415dc9f8/raw/rtype-coverage.json?v=1)](https://github.com/HexaTeam-By-Epitech/rtype/actions)

[![SonarQube Cloud](https://sonarcloud.io/images/project_badges/sonarcloud-light.svg)](https://sonarcloud.io/summary/new_code?id=HexaTeam-By-Epitech_rtype)

### Multi-Platform Support

This project compiles and runs on:

- ![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black) **Linux** (GCC 7+ / Clang
  6+)
- ![macOS](https://img.shields.io/badge/macOS-000000?style=flat&logo=apple&logoColor=white) **macOS** (Apple Clang /
  ARM64 & x86_64)
- ![Windows](https://img.shields.io/badge/Windows-0078D6?style=flat&logo=windows&logoColor=white) **Windows** (MSVC
  2019+)

---

## 🛠 Core Technical Features

* **Custom ECS Engine**: Optimized Entity-Component-System for high-performance game logic.
* **Asynchronous Networking**: Robust Client/Server communication powered by **Asio** (UDP protocol).
* **Cross-Platform Core**: Native support for Linux, Windows, and macOS with a unified build system.
* **Asset Management**: Centralized system for dynamic loading of textures, fonts, and audio.
* **Automated Pipeline**: Full CI/CD integration for testing and documentation deployment.

---

## ⚙️ Installation & Build

### Linux (Debian/Ubuntu)

**1. Install System Dependencies**

```bash
sudo apt update && sudo apt install -y build-essential cmake git curl zip unzip tar pkg-config \
    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev

```

**2. Clone and Compile**

```bash
git clone --recursive https://github.com/HexaTeam-By-Epitech/rtype.git
cd rtype
make debug

```

---

### Windows

**1. Install Prerequisites**

* **Chocolatey**: Open PowerShell (Admin) and run the [official install script](https://chocolatey.org/install). Then
  install tools:

```powershell
choco install cmake git make -y

```

* **Visual Studio 2022**: Install **"Desktop development with C++"** via the VS Installer. Ensure the following are
  checked:
* MSVC v143 build tools
* Windows 10/11 SDK
* C++ CMake tools for Windows

**2. Clone and Compile**

```powershell
git clone --recursive https://github.com/HexaTeam-By-Epitech/rtype.git
cd rtype
make debug

```

> 💡 **Note**: The initial build may take 5-10 minutes as `vcpkg` automatically handles dependency resolution.

---

## 🎮 How to Play

### Running the Project

The project consists of two main binaries: the **Server** (logic) and the **Client** (graphics/input).

| Component  | Linux Command                              | Windows Path                                           |
|------------|--------------------------------------------|--------------------------------------------------------|
| **Server** | `./build/linux-debug/server/r-type_server` | `.\build\windows-debug\server\Debug\r-type_server.exe` |
| **Client** | `./build/linux-debug/client/r-type_client` | `.\build\windows-debug\client\Debug\r-type_client.exe` |

*Alternatively, use the root shortcuts:* `make run-server` or `make run-client`.

---

## 🧪 Testing & Quality Assurance

We use a comprehensive suite of unit tests to ensure engine stability:

```bash
make tests

```

---

## 🆘 Troubleshooting

| Issue                            | Resolution                                                                                          |
|----------------------------------|-----------------------------------------------------------------------------------------------------|
| **vcpkg Installation Failure**   | Ensure a stable internet connection. The first build downloads several hundred MBs of dependencies. |
| **CMake: Command not found**     | Restart your terminal after installing CMake to refresh the Environment Variables (PATH).           |
| **Visual Studio Instance Error** | Verify that "Desktop development with C++" is fully installed in the Visual Studio Installer.       |

---

## 📄 Documentation

📚 Online API reference  
👉 https://hexateam-by-epitech.github.io/rtype/html

Includes:

- Engine design overview
- Network model
- Build & setup guide

---

## 👥 Contributors

A huge thanks to the **HexaTeam** developers:

<a href="https://github.com/mael-app">
<img src="https://github.com/mael-app.png" width="60px" alt="mael-app" style="border-radius: 50%;" />
</a>
<a href="https://github.com/IamSwan">
<img src="https://github.com/IamSwan.png" width="60px" alt="IamSwan" style="border-radius: 50%;" />
</a>
<a href="https://github.com/SamuelBleau">
<img src="https://github.com/SamuelBleau.png" width="60px" alt="SamuelBleau" style="border-radius: 50%;" />
</a>
<a href="https://github.com/Watersrc-stack">
<img src="https://github.com/Watersrc-stack.png" width="60px" alt="Water-s0urce" style="border-radius: 50%;" />
</a>
<a href="https://github.com/Hugo-Fabresse">
<img src="https://github.com/Hugo-Fabresse.png" width="60px" alt="Hugo-Fabresse" style="border-radius: 50%;" />
</a>

---

## License 
This project is licensed under the GPL-3.0 License - see the [LICENSE](LICENSE) file for details.
