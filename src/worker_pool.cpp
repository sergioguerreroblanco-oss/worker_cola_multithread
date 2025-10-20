/**
 * @file        worker_pool.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief       
 *
 * @details
 */

/*****************************************************************************/

/* Project libraries */

#include "worker_pool.h"

/*****************************************************************************/

/* Public Methods */

/**
 * @details Implementation of the Worker constructor.
 *          Initializes references to the queue and action, and sets the worker name.
 */
WorkerPool::WorkerPool(ThreadSafeQueue<std::function<void()>>& queue)
    : task_queue(queue), running(false) {}

/**
 * @details Ensures the worker thread has finished
 *          before destruction (joins the thread if needed)
 */
WorkerPool::~WorkerPool() {
    stop();
    workers.clear();
}

/**
 * @details Starts the worker by setting the running flag to true
 *          and launching a dedicated thread that executes the run() loop.
 */
void WorkerPool::start(const int number_workers) {
    if (running) return;

    running = true;
    for (int i = 0; i < number_workers; i++)
    {
        std::string name = std::string(DEFAULT_WORKER_NAME) + std::to_string(i);

         workers.emplace(name, std::thread([this, name]() { run(name); }));
    }
}

/**
 * @details 
 */
void WorkerPool::submit(std::function<void()> task)
{ task_queue.push(std::move(task)); }

/**
 * @details Stops the worker activity by setting its running flag to false.
 *          The worker thread will complete its current loop iteration and then exit.
 *          This does not affect the underlying queue.
 * @note stop() does not interrupt pop() immediately.
 *       If the worker is waiting in pop(), it will exit
 *       only after the configured timeout (5s by default).
 *       This simplifies the design and avoids pushing
 *       shutdown logic into the queue.
 */
void WorkerPool::stop() {
    if (!running) return;

    running = false;

    task_queue.close();

    for (auto& worker_pair : workers) {
        auto& thread = worker_pair.second;
        if (thread.joinable()) thread.join();
    }

    workers.clear();
}

/**
 * @details Main worker loop.
 */
void WorkerPool::run(const std::string& worker_name) {
    while (running) {
        std::function<void()> task;
        task_queue.pop(task);

        if (task) {
            try {
                task();
            } catch (const std::exception& e) {
                std::cerr << "[" << worker_name << "] Exception: " << e.what() << std::endl;
            }
        }
    }
}

/*****************************************************************************/

/* Private Methods */

/*****************************************************************************/