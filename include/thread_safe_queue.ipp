/**
 * @file        thread_safe_queue.ipp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief       
 *
 * @details
 * 
 */

/*****************************************************************************/

/* Project libraries */

#include "thread_safe_queue.h"

/*****************************************************************************/

/* Public Methods */

/**
 * @details
 */
template <typename T>
void ThreadSafeQueue<T>::push(T&& data) {
    std::unique_lock<std::mutex> lock(mtx);
    buffer.emplace_back(std::move(data));
    cv.notify_one();
}

/**
 * @details 
 * @return 
 */
template <typename T>
void ThreadSafeQueue<T>::pop(T& data) {
    std::unique_lock<std::mutex> lock(mtx);

    // Wait until new data is added
    cv.wait(lock, [this] { return !buffer.empty(); });
    data = std::move(buffer.front());
    buffer.pop_front();
}

/**
 * @details
 * @return
 */
template <typename T>
nonstd::optional<T> ThreadSafeQueue<T>::try_pop() {
    std::lock_guard<std::mutex> lock(mtx);

    if (!buffer.empty()) {
        T data = std::move(buffer.front());
        buffer.pop_front();
        return data;
    }
    return nonstd::nullopt;
}

/**
 * @details
 * @return
 */
template <typename T>
bool ThreadSafeQueue<T>::empty() const {
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.empty();
}

/**
 * @details
 * @return
 */
template <typename T>
size_t ThreadSafeQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.size();
}

/**
 * @details
 * @return
 */
template <typename T>
void ThreadSafeQueue<T>::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    buffer.clear();
}

/*****************************************************************************/

/* Private Methods */

/*****************************************************************************/