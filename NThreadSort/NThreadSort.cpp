#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <mutex>
#include <random>
#include <chrono>
#include "utils.h"
using namespace std;

/*
Введите количество потоков: 8
Введите размер массива: 10000000
Время сортировки: 370 мс
0


Введите количество потоков: 1
Введите размер массива: 10000000
Время сортировки: 1433 мс
1
*/
void find_first_unsorted(const std::vector<int>& vec) {
    for (size_t i = 1; i < vec.size(); ++i) {
        if (vec[i] < vec[i - 1]) {
            std::cout << "Found unsorted element: " << vec[i] << std::endl;
            std::cout << "Index: " << i << std::endl;

            if (i > 0) {
                std::cout << "Left neighbors: " << vec[i - 1] << " " << vec[i - 2] << std::endl;
            }
            else {
                std::cout << "No left neighbor (first element)" << std::endl;
            }

            if (i < vec.size() - 1) {
                std::cout << "Right neighbors: " << vec[i + 1] << " " << vec[i + 2] << std::endl;
            }
            else {
                std::cout << "No right neighbor (last element)" << std::endl;
            }

            return;
        }
    }

    std::cout << "The vector is fully sorted in ascending order" << std::endl;
}

int main() {
    int numThreads = 8, dataSize;

    cout << "Введите количество потоков: ";
    cin >> numThreads;

    cout << "Введите размер массива: ";
    cin >> dataSize;


    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, dataSize-1); // Генерируем числа от 1 до 1000 (равномерное распределение)

    vector<int> data(dataSize);
    for (int i = 0; i < dataSize; ++i) {
        data[i] = distrib(gen);
    }


    auto start = chrono::high_resolution_clock::now();
    parallel_sort(data, numThreads);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Время сортировки: " << duration.count() << " мс" << endl;

    find_first_unsorted(data);

    // Вывод отсортированного массива (для проверки)
    cout << is_sorted(data.cbegin(), data.cend()) << " ";
    cout << endl;

    return 0;
}