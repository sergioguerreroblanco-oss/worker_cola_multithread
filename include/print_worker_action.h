/**
 * @file        print_worker_action.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-15>
 * @version     1.0.0
 *
 * @brief       Concrete implementation of IWorkerAction that logs events.
 *
 * @details
 * This class provides a concrete implementation of the IWorkerAction<T>
 * interface. It uses the Logger utility to print information about the
 * Worker lifecycle and its interaction with the queue:
 *
 * - Logs retrieved data values with INFO level.
 * - Logs timeout events (empty queue) with WARN level.
 * - Logs shutdown events with ERROR level.
 * - Logs when a Worker finishes execution with INFO level.
 *
 * It is intended as the default strategy for observing the behavior
 * of Workers in this project.
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <chrono>
#include <string>

/* Project libraries */

#include "i_worker_action.h"
#include "logger.h"

/*****************************************************************************/

/**
 * @class PrintWorkerAction
 * @brief Concrete implementation of IWorkerAction that logs events.
 * @tparam T Type of data processed by the worker.
 *
 * This action prints log messages whenever a worker processes
 * data, detects an empty queue, shuts down, or stops.
 */
template <typename T>
class PrintWorkerAction : public IWorkerAction<T> {
    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @details Prints a message indicating that the "dato" was successfully
     *          retrieved from the buffer and shows the "dato".
     */
    void trabajo(const std::string& workerName, const T& dato) override {
        Logger::info("[" + workerName + "] Data processed: " + std::to_string(dato));
    }

    /**
     * @details Prints a message indicating that the timeout retrieving
     *          the "dato" from the buffer has passed and currently it is empty.
     */
    void colaVacia(const std::string& workerName,
                   const std::chrono::seconds waitting_time) override {
        Logger::warn("[" + workerName + "] Cola empty after timeout of " +
                     std::to_string(waitting_time.count()) + "s");
    }

    /**
     * @details Prints a message indicating that the worker finished its action.
     */
    void onStop(const std::string& workerName) override {
        Logger::info("[" + workerName + "] Finished.");
    }

    /******************************************************************/
};