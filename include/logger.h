/**
 * @file        logger.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-15>
 * @version     1.0.0
 *
 * @brief       Thread-safe logging utility with multiple verbosity levels.
 *
 * @details
 * The `Logger` class provides a global, thread-safe mechanism for structured
 * console logging in concurrent applications.
 *
 * Each message is prefixed with:
 * - A timestamp in ISO-like format (`YYYY-MM-DD HH:MM:SS`)
 * - The log level (`DBG`, `INFO`, `WARN`, `ERROR`)
 *
 * Internally:
 * - A static `std::mutex` guarantees that log messages are **not interleaved**
 *   when printed from multiple threads.
 * - A static `minLevel` acts as a **filter**: messages below the current
 *   level are ignored.
 *
 * @note
 * The Logger is purely static — no instances should be created.
 * It is designed to be safe for concurrent use across all threads.
 */

/*****************************************************************************/

#pragma once

/*****************************************************************************/

/* Standard libraries */
#include <mutex>
#include <string>

/*****************************************************************************/

/**
 * @class Logger
 * @brief Static thread-safe console logger with configurable severity filtering.
 *
 * @details
 * ### Features:
 * - Thread-safe: synchronized via internal static `std::mutex`.
 * - Supports 4 severity levels (`DBG`, `INFO`, `WARN`, `ERROR`).
 * - Global filter level configurable at runtime (`set_min_level()`).
 * - Each line includes timestamp + level + message.
 *
 * ### Usage example:
 * ```cpp
 * Logger::set_min_level(Logger::Level::INFO);
 * Logger::info("Worker started");
 * Logger::warn("Task queue is almost full");
 * Logger::error("Unhandled exception in worker");
 * ```
 */
class Logger
{
   public:
    /******************************************************************/
    /** @name Severity Levels */
    /**@{*/

    /**
     * @enum Level
     * @brief Log message severity levels.
     *
     * @details
     * Each level defines a minimum importance threshold:
     * - **DBG** → Most verbose (useful for tracing internal flow)
     * - **INFO** → Regular runtime information
     * - **WARN** → Indicates a potential problem
     * - **ERROR** → Indicates an actual failure
     */
    enum class Level
    {
        DBG   = 0, /**< Debug messages — detailed runtime info. */
        INFO  = 1, /**< Informational messages — normal operation. */
        WARN  = 2, /**< Warning messages — possible issue. */
        ERROR = 3  /**< Error messages — failure occurred. */
    };

    /**@}*/
    /******************************************************************/
    /** @name Configuration */
    /**@{*/

    /**
     * @brief Sets the minimum log level.
     *
     * @param lvl The minimum level to display.
     *
     * @details
     * GIVEN a desired log verbosity,
     * WHEN `set_min_level()` is called,
     * THEN only messages with `level >= lvl` will be printed to stdout.
     *
     * Example:
     * ```cpp
     * Logger::set_min_level(Logger::Level::WARN);
     * Logger::info("This will be ignored");
     * Logger::error("This will be shown");
     * ```
     */
    static void set_min_level(Level lvl);

    /**@}*/
    /******************************************************************/
    /** @name Logging Methods */
    /**@{*/

    /**
     * @brief Logs a debug message (`DBG` level).
     * @param msg The message to log.
     *
     * @details
     * Only printed if the current `minLevel` is `DBG` or lower.
     * Thread-safe.
     */
    static void debug(const std::string& msg);

    /**
     * @brief Logs an informational message (`INFO` level).
     * @param msg The message to log.
     *
     * @details
     * Commonly used to indicate normal runtime events.
     */
    static void info(const std::string& msg);

    /**
     * @brief Logs a warning message (`WARN` level).
     * @param msg The message to log.
     *
     * @details
     * Indicates a non-critical issue or unexpected state.
     */
    static void warn(const std::string& msg);

    /**
     * @brief Logs an error message (`ERROR` level).
     * @param msg The message to log.
     *
     * @details
     * Used for reporting critical failures or exceptions.
     * Typically displayed regardless of the `minLevel` setting,
     * unless the filter is set to `ERROR` itself.
     */
    static void error(const std::string& msg);

    /**
     * @brief Core logging function that prints a message with explicit severity.
     *
     * @param lvl Severity level (`DBG`, `INFO`, `WARN`, `ERROR`).
     * @param msg The message to print.
     *
     * @details
     * All wrapper methods (`debug()`, `info()`, etc.) call this internally.
     * This function checks the `minLevel`, then prints a timestamped message.
     *
     * @note
     * Thread-safe: a global `std::mutex` prevents output interleaving.
     */
    static void log(Level lvl, const std::string& msg);

    /**@}*/
    /******************************************************************/
    /** @name Internal Helpers */
    /**@{*/

   private:
    /**
     * @brief Returns the current timestamp formatted as "YYYY-MM-DD HH:MM:SS".
     *
     * @details
     * Uses `std::chrono::system_clock` and `std::put_time()` for formatting.
     *
     * @return String containing the formatted timestamp.
     *
     * Example:
     * ```
     * 2025-10-07 16:30:15
     * ```
     */
    static std::string timestamp();

    /**
     * @brief Converts a `Level` enum value into a short string label.
     *
     * @param lvl Severity level.
     * @return Corresponding label, e.g., `"INFO"`, `"ERROR"`, etc.
     */
    static const char* levelToString(Level lvl);

    /**@}*/
    /******************************************************************/
    /** @name Static Attributes */
    /**@{*/

   private:
    static std::mutex mtx;      /**< Global mutex to serialize console output. */
    static Level      minLevel; /**< Current minimum severity threshold. */

    /**@}*/
    /******************************************************************/
};
