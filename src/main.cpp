/**
 * @file        main.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief       Example entry point demonstrating the WorkerPool and ThreadSafeQueue.
 *
 * @details
 * This example shows how to:
 * 1. Create a thread-safe queue for task distribution.
 * 2. Initialize and start a worker pool with multiple threads.
 * 3. Submit multiple concurrent tasks.
 * 4. Wait for completion and gracefully stop all workers.
 *
 * ### Execution flow:
 * - Each submitted task simulates a workload with different durations.
 * - Tasks log their completion and the thread ID using the thread-safe `Logger`.
 * - After all tasks have been dispatched, the program waits briefly, then calls `stop()`
 *   to trigger a clean shutdown.
 *
 * @example
 * Example output:
 * ```
 * [INFO] [Worker Pool] Starting 4 workers
 * [INFO] [Main] Task 3 completed in thread: 140189585630976
 * [INFO] [Main] Task 1 completed in thread: 140189594023680
 * [INFO] [Worker Pool] Task queue drained, closing...
 * ```
 */

/*****************************************************************************/

/* Standard libraries */
#include <mutex>
#include <sstream>

/* Project libraries */
#include "logger.h"
#include "thread_safe_queue.h"
#include "worker_pool.h"

/*****************************************************************************/

/**
 * @brief Application entry point.
 *
 * @return Exit code (0 = success).
 *
 * @details
 * GIVEN the worker pool system components (`ThreadSafeQueue`, `WorkerPool`, and `Logger`),
 * WHEN the program is executed,
 * THEN:
 * - Initializes the queue and worker pool.
 * - Starts four worker threads.
 * - Submits ten tasks of varying simulated duration.
 * - Waits for execution and stops the pool gracefully.
 *
 * This function demonstrates the correct interaction between the components
 * and acts as a live validation of thread safety and task scheduling.
 */
int main()
{
    /**************************************************************************/
    /* 1. Create shared queue and worker pool                                 */
    /**************************************************************************/
    ThreadSafeQueue<std::function<void()>> queue;
    WorkerPool                             pool(queue);
    std::mutex                             cout_mtx;

    /**************************************************************************/
    /* 2. Configure logger verbosity                                          */
    /**************************************************************************/
    Logger::set_min_level(Logger::Level::INFO);

    /**************************************************************************/
    /* 3. Start worker threads                                                */
    /**************************************************************************/
    pool.start(4);

    /**************************************************************************/
    /* 4. Submit tasks                                                        */
    /**************************************************************************/
    for (int i = 0; i < 10; ++i)
    {
        pool.submit(
            [i, &cout_mtx]
            {
                // Simulate variable-duration workload
                std::this_thread::sleep_for(std::chrono::milliseconds(200 * (i % 3 + 1)));

                // Protect standard output
                std::lock_guard<std::mutex> lock(cout_mtx);

                std::ostringstream oss;
                oss << "[Main] Task " << i
                    << " completed in thread: " << std::this_thread::get_id();

                Logger::info(oss.str());
            });
    }

    /**************************************************************************/
    /* 5. Wait and stop workers gracefully                                    */
    /**************************************************************************/
    std::this_thread::sleep_for(std::chrono::seconds(1));
    pool.stop();

    /**************************************************************************/
    /* 6. Exit cleanly                                                        */
    /**************************************************************************/
    return 0;
}
