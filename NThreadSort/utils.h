#include <vector>
#include <thread>
#include <algorithm>
#include <cstring>
#include <cassert>

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

void test_parallel_sort() {
    // Тест 1: Пустой массив
    {
        std::vector<int> empty;
        parallel_sort(empty, 4);
        assert(empty.empty());
    }

    // Тест 2: Массив из одного элемента
    {
        std::vector<int> single = { 42 };
        parallel_sort(single, 4);
        assert(single.size() == 1 && single[0] == 42);
    }

    // Тест 3: Уже отсортированный массив
    {
        std::vector<int> sorted = { 1, 2, 3, 4, 5 };
        auto sorted_copy = sorted;
        parallel_sort(sorted, 2);
        assert(sorted == sorted_copy);
    }

    // Тест 4: Обратно отсортированный массив
    {
        std::vector<int> reversed = { 5, 4, 3, 2, 1 };
        parallel_sort(reversed, 2);
        assert(std::is_sorted(reversed.begin(), reversed.end()));
    }

    // Тест 5: Случайный массив (маленький)
    {
        std::vector<int> small = { 3, 1, 4, 1, 5, 9, 2, 6 };
        auto small_expected = small;
        parallel_sort(small, 2);
        std::sort(small_expected.begin(), small_expected.end());
        assert(small == small_expected);
    }

    // Тест 6: Случайный массив (большой)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 10000);

        std::vector<int> large(10000);
        for (auto& num : large) {
            num = dis(gen);
        }

        auto large_expected = large;
        parallel_sort(large, 4);
        std::sort(large_expected.begin(), large_expected.end());
        assert(large == large_expected);
    }


    std::cout << "Все тесты пройдены успешно!\n";
}