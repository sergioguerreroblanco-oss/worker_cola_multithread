/**
 * @file        logger.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-15>
 * @version     1.0.0
 *
 * @brief       Thread-safe logging utility with levels.
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

std::mutex Logger::mtx;
Logger::Level Logger::minLevel = Logger::Level::INFO;

/*****************************************************************************/

/* Public Methods */

void Logger::set_min_level(Level lvl) {
    std::lock_guard<std::mutex> lock(mtx);
    minLevel = lvl;
}

void Logger::debug(const std::string& msg) { log(Level::DBG, msg); }

void Logger::info(const std::string& msg) { log(Level::INFO, msg); }

void Logger::warn(const std::string& msg) { log(Level::WARN, msg); }

void Logger::error(const std::string& msg) { log(Level::ERROR, msg); }

void Logger::log(Level lvl, const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx);
    if (static_cast<int>(lvl) < static_cast<int>(minLevel)) {
        return;
    }

    std::cout << "[" << timestamp() << "] " << "[" << levelToString(lvl) << "] " << msg
              << std::endl;
}

/*****************************************************************************/

/* Private Methods */

std::string Logger::timestamp() {
    using clock = std::chrono::system_clock;
    const auto now = clock::now();
    const std::time_t tt = clock::to_time_t(now);

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

const char* Logger::levelToString(Level lvl) {
    switch (lvl) {
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