/**
 * @file        main.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief       
 *
 * @details
 */

/*****************************************************************************/

/* Standard libraries */

#include <mutex>
#include <sstream>

/* Project libraries */
#include "thread_safe_queue.h"
#include "worker_pool.h"
#include "logger.h"

/*****************************************************************************/

int main() {
    ThreadSafeQueue<std::function<void()>> queue;
    WorkerPool pool(queue);
    std::mutex cout_mtx;

    Logger::set_min_level(Logger::Level::INFO);

    pool.start(4);

    for (int i = 0; i < 10; ++i) {
        pool.submit([i, &cout_mtx] {
            std::this_thread::sleep_for(std::chrono::milliseconds(200 * (i % 3 + 1)));
            std::lock_guard<std::mutex> lock(cout_mtx);

            std::ostringstream oss;
            oss << "[Main] Task " << i << " completed in thread: " << std::this_thread::get_id();
            Logger::info(oss.str());
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    pool.stop();

    return 0;
}