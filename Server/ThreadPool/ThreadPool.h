/**
 * @file ThreadPool.h
 * @brief Заголовочный файл с реализацией пула потоков.
 */

#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

/**
 * @class ThreadPool
 * @brief Класс для управления пулом потоков и асинхронным выполнением задач.
 *
 * Класс создает фиксированное количество потоков, которые обрабатывают 
 * задачи из очереди. Задачи можно добавлять через метод enqueue.
 */
class ThreadPool {
public:
    /**
     * @brief Конструктор пула потоков.
     * @param threads Количество потоков в пуле.
     */
    explicit ThreadPool(size_t threads);

    /**
     * @brief Деструктор. Останавливает все потоки и завершает выполнение задач.
     */
    ~ThreadPool();

    /**
     * @brief Добавляет задачу в очередь для выполнения.
     * @param func Функция без аргументов, которая будет выполнена в одном из потоков.
     */
    void enqueue(std::function<void()> func);

private:
    /**
     * @brief Метод, выполняемый каждым потоком. Ожидает задачи и выполняет их.
     */
    void worker();

    std::vector<std::thread> workers;               ///< Вектор рабочих потоков.
    std::queue<std::function<void()>> tasks;        ///< Очередь задач.
    std::mutex queue_mutex;                         ///< Мьютекс для синхронизации доступа к очереди.
    std::condition_variable cv;                     ///< Условная переменная для уведомления потоков.
    bool stop;                                      ///< Флаг завершения работы пула.
};
