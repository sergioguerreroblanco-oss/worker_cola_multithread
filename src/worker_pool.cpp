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

#include "logger.h"

/*****************************************************************************/

/* Public Methods */

/**
 * @details Implementation of the Worker constructor.
 *          Initializes references to the queue and action, and sets the worker name.
 */
WorkerPool::WorkerPool(ThreadSafeQueue<std::function<void()>>& queue)
    : task_queue(queue), running(false)
{
}

/**
 * @details Ensures the worker thread has finished
 *          before destruction (joins the thread if needed)
 */
WorkerPool::~WorkerPool()
{
    stop();
    workers.clear();
}

/**
 * @details Starts the worker by setting the running flag to true
 *          and launching a dedicated thread that executes the run() loop.
 */
void WorkerPool::start(const int number_workers)
{
    if (running)
        return;

    running = true;
    Logger::info("[Worker Pool] Starting " + std::to_string(number_workers) + " workers");
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
{
    task_queue.push(std::move(task));
}

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
void WorkerPool::stop()
{
    if (!running)
        return;

    running = false;

    Logger::info("[Worker Pool] Stop requested, waiting for remaining tasks...");
    auto start = std::chrono::steady_clock::now();
    while (!task_queue.empty())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        if (std::chrono::steady_clock::now() - start > std::chrono::seconds(1))
        {
            Logger::warn("[Worker Pool] Timeout waiting for queue to drain.");
            break;
        }
    }

    task_queue.close();
    Logger::info("[Worker Pool] Task queue drained, closing...");

    for (auto& worker_pair : workers)
    {
        auto& thread = worker_pair.second;
        if (thread.joinable())
            thread.join();
    }

    workers.clear();
}

/**
 * @details Main worker loop.
 */
void WorkerPool::run(const std::string& worker_name)
{
    for (;;)
    {
        std::function<void()> task;

        if (!task_queue.pop(task))
            break;

        try
        {
            task();
        }
        catch (const std::exception& e)
        {
            Logger::error("[Worker Pool][" + worker_name + "] Exception: " + e.what());
        }
    }
}

/*****************************************************************************/

/* Private Methods */

/*****************************************************************************/