# Logger System

A thread-safe, centralized logging system with timestamps, file locations, and color-coded output.

## Features

- **Timestamps**: Millisecond precision (HH:MM:SS.mmm)
- **Source Location**: Automatic filename and line number
- **Log Levels**: DEBUG, INFO, WARNING, ERROR, CRITICAL
- **Thread-Safe**: Mutex-protected output
- **Colored Output**: ANSI color codes for better readability
- **Header-Only**: Easy integration, no linking required

## Usage

### Basic Logging

```cpp
#include "common/Logger/Logger.hpp"

// Simple messages
LOG_INFO("Server started successfully");
LOG_ERROR("Connection failed");

// With variables (variadic templates)
LOG_INFO("Player ", playerName, " joined with ID: ", playerId);
LOG_DEBUG("Position: (", x, ", ", y, ")");
LOG_WARNING("High latency detected: ", latency, "ms");
```

### Available Macros

- `LOG_DEBUG(...)` - Detailed debug information
- `LOG_INFO(...)` - General informational messages
- `LOG_WARNING(...)` - Warning messages
- `LOG_ERROR(...)` - Error messages
- `LOG_CRITICAL(...)` - Critical failures

**Short aliases:**
- `LOG_D(...)` - Debug
- `LOG_I(...)` - Info
- `LOG_W(...)` - Warning
- `LOG_E(...)` - Error

### Configuration

```cpp
// Set minimum log level (suppress lower levels)
logger::Logger::setLevel(logger::Level::INFO);  // Hide DEBUG messages

// Disable colors (for non-terminal output)
logger::Logger::setColors(false);
```

## Output Format

```
[HH:MM:SS.mmm] [LEVEL] [filename.cpp:line] message
```

**Example:**
```
[14:23:45.123] [INFO ] [Server.cpp:33] Initializing R-Type server...
[14:23:45.234] [ERROR] [GameLogic.cpp:105] Player 1000 already exists!
[14:23:46.001] [DEBUG] [GameLogic.cpp:81] Tick 60 | Players: 2
```

## Color Scheme

- **DEBUG**: Gray
- **INFO**: Green
- **WARNING**: Yellow
- **ERROR**: Red
- **CRITICAL**: Bold Red
- **Timestamp**: Bold White
- **File**: Cyan

## Integration

Simply include the header in any file:

```cpp
#include "../../common/Logger/Logger.hpp"
```

No CMake configuration or linking required.
