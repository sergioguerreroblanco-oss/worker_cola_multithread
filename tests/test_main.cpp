/**
 * @file        test_main.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        2025-09-15
 * @version     1.0.0
 *
 * @brief Unit tests for ThreadSafeQueue and WorkerPool components.
 *
 * @details
 * These tests validate the correct behavior of the multithreaded system,
 * including synchronized access, task execution, graceful shutdown, and
 * non-blocking operations.
 *
 * The tests follow the GIVEN / WHEN / THEN documentation pattern:
 *  - GIVEN: Context or initial setup.
 *  - WHEN:  Action or operation being tested.
 *  - THEN:  Expected outcome or verification.
 */

/* Standard libraries */

#include <atomic>
#include <chrono>
#include <thread>
#include <functional>
#include <gtest/gtest.h>

/* Project libraries */

#include "thread_safe_queue.h"
#include "worker_pool.h"

/*****************************************************************************/

/* Tests */

/**
 * @test ThreadSafeQueue.PushPopWorks
 * @brief Verify basic push/pop behavior in a single-thread context.
 *
 * @details
 * GIVEN a ThreadSafeQueue with one pushed value
 * WHEN pop() is called
 * THEN the same value must be retrieved correctly.
 */
TEST(ThreadSafeQueue, PushPopWorks) {
    ThreadSafeQueue<int> q;
    q.push(42);

    int val = 0;
    q.pop(val);
    EXPECT_EQ(val, 42);
}

/**
 * @test ThreadSafeQueue.GracefulClose
 * @brief Ensure pop() unblocks gracefully when the queue is closed.
 *
 * @details
 * GIVEN a consumer thread waiting on pop()
 * WHEN close() is called
 * THEN the waiting thread must unblock and the queue must be empty.
 */
TEST(ThreadSafeQueue, GracefulClose) {
    ThreadSafeQueue<int> q;
    std::thread t([&q]() {
        int val;
        q.pop(val);  // should unblock when closed
    });
    q.close();
    t.join();
    EXPECT_TRUE(q.empty());
}

/**
 * @test WorkerPool.ExecutesAllTasks
 * @brief Validate that all submitted tasks are executed.
 *
 * @details
 * GIVEN a WorkerPool with 3 workers
 * WHEN 10 tasks are submitted
 * THEN all of them must be executed before stop().
 */
TEST(WorkerPool, ExecutesAllTasks) {
    ThreadSafeQueue<std::function<void()>> queue;
    WorkerPool pool(queue);
    std::atomic<int> counter{0};

    pool.start(3);
    for (int i = 0; i < 10; ++i) pool.submit([&] { ++counter; });
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pool.stop();

    EXPECT_EQ(counter, 10);
}

/**
 * @class WorkerPoolParamTest
 * @brief Parameterized test fixture to validate WorkerPool scalability.
 *
 * @details
 * Runs the same test with different numbers of worker threads.
 */
class WorkerPoolParamTest : public ::testing::TestWithParam<int> {
   protected:
    ThreadSafeQueue<std::function<void()>> queue;
    std::atomic<int> counter{0};

    void SetUp() override { counter = 0; }
};


/**
 * @test WorkerPoolParamTest.ExecutesAllTasksWithDifferentWorkerCounts
 * @brief Check correctness with varying number of workers.
 *
 * @details
 * GIVEN a WorkerPool with N workers
 * WHEN 20 tasks are submitted
 * THEN all must be executed exactly once, regardless of thread count.
 */
TEST_P(WorkerPoolParamTest, ExecutesAllTasksWithDifferentWorkerCounts) {
    const int num_workers = GetParam();
    WorkerPool pool(queue);
    pool.start(num_workers);

    const int total_tasks = 20;
    for (int i = 0; i < total_tasks; ++i)
        pool.submit([&] {
            ++counter;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    pool.stop();

    EXPECT_EQ(counter, total_tasks) << "Failed with " << num_workers << " workers";
}

INSTANTIATE_TEST_SUITE_P(WorkerPoolVariations,
                         WorkerPoolParamTest,
                         ::testing::Values(1, 2, 4, 8)
);

/**
 * @test ThreadSafeQueue.TryPopNonBlocking
 * @brief Validate that try_pop() behaves correctly without blocking.
 *
 * @details
 * GIVEN an empty ThreadSafeQueue
 * WHEN try_pop() is called
 * THEN it must return nullopt immediately.
 *
 * GIVEN a queue with one item
 * WHEN try_pop() is called
 * THEN it must retrieve the item and leave the queue empty.
 */
TEST(ThreadSafeQueue, TryPopNonBlocking) {
    ThreadSafeQueue<int> q;

    auto result = q.try_pop();
    EXPECT_FALSE(result.has_value()) << "try_pop() must return nullopt on an empty queue";

    q.push(99);
    result = q.try_pop();
    ASSERT_TRUE(result.has_value()) << "try_pop() must return a value when there is data";
    EXPECT_EQ(result.value(), 99) << "The returned value must match with the one inserted previously";

    result = q.try_pop();
    EXPECT_FALSE(result.has_value()) << "try_pop() must return nullopt after empty the queue";
}

/**
 * @test ThreadSafeQueue.TryPopConcurrentReaders
 * @brief Validate thread-safety when multiple readers use try_pop().
 *
 * @details
 * GIVEN a queue prefilled with 100 elements
 * WHEN two concurrent threads call try_pop()
 * THEN all elements must be consumed exactly once and the total count must match.
 */
TEST(ThreadSafeQueue, TryPopConcurrentReaders) {
    ThreadSafeQueue<int> q;
    const int max = 100;
    std::atomic<int> count{0};

    for (int i = 0; i < max; ++i) { q.push(std::move(i)); }

    auto consumer = [&]() {
        while (auto val = q.try_pop()) {
            ++count;
        }
    };

    std::thread t1(consumer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    EXPECT_EQ(count.load(), max)
        << "All the values must be retrieved only once until the queue is empty";
}