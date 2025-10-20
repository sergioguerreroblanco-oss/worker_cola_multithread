/**
 * @file        i_worker_action.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-15>
 * @version     1.0.0
 *
 * @brief       Abstract interface for Worker actions.
 *
 * @details
 * Defines the abstract contract that a Worker uses to delegate behavior
 * when consuming data from a queue, handling timeouts, reacting to
 * shutdown, and notifying lifecycle end (stop).
 *
 * This interface is templated to support any element type (Cola<T>).
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <chrono>

/*****************************************************************************/

/**
 * @class IWorkerAction
 * @brief Abstract interface for worker actions.
 * @tparam T Type of data processed by the worker.
 *
 * Defines the contract that worker actions must implement.
 * This allows `Worker<T>` to delegate behavior without
 * depending on concrete implementations (strategy pattern).
 */
template <typename T>
class IWorkerAction {
    /******************************************************************/

    /* Public Methods */

   public:
    virtual ~IWorkerAction() = default;

    /**
     * @brief Action executed when data is successfully retrieved.
     * @param workerName Name of the worker invoking the callback.
     * @param dato Data retrieved from the queue.
     */
    virtual void trabajo(const std::string& workerName, const T& dato) = 0;

    /**
     * @brief Action executed when the queue is empty after timeout.
     * @param workerName Name of the worker invoking the callback.
     * @param timeout Time waited before considering Cola empty.
     */
    virtual void colaVacia(const std::string& workerName, const std::chrono::seconds timeout) = 0;

    /**
     * @brief Action executed when the queue is shut down.
     * @param workerName Name of the worker invoking the callback.
     */
    virtual void onStop(const std::string& workerName) = 0;

    /******************************************************************/
};