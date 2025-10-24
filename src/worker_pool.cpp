/**
 * @file        worker_pool.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief       Implementation of the WorkerPool class.
 *
 * @details
 * This file defines the operational behavior of the WorkerPool, including:
 * - Thread creation and lifecycle management.
 * - Task submission and consumption.
 * - Graceful shutdown and queue coordination.
 *
 * The pool cooperates with a shared `ThreadSafeQueue<std::function<void()>>`
 * to execute submitted tasks in parallel across multiple worker threads.
 */

/*****************************************************************************/

/* Project libraries */

#include "worker_pool.h"

#include "logger.h"

/*****************************************************************************/

/* Public Methods */

/**
 * @brief Constructs a WorkerPool attached to a shared ThreadSafeQueue.
 *
 * @param queue Reference to the task queue shared among all workers.
 *
 * @details
 * GIVEN an existing queue instance,
 * WHEN the WorkerPool is constructed,
 * THEN it initializes its internal state but does not spawn any threads yet.
 *
 * The actual worker threads are created only after calling `start()`.
 */
WorkerPool::WorkerPool(ThreadSafeQueue<std::function<void()>>& queue)
    : task_queue(queue), running(false)
{
}

/**
 * @brief Destructor ensuring all threads are stopped and joined before cleanup.
 *
 * @details
 * Automatically invokes `stop()` to guarantee proper shutdown and to avoid
 * orphaned threads or deadlocks.
 *
 * @note
 * If the pool was never started, this is a no-op.
 */
WorkerPool::~WorkerPool()
{
    stop();
    workers.clear();
}

/**
 * @brief Launches multiple worker threads that consume tasks from the queue.
 *
 * @param number_workers Number of threads to start.
 *
 * @details
 * GIVEN an idle WorkerPool,
 * WHEN `start()` is invoked,
 * THEN:
 * - Sets the `running` flag to `true`.
 * - Creates `number_workers` threads.
 * - Each thread executes the `run()` method with a unique worker name.
 *
 * Example:
 * ```cpp
 * WorkerPool pool(queue);
 * pool.start(4); // Launch 4 worker threads
 * ```
 *
 * @note
 * - If the pool is already running, the function returns immediately.
 * - Each thread runs independently and blocks inside `ThreadSafeQueue::pop()`.
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
 * @brief Submits a task for execution by any available worker.
 *
 * @param task Callable object representing a unit of work.
 *
 * @details
 * GIVEN an active WorkerPool,
 * WHEN a producer thread calls `submit()`,
 * THEN the task is enqueued into `task_queue` and will be executed asynchronously
 * by the next available worker.
 *
 * Example:
 * ```cpp
 * pool.submit([] { std::cout << "Hello from worker!" << std::endl; });
 * ```
 *
 * @note
 * Thread-safe.
 * If the queue is closed or the pool stopped, pushing may be ignored.
 */
void WorkerPool::submit(std::function<void()> task)
{
    task_queue.push(std::move(task));
}

/**
 * @brief Stops all workers and ensures graceful shutdown.
 *
 * @details
 * GIVEN a running WorkerPool,
 * WHEN `stop()` is called,
 * THEN:
 *  - The `running` flag is set to `false`.
 *  - The pool waits briefly for remaining tasks to drain.
 *  - The queue is closed (`task_queue.close()`).
 *  - All worker threads are joined safely.
 *
 * @note
 * - The function blocks until all threads finish.
 * - If the queue still contains tasks after the timeout, a warning is logged.
 * - The method is idempotent (safe to call multiple times).
 *
 * @warning
 * This call will not forcibly interrupt a thread in `pop()` — threads exit naturally
 * once the queue closes or `pop()` returns false.
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
 * @brief Worker thread loop.
 *
 * @param worker_name Name identifier of the worker thread.
 *
 * @details
 * Each worker repeatedly attempts to retrieve tasks from the queue using `pop()`.
 *
 * GIVEN a running worker thread,
 * WHEN a new task is available in the queue,
 * THEN:
 *  - It is retrieved and executed.
 *  - If an exception occurs, it is caught and logged.
 *  - When the queue closes, the loop terminates gracefully.
 *
 * @note
 * This function runs indefinitely until:
 * - The queue returns `false` (closed and empty).
 * - Or the `running` flag becomes false.
 *
 * @exception std::exception
 * Any exception thrown by a task is caught, logged, and ignored to keep the pool stable.
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