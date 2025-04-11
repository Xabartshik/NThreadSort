// main.cpp
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "utils.h"

int main() {
    int numThreads, numSubtasks, dataSize;

    std::cout << "Введите количество потоков: ";
    std::cin >> numThreads;

    std::cout << "Введите количество подзадач: ";
    std::cin >> numSubtasks;

    std::cout << "Введите размер массива: ";
    std::cin >> dataSize;


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 1000); // Генерируем числа от 1 до 1000 (равномерное распределение)

    std::vector<int> data(dataSize);
    for (int i = 0; i < dataSize; ++i) {
        data[i] = distrib(gen);
    }

    ParallelSorter sorter(numThreads, numSubtasks);

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> sortedData = sorter.sort(data);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Время сортировки: " << duration.count() << " мс" << std::endl;

    // Вывод отсортированного массива (для проверки)
    for (int x : sortedData) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    return 0;
}