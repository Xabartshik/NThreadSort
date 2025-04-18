#include <vector>
#include <thread>
#include <algorithm>
#include <cstring>

// Функция для сортировки части массива
void sort_part(std::vector<int>& arr, int start, int end) {
    std::sort(arr.begin() + start, arr.begin() + end);
}

// Функция для слияния двух отсортированных частей массива
void merge(std::vector<int>& arr, int start1, int end1, int start2, int end2) {
    std::vector<int> temp(end2 - start1);

    int i = start1, j = start2, k = 0;
    while (i < end1 && j < end2) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        }
        else {
            temp[k++] = arr[j++];
        }
    }

    // Копируем оставшиеся элементы из левой части
    while (i < end1) {
        temp[k++] = arr[i++];
    }

    // Копируем оставшиеся элементы из правой части
    while (j < end2) {
        temp[k++] = arr[j++];
    }

    // Копируем обратно в исходный массив
    std::memcpy(&arr[start1], &temp[0], temp.size() * sizeof(int));
}

void parallel_sort(std::vector<int>& arr, int num_threads) {
    if (arr.empty()) return;

    int size = arr.size();
    num_threads = std::min(num_threads, size);
    int chunk_size = (size + num_threads - 1) / num_threads;  // Округление вверх

    std::vector<std::thread> threads;

    // Сортировка частей массива в отдельных потоках
    for (int i = 0; i < num_threads; ++i) {
        int start = i * chunk_size;
        int end = std::min(start + chunk_size, size);
        /*
        Типа push_back, но вызывает внутри себя конструктор, создава объект на основе переданных параметров
        */
        threads.emplace_back(sort_part, std::ref(arr), start, end);
    }

    // Ожидание завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Последовательное слияние отсортированных частей
    for (int merge_size = chunk_size; merge_size < size; merge_size *= 2) {
        for (int left = 0; left < size; left += 2 * merge_size) {
            int mid = std::min(left + merge_size, size);
            int right = std::min(left + 2 * merge_size, size);
            if (mid < right) {
                merge(arr, left, mid, mid, right);
            }
        }
    }
}