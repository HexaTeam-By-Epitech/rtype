/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Logger - Centralized logging system with timestamps and file information
*/

#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>

namespace logger {

    /**
     * @brief Log severity levels
     */
    enum class Level {
        DEBUG,    // Detailed information for debugging
        INFO,     // General informational messages
        WARNING,  // Warning messages for potential issues
        ERROR,    // Error messages for failures
        CRITICAL  // Critical errors that may cause termination
    };

    /**
     * @brief ANSI color codes for terminal output
     */
    namespace Colors {
        constexpr const char *RESET = "\033[0m";
        constexpr const char *LOG_GRAY = "\033[90m";
        constexpr const char *LOG_GREEN = "\033[32m";
        constexpr const char *LOG_YELLOW = "\033[33m";
        constexpr const char *LOG_RED = "\033[31m";
        constexpr const char *BOLD_RED = "\033[1;31m";
        constexpr const char *CYAN = "\033[36m";
        constexpr const char *BOLD_WHITE = "\033[1;37m";
    }  // namespace Colors

    /**
     * @class Logger
     * @brief Thread-safe logging system with timestamps and source location
     */
    class Logger {
       private:
        static inline std::mutex _mutex;
        static inline Level _minLevel = Level::DEBUG;
        static inline bool _enableColors = true;

        /**
         * @brief Extract basename from file path
         */
        static std::string getBasename(const char *filePath) {
            std::string path(filePath);
            size_t pos = path.find_last_of("/\\");
            return (pos == std::string::npos) ? path : path.substr(pos + 1);
        }

        /**
         * @brief Get current timestamp with milliseconds
         */
        static std::string getTimestamp() {
            auto now = std::chrono::system_clock::now();
            auto nowTime = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            std::ostringstream oss;
            oss << std::put_time(std::localtime(&nowTime), "%H:%M:%S");
            oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return oss.str();
        }

        /**
         * @brief Get color for log level
         */
        static const char *getLevelColor(Level level) {
            if (!_enableColors)
                return "";

            switch (level) {
                case Level::DEBUG:
                    return Colors::LOG_GRAY;
                case Level::INFO:
                    return Colors::LOG_GREEN;
                case Level::WARNING:
                    return Colors::LOG_YELLOW;
                case Level::ERROR:
                    return Colors::LOG_RED;
                case Level::CRITICAL:
                    return Colors::BOLD_RED;
                default:
                    return Colors::RESET;
            }
        }

        /**
         * @brief Get string representation of log level
         */
        static const char *getLevelString(Level level) {
            switch (level) {
                case Level::DEBUG:
                    return "DEBUG";
                case Level::INFO:
                    return "INFO ";
                case Level::WARNING:
                    return "WARN ";
                case Level::ERROR:
                    return "ERROR";
                case Level::CRITICAL:
                    return "CRIT ";
                default:
                    return "UNKNOWN";
            }
        }

       public:
        /**
         * @brief Set minimum log level (messages below this level are ignored)
         */
        static void setLevel(Level level) { _minLevel = level; }

        /**
         * @brief Enable or disable colored output
         */
        static void setColors(bool enable) { _enableColors = enable; }

        /**
         * @brief Log a message with source location
         */
        static void log(Level level, const char *file, int line, const std::string &message) {
            if (level < _minLevel)
                return;

            std::lock_guard<std::mutex> lock(_mutex);

            const char *levelColor = getLevelColor(level);
            const char *resetColor = _enableColors ? Colors::RESET : "";
            const char *fileColor = _enableColors ? Colors::CYAN : "";
            const char *timeColor = _enableColors ? Colors::BOLD_WHITE : "";

            std::ostringstream oss;
            oss << timeColor << "[" << getTimestamp() << "]" << resetColor << " ";
            oss << levelColor << "[" << getLevelString(level) << "]" << resetColor << " ";
            oss << fileColor << "[" << getBasename(file) << ":" << line << "]" << resetColor << " ";
            oss << message;

            std::cout << oss.str() << std::endl;
        }

        /**
         * @brief Log a message with formatted arguments
         */
        template <typename... Args>
        static void logf(Level level, const char *file, int line, Args &&...args) {
            if (level < _minLevel)
                return;

            std::ostringstream oss;
            (oss << ... << std::forward<Args>(args));
            log(level, file, line, oss.str());
        }
    };

}  // namespace logger

// Convenience macros for logging
#define LOG_DEBUG(...) logger::Logger::logf(logger::Level::DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) logger::Logger::logf(logger::Level::INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) logger::Logger::logf(logger::Level::WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) logger::Logger::logf(logger::Level::ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_CRITICAL(...) logger::Logger::logf(logger::Level::CRITICAL, __FILE__, __LINE__, __VA_ARGS__)

// Short aliases
#define LOG_D(...) LOG_DEBUG(__VA_ARGS__)
#define LOG_I(...) LOG_INFO(__VA_ARGS__)
#define LOG_W(...) LOG_WARNING(__VA_ARGS__)
#define LOG_E(...) LOG_ERROR(__VA_ARGS__)
