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

#include <iostream>
#include <functional>
#include <mutex>

/* Project libraries */
#include "thread_safe_queue.h"
#include "worker_pool.h"
#include "logger.h"

/*****************************************************************************/

int main() {
    ThreadSafeQueue<std::function<void()>> queue;
    WorkerPool pool(queue);
    std::mutex cout_mtx;

    pool.start(4);

    for (int i = 0; i < 10; ++i) {
        pool.submit([i, &cout_mtx] {
            std::this_thread::sleep_for(std::chrono::milliseconds(200 * (i % 3 + 1)));
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << "Tarea " << i << " completada en thread " << std::this_thread::get_id()
                      << std::endl;
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    pool.stop();

    return 0;
}