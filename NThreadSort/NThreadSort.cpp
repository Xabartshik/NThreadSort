#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <mutex>
#include <random>
#include <chrono>
#include "utils.h"
using namespace std;
using namespace std::chrono;
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
    test_parallel_sort();

    int numThreads = 8, dataSize;
    vector<int> dataSizes = { 2000000, 4000000, 6000000, 8000000, 10000000, 12000000, 14000000, 16000000, 18000000, 20000000 };




    vector<long long> parallelTimes;
    vector<long long> sequentialTimes;

    random_device rd;
    mt19937 gen(rd());

    for (int size : dataSizes) {
        cout << "\nТестируем размер массива: " << size << endl;

        // Генерация случайного массива
        uniform_int_distribution<> distrib(1, size - 1);
        vector<int> data(size);
        for (int i = 0; i < size; ++i) {
            data[i] = distrib(gen);
        }

        // Копии для тестов
        vector<int> dataForParallel = data;
        vector<int> dataForSequential = data;

        // Параллельная сортировка
        auto start = high_resolution_clock::now();
        parallel_sort(dataForParallel, numThreads);
        auto end = high_resolution_clock::now();
        auto parallelDuration = duration_cast<milliseconds>(end - start);
        parallelTimes.push_back(parallelDuration.count());

        bool parallelSorted = is_sorted(dataForParallel.begin(), dataForParallel.end());

        // Последовательная сортировка
        start = high_resolution_clock::now();
        sort(dataForSequential.begin(), dataForSequential.end());
        end = high_resolution_clock::now();
        auto sequentialDuration = duration_cast<milliseconds>(end - start);
        sequentialTimes.push_back(sequentialDuration.count());

        bool sequentialSorted = is_sorted(dataForSequential.begin(), dataForSequential.end());

        // Вывод результатов
        cout << "Параллельная сортировка (" << numThreads << " потоков): "
            << parallelDuration.count() << " мс, корректность: "
            << (parallelSorted ? "да" : "нет") << endl;

        cout << "Последовательная сортировка: "
            << sequentialDuration.count() << " мс, корректность: "
            << (sequentialSorted ? "да" : "нет") << endl;

        if (!parallelSorted) find_first_unsorted(dataForParallel);
    }

    // Вывод результатов
    cout << "\nИтоговые результаты:" << endl;
    cout << "Размеры массивов: [";
    for (int size : dataSizes) cout << size << (size != dataSizes.back() ? ", " : "");
    cout << "]" << endl;

    cout << "Время параллельной сортировки (мс): [";
    for (long long time : parallelTimes) cout << time << (time != parallelTimes.back() ? ", " : "");
    cout << "]" << endl;

    cout << "Время последовательной сортировки (мс): [";
    for (long long time : sequentialTimes) cout << time << (time != sequentialTimes.back() ? ", " : "");
    cout << "]" << endl;

    cout << "Ускорение (послед./паралл.): [";
    for (size_t i = 0; i < dataSizes.size(); ++i) {
        double speedup = static_cast<double>(sequentialTimes[i]) / parallelTimes[i];
        cout << speedup << (i != dataSizes.size() - 1 ? ", " : "");
    }
    cout << "]" << endl;

    return 0;
}
