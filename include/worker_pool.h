/**
 * @file        worker_pool.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief       Thread pool that manages multiple worker threads consuming tasks from a shared
 * queue.
 *
 * @details
 * The WorkerPool class coordinates a fixed number of worker threads that
 * continuously consume tasks from a `ThreadSafeQueue<std::function<void()>>`.
 *
 * The design follows a **producer-consumer model**:
 * - The main thread (producer) submits tasks via `submit()`.
 * - Worker threads (consumers) run the `run()` loop, each retrieving and executing tasks.
 *
 * The pool supports **graceful shutdown** via `stop()`, which waits for pending tasks to finish
 * before joining all worker threads.
 *
 * Key properties:
 * - Thread-safe task submission.
 * - Deterministic start/stop lifecycle.
 * - Automatic synchronization through `ThreadSafeQueue`.
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

/* Project libraries */

#include "thread_safe_queue.h"

/*****************************************************************************/

/**
 * @class WorkerPool
 * @brief Fixed-size pool of worker threads that execute queued tasks concurrently.
 *
 * @details
 * Each worker runs a loop (`run()`) that blocks on `ThreadSafeQueue::pop()`
 * until a new task arrives or the queue closes.
 *
 * This implementation favors **simplicity and determinism**:
 * - Each worker owns exactly one `std::thread`.
 * - `stop()` guarantees that all threads finish cleanly.
 * - `submit()` is lock-free from the caller’s perspective.
 *
 * @note
 * WorkerPool does **not** own the queue — it receives a reference to an external
 * `ThreadSafeQueue` instance during construction. This allows flexible sharing or
 * reuse between multiple components.
 */
class WorkerPool
{
    /******************************************************************/

    /* Private Constants */

   private:
    /**
     * @brief Default prefix for worker thread names.
     */
    static constexpr const char* DEFAULT_WORKER_NAME = "Worker ";

    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Constructs a WorkerPool attached to an existing ThreadSafeQueue.
     *
     * @param queue Reference to the queue from which worker threads will consume tasks.
     *
     * @details
     * GIVEN an external `ThreadSafeQueue<std::function<void()>>`,
     * WHEN the WorkerPool is constructed,
     * THEN it binds to that queue but does not start any threads yet.
     *
     * @note
     * Threads are only created when `start()` is explicitly called.
     */
    explicit WorkerPool(ThreadSafeQueue<std::function<void()>>& queue);

    /**
     * @brief Destructor.
     *
     * @details
     * Ensures that all threads are stopped and joined before destruction.
     *
     * @note
     * Calls `stop()` automatically for safety, even if the user forgets.
     */
    ~WorkerPool();

    /**
     * @brief Deleted copy constructor.
     *
     * @details
     * WorkerPool cannot be copied because it manages active threads and synchronization primitives.
     * Copying would imply duplicating ownership of threads, which is undefined behavior.
     */
    WorkerPool(const WorkerPool&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * @details
     * Prevents assignment between WorkerPool instances for the same reason as the copy constructor:
     * thread ownership cannot be shared safely.
     */
    WorkerPool& operator=(const WorkerPool&) = delete;

    /**
     * @brief Deleted move constructor.
     *
     * @details
     * Although `std::thread` supports move semantics, allowing WorkerPool to be movable would
     * transfer ownership of active threads and invalidate the source object. Movement is disabled
     * to enforce strict lifetime control.
     */
    WorkerPool(WorkerPool&&) = delete;

    /**
     * @brief Deleted move assignment operator.
     *
     * @details
     * Move assignment is disabled to avoid undefined behavior related to thread ownership transfer.
     */
    WorkerPool& operator=(WorkerPool&&) = delete;

    /**
     * @brief Starts the specified number of worker threads.
     *
     * @param number_workers Number of threads to create and start.
     *
     * @details
     * GIVEN an idle WorkerPool,
     * WHEN `start(n)` is called,
     * THEN it spawns `n` worker threads, each executing the `run()` loop.
     *
     * @note
     * - Each worker is named `"Worker 0"`, `"Worker 1"`, etc.
     * - If the pool is already running, subsequent calls to `start()` are ignored.
     * - Thread creation uses `std::thread`, and each thread blocks on `task_queue.pop()`.
     */
    void start(const int number_workers);

    /**
     * @brief Submits a task to be executed by any available worker.
     *
     * @param task Callable object representing a task (e.g., lambda or `std::bind`).
     *
     * @details
     * GIVEN a running pool with one or more active workers,
     * WHEN `submit()` is called,
     * THEN the task is moved into the queue and executed asynchronously
     * by the next available worker thread.
     *
     * @note
     * - Thread-safe.
     * - If the queue is closed, submitting new tasks may have no effect.
     * - Uses perfect forwarding and `std::move()` for efficiency.
     */
    void submit(std::function<void()> task);

    /**
     * @brief Stops all workers and waits for remaining tasks to complete.
     *
     * @details
     * GIVEN a running WorkerPool,
     * WHEN `stop()` is invoked,
     * THEN:
     * - The `running` flag is set to `false`.
     * - The queue is closed (via `task_queue.close()`).
     * - Each thread is joined safely.
     *
     * This provides a **graceful shutdown**, allowing all pending tasks to finish
     * before termination.
     *
     * @note
     * - Calling `stop()` multiple times is safe (idempotent).
     * - Must be called before destroying the WorkerPool to avoid leaks.
     */
    void stop();

    /******************************************************************/

    /* Private Methods */

   private:
    /**
     * @brief Main worker loop executed by each thread.
     *
     * @param worker_name Unique name identifying the thread (e.g., "Worker 0").
     *
     * @details
     * Each worker continuously calls `task_queue.pop(task)` and executes the returned task
     * until:
     * - The queue is closed (`pop()` returns false), or
     * - The `running` flag becomes false.
     *
     * @note
     * - Catches and logs any exceptions thrown by user tasks.
     * - This method never throws.
     */
    void run(const std::string& worker_name);

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @brief Reference to the task queue shared among workers.
     *
     * @details
     * The queue must outlive the WorkerPool instance.
     * The pool never owns or destroys the queue; it only accesses it.
     */
    ThreadSafeQueue<std::function<void()>>& task_queue;

    /**
     * @brief Atomic flag controlling the running state of all workers.
     *
     * @details
     * Used to coordinate start/stop operations and prevent race conditions.
     */
    std::atomic<bool> running;

    /**
     * @brief Map of worker names to their associated threads.
     *
     * @details
     * Stores active workers as (`name`, `std::thread`) pairs.
     * Threads are joined and cleared during shutdown.
     */
    std::unordered_map<std::string, std::thread> workers;

    /******************************************************************/
};