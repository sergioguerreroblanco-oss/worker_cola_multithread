/**
 * @file        worker_pool.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief       
 *
 * @details
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

/* Project libraries */

#include "thread_safe_queue.h"

/*****************************************************************************/

/**
 * @class Worker
 * @brief Worker thread that consumes data from a queue.
 * @tparam T Type of data consumed from the queue.
 *
 * Each Worker runs in its own thread, repeatedly calling `pop()` on the queue
 * and delegating the retrieved data to the associated IWorkerAction.
 * It supports graceful shutdown or immediate stop.
 */
class WorkerPool {
    /******************************************************************/

    /* Private Constants */

   private:
    /**
     * @brief Default name of the worker used during the construction of it.
     */
    static constexpr const char* DEFAULT_WORKER_NAME = "Worker ";

    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Construct
     * @param 
     * @param 
     */
    explicit WorkerPool(ThreadSafeQueue<std::function<void()>>& queue);

    /**
     * @brief Destruct a Worker object.
     */
    ~WorkerPool();

    /**
     * @brief Disable copy constructor.
     *        A WorkerPool cannot be copied because it manages a std::thread,
     *        which is non-copyable. Copying would imply two Workers
     *        owning the same thread, which is undefined.
     */
    WorkerPool(const WorkerPool&) = delete;

    /**
     * @brief Disable copy assignment operator.
     *        A WorkerPool cannot be copy-assigned because std::thread
     *        cannot be duplicated or reassigned between different owners.
     */
    WorkerPool& operator=(const WorkerPool&) = delete;

    /**
     * @brief Disable move constructor.
     *        Although std::thread supports move semantics,
     *        allowing WorkerPool to be movable could transfer the
     *        ownership of an active thread, leaving the source
     *        Worker in an inconsistent state. To enforce strict
     *        ownership, movement is disabled.
     */
    WorkerPool(WorkerPool&&) = delete;

    /**
     * @brief Disable move assignment operator.
     *        Move assignment is also disabled for the same reason:
     *        to prevent accidental transfer of thread ownership
     *        between WorkerPool instances.
     */
    WorkerPool& operator=(WorkerPool&&) = delete;

    /**
     * @brief 
     */
    void start(const int number_workers);

     /**
     * @brief
     */
    void submit(std::function<void()> task);

    /**
     * @brief 
     */
    void stop();

    /******************************************************************/

    /* Private Methods */

   private:
    /**
     * @brief
     */
    void run(const std::string& worker_name);

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @brief ThreadSafeQueue used by the workers.
     */
    ThreadSafeQueue<std::function<void()>>& task_queue;

    /**
     * @brief Indicator of the worker that can be modified by other threads.
     */
    std::atomic<bool> running;

    /**
     * @brief Map gathering workers names attached to its threads.
     */
    std::unordered_map<std::string, std::thread> workers;

    /******************************************************************/
};