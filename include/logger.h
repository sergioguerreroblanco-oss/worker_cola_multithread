/**
 * @file        logger.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-15>
 * @version     1.0.0
 *
 * @brief       Thread-safe logging utility with levels.
 *
 * @details
 * The Logger class provides a global, thread-safe mechanism to print
 * messages to the standard output stream. Messages are categorized by
 * severity levels (INFO, WARN, ERROR), and a configurable minimum level
 * filter determines which messages are displayed.
 *
 * Internally, the Logger uses a static mutex to synchronize concurrent
 * writes from multiple threads, ensuring that log messages are not
 * interleaved. It also attaches a timestamp and the severity label to
 * each printed message, providing clear context for debugging and
 * monitoring concurrent applications.
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <mutex>
#include <string>

/*****************************************************************************/

/**
 * @class Logger
 * @brief A thread-safe static logger utility with configurable severity levels.
 */
class Logger {
    /******************************************************************/

    /* Public Data Types */

   public:
    /**
     * @enum Level
     * @brief Severity levels for log messages.
     */
    enum class Level {
        DBG = 0,  /**< Debug messages, most verbose. */
        INFO = 1, /**< Informational messages about normal operation. */
        WARN = 2, /**< Warning messages indicating potential issues. */
        ERROR = 3 /**< Error messages indicating failures. */
    };

    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Set the minimum log level.
     * @param lvl The minimum level; messages below this level are ignored.
     */
    static void set_min_level(Level lvl);

    /**
     * @brief Log a debug message.
     * @param msg The message to log.
     */
    static void debug(const std::string& msg);

    /**
     * @brief Log an informational message.
     * @param msg The message to log.
     */
    static void info(const std::string& msg);

    /**
     * @brief Log a warning message.
     * @param msg The message to log.
     */
    static void warn(const std::string& msg);

    /**
     * @brief Log an error message.
     * @param msg The message to log.
     */
    static void error(const std::string& msg);

    /**
     * @brief Log a message with the specified severity level.
     * @param lvl Severity level of the message.
     * @param msg The message to log.
     */
    static void log(Level lvl, const std::string& msg);

    /******************************************************************/

    /* Private Methods */

   private:
    /**
     * @brief Get the current timestamp as a string.
     * @return A formatted timestamp ("YYYY-MM-DD HH:MM:SS").
     */
    static std::string timestamp();

    /**
     * @brief Convert a log level to its string representation.
     * @param lvl The severity level.
     * @return The corresponding string (e.g., "INFO").
     */
    static const char* levelToString(Level);

    /******************************************************************/

    /* Private Attributes */

   private:
    static std::mutex mtx; /**< Mutex for synchronizing log output. */
    static Level minLevel; /**< Minimum level required to print messages. */

    /******************************************************************/
};