/**
 * @file        thread_safe_queue.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief       Thread-safe FIFO queue implementation for concurrent producers and consumers.
 *
 * @details
 * This class provides a minimal and efficient thread-safe FIFO queue based on
 * `std::deque`, `std::mutex`, and `std::condition_variable`.
 *
 * It supports both blocking (`pop`) and non-blocking (`try_pop`) access patterns,
 * and provides a `close()` mechanism for graceful shutdown, allowing waiting threads
 * to exit cleanly when the queue is being destroyed or stopped.
 *
 * The queue follows a producer-consumer design pattern, ensuring that:
 *  - Multiple producers can push elements concurrently.
 *  - Multiple consumers can safely pop or try_pop elements.
 *  - Synchronization is handled internally using RAII locks.
 *
 * The class is intentionally **non-copyable** and **non-movable**, as it manages
 * synchronization primitives (`std::mutex`, `std::condition_variable`) that cannot be
 * transferred safely between instances.
 */

/*****************************************************************************/

/* Include Guard */
#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <condition_variable>
#include <deque>
#include <mutex>

/* Third party libraries */

#include "third_party/optional.hpp"

/*****************************************************************************/

/**
 * @class ThreadSafeQueue
 * @brief Thread-safe FIFO queue supporting multiple producers and consumers.
 *
 * @tparam T Type of element stored in the queue.
 *
 * @note
 * This queue is designed for use in multi-threaded environments.
 * It provides blocking (`pop`) and non-blocking (`try_pop`) retrieval operations,
 * as well as a `close()` method for graceful termination of waiting consumers.
 */
template <typename T>
class ThreadSafeQueue
{
    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Default constructor.
     *
     * Initializes an empty queue ready for use by producer and consumer threads.
     */
    explicit ThreadSafeQueue() = default;

    /**
     * @brief Destructor.
     *
     * Automatically releases resources. If `close()` was not called, any threads waiting
     * on `pop()` may be unblocked during destruction.
     */
    ~ThreadSafeQueue() = default;

    /**
     * @brief Disable copy constructor.
     *
     * ThreadSafeQueue cannot be copied because it manages synchronization primitives
     * (`std::mutex`, `std::condition_variable`), which are inherently non-copyable.
     */
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;

    /**
     * @brief Disable copy assignment operator.
     *
     * Copy assignment is forbidden for the same reason as the copy constructor:
     * synchronization primitives cannot be duplicated safely.
     */
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    /**
     * @brief Disable move constructor.
     *
     * Moving synchronization primitives would invalidate any existing waiting threads.
     * To preserve strict ownership semantics, move operations are not supported.
     */
    ThreadSafeQueue(ThreadSafeQueue&&) = delete;

    /**
     * @brief Disable move assignment operator.
     *
     * Moving the queue could break thread synchronization guarantees, so it is disabled.
     */
    ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

    /**
     * @brief Pushes a new element into the queue (thread-safe).
     *
     * @param data Rvalue reference to the element being pushed.
     *
     * @details
     * Adds an element to the back of the internal buffer.
     * If one or more threads are waiting in `pop()`, one of them will be notified.
     *
     * This method uses perfect forwarding via `std::move()`.
     *
     * @warning
     * Throws no exceptions unless the internal `std::deque::emplace_back` does.
     */
    void push(T&& data);

    /**
     * @brief Pops an element from the queue, blocking until one becomes available.
     *
     * @param[out] data Reference to a variable where the popped value will be stored.
     * @return `true` if an element was successfully retrieved, or `false` if the queue
     *         was closed and empty.
     *
     * @details
     * This method will block the calling thread until:
     *  - An element is available to pop, or
     *  - The queue has been closed via `close()`.
     *
     * If the queue is closed and empty, the method returns `false`.
     */
    bool pop(T& data);

    /**
     * @brief Attempts to pop an element without blocking.
     *
     * @return A `nonstd::optional<T>` containing the element if available,
     *         or `nonstd::nullopt` if the queue is empty or closed.
     *
     * @details
     * This method is non-blocking and returns immediately.
     * Useful for polling scenarios or single-threaded draining logic.
     */
    nonstd::optional<T> try_pop();

    /**
     * @brief Checks whether the queue is currently empty.
     *
     * @return `true` if the queue is empty, `false` otherwise.
     *
     * @note
     * This method is thread-safe and can be called concurrently with other operations.
     */
    bool empty() const;

    /**
     * @brief Returns the current number of elements in the queue.
     *
     * @return Number of elements stored in the internal buffer.
     *
     * @note
     * This method acquires a lock briefly to read the size safely.
     */
    size_t size() const;

    /**
     * @brief Clears all elements currently stored in the queue.
     *
     * @details
     * Removes all elements from the internal buffer.
     * Does not affect the `closed` state.
     *
     * @warning
     * Should be used carefully in concurrent environments to avoid discarding
     * data being produced/consumed simultaneously.
     */
    void clear();

    /**
     * @brief Closes the queue, unblocking all waiting threads.
     *
     * @details
     * Sets an internal flag (`closed = true`) and notifies all waiting threads
     * so they can exit gracefully.
     *
     * After calling this, subsequent calls to `pop()` will return `false`
     * once the queue is empty, and `try_pop()` will return `nullopt`.
     */
    void close();

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @brief Internal FIFO buffer used to store queued elements.
     */
    std::deque<T> buffer;

    /**
     * @brief Mutex protecting access to the buffer and synchronization state.
     */
    mutable std::mutex mtx;

    /**
     * @brief Condition variable used to signal availability of data.
     */
    std::condition_variable cv;

    /**
     * @brief Indicates whether the queue has been closed (graceful shutdown flag).
     */
    bool closed = false;

    /******************************************************************/
};

#include "thread_safe_queue.ipp"