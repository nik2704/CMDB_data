#define BOOST_TEST_MODULE ThreadPoolTests
#include <boost/test/included/unit_test.hpp>
#include <atomic>
#include <chrono>
#include <thread>
#include "../../Server/ThreadPool/ThreadPool.h"

BOOST_AUTO_TEST_CASE(SingleTaskTest) {
    ThreadPool pool(2);
    std::atomic<bool> taskExecuted{false};

    pool.enqueue([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        taskExecuted = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    BOOST_CHECK(taskExecuted.load());
}

BOOST_AUTO_TEST_CASE(MultipleTasksTest) {
    ThreadPool pool(4);
    std::atomic<int> counter{0};

    for (int i = 0; i < 10; ++i) {
        pool.enqueue([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ++counter;
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    BOOST_CHECK_EQUAL(counter.load(), 10);
}
