/**
 * @file        thread_safe_queue.ipp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief       Implementation of the ThreadSafeQueue template class.
 *
 * @details
 * This file provides the template definitions for all methods declared in
 * `thread_safe_queue.h`.
 *
 * Each method ensures correct synchronization using RAII-based locking via
 * `std::unique_lock` or `std::lock_guard`, and guarantees safe concurrent access
 * from multiple producer and consumer threads.
 */

/*****************************************************************************/

/* Project libraries */

#include "thread_safe_queue.h"
#include "logger.h"

/*****************************************************************************/

/* Public Methods */

/**
 * @brief Pushes a new element into the queue in a thread-safe manner.
 *
 * @param data Rvalue reference to the element to be enqueued.
 *
 * @details
 * GIVEN a running producer thread,
 * WHEN `push()` is called with a new element,
 * THEN the element is added to the back of the internal FIFO buffer
 * and one waiting consumer thread (if any) is notified.
 *
 * @note
 * - Locks the mutex before accessing the internal `std::deque`.
 * - Notifies one consumer waiting on the condition variable.
 * - Does not block (non-blocking push).
 *
 * @threadsafe Yes.
 * @throws Only if the internal container throws during `emplace_back`.
 */
template <typename T>
void ThreadSafeQueue<T>::push(T&& data) {
    std::unique_lock<std::mutex> lock(mtx);
    buffer.emplace_back(std::move(data));
    cv.notify_one();
}

/**
 * @brief Pops an element from the queue, blocking until one becomes available or the queue closes.
 *
 * @param[out] data Reference where the extracted element will be stored.
 * @return `true` if an element was successfully retrieved, or `false` if the queue
 *         was closed and no more data is available.
 *
 * @details
 * GIVEN one or more consumer threads waiting for tasks,
 * WHEN the queue is empty, `pop()` blocks the thread until either:
 * - A producer pushes a new element (`cv.notify_one()`), or
 * - The queue is closed via `close()`.
 *
 * THEN, once unblocked:
 * - If there is data available, the front element is popped and returned.
 * - If the queue is closed and empty, the function returns `false`.
 *
 * @note
 * - This call *will block* if the queue is empty and not closed.
 * - Once `close()` is invoked, all blocked threads are awakened.
 * - Thread safety is guaranteed via `std::unique_lock`.
 */
template <typename T>
bool ThreadSafeQueue<T>::pop(T& data) {
    std::unique_lock<std::mutex> lock(mtx);

    // Wait until new data is added
    cv.wait(lock, [this] { return closed || !buffer.empty(); });

    if (closed && buffer.empty()) return false;

    Logger::info("[Thread Safe Queue] Task extracted successfully");
    data = std::move(buffer.front());
    buffer.pop_front();
    return true;
}

/**
 * @brief Attempts to pop an element without blocking.
 *
 * @return A `nonstd::optional<T>` containing the popped element if available,
 *         or `nonstd::nullopt` if the queue is empty or closed.
 *
 * @details
 * GIVEN a queue that may or may not contain elements,
 * WHEN `try_pop()` is called,
 * THEN it will immediately:
 * - Return the front element (moved) if available.
 * - Return `nullopt` if the queue is empty or has been closed.
 *
 * @note
 * - This is a *non-blocking* call.
 * - Safe to call concurrently with `push()` and `pop()`.
 */
template <typename T>
nonstd::optional<T> ThreadSafeQueue<T>::try_pop() {
    std::lock_guard<std::mutex> lock(mtx);

    if (!closed && !buffer.empty()) {
        T data = std::move(buffer.front());
        buffer.pop_front();
        Logger::info("[Thread Safe Queue] Task extracted successfully");
        return data;
    }
    Logger::info("[Thread Safe Queue] No task extracted");
    return nonstd::nullopt;
}

/**
 * @brief Checks whether the queue is empty.
 *
 * @return `true` if the internal buffer contains no elements, otherwise `false`.
 *
 * @details
 * This method provides a thread-safe check on the queue’s current size.
 *
 * @note
 * - Acquires a brief lock to safely inspect the buffer.
 * - Useful for status checks or conditional waiting logic.
 */
template <typename T>
bool ThreadSafeQueue<T>::empty() const {
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.empty();
}

/**
 * @brief Returns the number of elements currently stored in the queue.
 *
 * @return The number of elements in the internal buffer.
 *
 * @details
 * @note
 * - Thread-safe read operation.
 * - Acquires a short-lived lock on the internal mutex.
 */
template <typename T>
size_t ThreadSafeQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.size();
}

/**
 * @brief Removes all elements from the queue.
 *
 * @details
 * GIVEN a queue that may contain elements,
 * WHEN `clear()` is called,
 * THEN all stored elements are destroyed and the queue becomes empty.
 *
 * @note
 * - Does not modify the `closed` flag.
 * - Thread-safe.
 * - Should be used cautiously in concurrent systems to avoid discarding data
 *   still being processed by consumers.
 */
template <typename T>
void ThreadSafeQueue<T>::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    Logger::info("[Thread Safe Queue] Tasks cleaned");
    buffer.clear();
}

/**
 * @brief Closes the queue and unblocks all waiting threads.
 *
 * @details
 * GIVEN a running queue with potential blocking consumers,
 * WHEN `close()` is invoked,
 * THEN:
 * - Sets the internal `closed` flag to `true`.
 * - Notifies all threads waiting on `cv.wait()` so they can terminate gracefully.
 *
 * After closure:
 * - `pop()` will return `false` once the queue becomes empty.
 * - `try_pop()` will return `nullopt` if empty.
 *
 * @note
 * - Safe to call multiple times (idempotent).
 * - Commonly used before destruction to prevent deadlocks.
 */
template <typename T>
void ThreadSafeQueue<T>::close() {
    std::lock_guard<std::mutex> lock(mtx);
    closed = true;
    Logger::info("[Thread Safe Queue] Task queue closed");
    cv.notify_all();
}

/*****************************************************************************/

/* Private Methods */

/*****************************************************************************/