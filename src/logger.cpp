/**
 * @file        logger.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-15>
 * @version     1.0.0
 *
 * @brief       Implementation of the thread-safe Logger class.
 *
 * @details
 * This file implements the static methods of the Logger utility,
 * including level filtering, formatted output, and time-stamping.
 *
 * The Logger ensures that:
 * - Output from multiple threads is synchronized via a shared `std::mutex`.
 * - Messages are filtered according to the current `minLevel`.
 * - Each log line includes timestamp + severity + message.
 */

/*****************************************************************************/

/* Standard libraries */
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

/* Project libraries */
#include "logger.h"

/*****************************************************************************/

/* Static member initialization */

std::mutex    Logger::mtx;
Logger::Level Logger::minLevel = Logger::Level::INFO;

/*****************************************************************************/

/* Public Methods */

/**
 * @brief Sets the minimum severity level to be printed.
 *
 * @param lvl The minimum log level; messages below this level are ignored.
 *
 * @details
 * GIVEN a running application,
 * WHEN `set_min_level()` is called,
 * THEN subsequent calls to `log()` will only print messages with
 * `level >= lvl`.
 *
 * Thread-safe: uses a lock to protect shared configuration.
 */
void Logger::set_min_level(Level lvl)
{
    std::lock_guard<std::mutex> lock(mtx);
    minLevel = lvl;
}

/**
 * @brief Logs a debug message (`DBG` level).
 * @param msg Message to print.
 *
 * @details
 * Wrapper for `log(Level::DBG, msg)`.
 * Typically used for verbose diagnostic information.
 */
void Logger::debug(const std::string& msg)
{
    log(Level::DBG, msg);
}

/**
 * @brief Logs an informational message (`INFO` level).
 * @param msg Message to print.
 *
 * @details
 * Wrapper for `log(Level::INFO, msg)`.
 * Used for high-level runtime information.
 */
void Logger::info(const std::string& msg)
{
    log(Level::INFO, msg);
}

/**
 * @brief Logs a warning message (`WARN` level).
 * @param msg Message to print.
 *
 * @details
 * Wrapper for `log(Level::WARN, msg)`.
 * Used for recoverable or unexpected events.
 */
void Logger::warn(const std::string& msg)
{
    log(Level::WARN, msg);
}

/**
 * @brief Logs an error message (`ERROR` level).
 * @param msg Message to print.
 *
 * @details
 * Wrapper for `log(Level::ERROR, msg)`.
 * Indicates a critical issue or failure.
 */
void Logger::error(const std::string& msg)
{
    log(Level::ERROR, msg);
}

/**
 * @brief Core method that handles message formatting and output.
 *
 * @param lvl Severity level of the message.
 * @param msg Message text to print.
 *
 * @details
 * GIVEN a message and a severity level,
 * WHEN `log()` is invoked,
 * THEN:
 *  - If `lvl < minLevel`, the message is ignored.
 *  - Otherwise, the message is printed to `std::cout` as:
 *    ```
 *    [YYYY-MM-DD HH:MM:SS] [LEVEL] message
 *    ```
 *
 * @note
 * - Thread-safe: all access to `std::cout` is serialized with a mutex.
 * - Uses `std::endl` to flush output immediately.
 */
void Logger::log(Level lvl, const std::string& msg)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (static_cast<int>(lvl) < static_cast<int>(minLevel))
    {
        return;
    }

    std::cout << "[" << timestamp() << "] "
              << "[" << levelToString(lvl) << "] " << msg << std::endl;
}

/*****************************************************************************/

/* Private Methods */

/**
 * @brief Returns the current timestamp in "YYYY-MM-DD HH:MM:SS" format.
 *
 * @details
 * Internally uses `std::chrono::system_clock` and converts to `std::tm`
 * using the thread-safe functions:
 * - `localtime_s()` on Windows.
 * - `localtime_r()` on POSIX systems.
 *
 * Example:
 * ```
 * 2025-10-07 17:26:45
 * ```
 *
 * @return Formatted timestamp string.
 */
std::string Logger::timestamp()
{
    using clock           = std::chrono::system_clock;
    const auto        now = clock::now();
    const std::time_t tt  = clock::to_time_t(now);

    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

/**
 * @brief Converts a `Logger::Level` to its string representation.
 *
 * @param lvl Severity level to convert.
 * @return The corresponding string constant.
 *
 * @details
 * Example output:
 * ```
 * levelToString(Logger::Level::WARN) -> "WARN"
 * ```
 */
const char* Logger::levelToString(Level lvl)
{
    switch (lvl)
    {
        case Level::DBG:
            return "DEBUG";
        case Level::INFO:
            return "INFO";
        case Level::WARN:
            return "WARN";
        case Level::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

/*****************************************************************************/
