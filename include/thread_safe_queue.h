/**
 * @file        thread_safe_queue.h
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
 * @brief 
 * @tparam 
 */
template <typename T>
class ThreadSafeQueue {
    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Constructor of the ThreadSafeQueue class.
     */
    explicit ThreadSafeQueue() = default;

    /**
     * @brief Destructor of the ThreadSafeQueue class.
     */
    ~ThreadSafeQueue() = default;

    /**
     * @brief Disable copy constructor.
     *        ThreadSafeQueue cannot be copied because it manages synchronization primitives
     *        (std::mutex, std::condition_variable), which are non-copyable.
     */
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;

    /**
     * @brief Disable copy assignment operator.
     *        ThreadSafeQueue cannot be copy-assigned for the same reason:
     *        synchronization primitives cannot be duplicated safely.
     */
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    /**
     * @brief Disable move constructor.
     *        Although std::mutex and std::condition_variable technically
     *        have move operations deleted, even if they were movable,
     *        transferring them between ThreadSafeQueue instances would break
     *        synchronization guarantees. To enforce strict ownership,
     *        movement is disabled.
     */
    ThreadSafeQueue(ThreadSafeQueue&&) = delete;

    /**
     * @brief Disable move assignment operator.
     *        Move assignment is also disabled to avoid transferring
     *        synchronization primitives between instances, which could
     *        lead to undefined behavior.
     */
    ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

    /**
     * @brief 
     * @param 
     */
    void push(T&& data);

    /**
     * @brief 
     * @param 
     */
    void pop(T& data);

    /**
     * @brief
     * @param
     */
    nonstd::optional<T> try_pop();

     /**
     * @brief
     * @param
     */
    bool empty() const;

     /**
     * @brief
     * @param
     */
    size_t size() const;

    /**
     * @brief
     * @param
     */
    void clear();

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @brief FIFO buffer.
     */
    std::deque<T> buffer;

    /**
     * @brief Mutex.
     */
    mutable std::mutex mtx;

    /**
     * @brief Condition variable.
     */
    std::condition_variable cv;

    /******************************************************************/
};

#include "thread_safe_queue.ipp"