#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <random>

class ParallelSorter {
public:
    // Конструктор: инициализирует количество потоков и подзадач
    ParallelSorter(int numThreads, int numSubtasks) : numThreads_(numThreads), numSubtasks_(numSubtasks) {}

    // Основной метод сортировки
    std::vector<int> sort(std::vector<int>& data) {
        // Копируем входные данные в член класса для доступа из других методов
        data_ = data;
        // Сохраняем размер данных
        size_ = data_.size();
        // Вычисляем размер каждой подзадачи
        subtaskSize_ = size_ / numSubtasks_;


        // Создаем и запускаем потоки
        threads_.reserve(numThreads_);
        for (int i = 0; i < numThreads_; ++i) {
            // Каждый поток выполняет метод sortSubtasks
            threads_.emplace_back(&ParallelSorter::sortSubtasks, this, i);
        }

        // Ожидаем завершения всех потоков
        for (auto& thread : threads_) {
            thread.join();
        }

        // Сливаем отсортированные подзадачи в один результирующий массив
        mergeSubtasks();

        // Возвращаем отсортированный массив
        return data_;
    }


private:
    // Метод, выполняемый каждым потоком
    void sortSubtasks(int threadId) {
        // Вычисляем начальный и конечный индексы подзадач для текущего потока
        int startTask = threadId * (numSubtasks_ / numThreads_);
        int endTask = (threadId + 1) * (numSubtasks_ / numThreads_);
        // Корректируем конец для последнего потока, чтобы он обработал все оставшиеся подзадачи
        if (threadId == numThreads_ - 1) {
            endTask = numSubtasks_;
        }

        // Сортируем каждую подзадачу, назначенную текущему потоку
        for (int i = startTask; i < endTask; ++i) {
            int start = i * subtaskSize_;
            int end = (i + 1) * subtaskSize_;
            // Корректируем конец для последней подзадачи, чтобы обработать все оставшиеся элементы
            if (i == numSubtasks_ - 1) {
                end = size_;
            }
            // Используем std::sort для сортировки подмассива
            std::sort(data_.begin() + start, data_.begin() + end);
        }
    }

    // Метод для слияния отсортированных подзадач
    void mergeSubtasks() {
        // Создаем результирующий массив
        std::vector<int> merged;
        merged.reserve(size_);

        // Массив индексов для отслеживания текущей позиции в каждой подзадаче
        std::vector<int> indices(numSubtasks_, 0);

        // Проходим по всем элементам исходного массива
        for (int i = 0; i < size_; ++i) {
            // Находим минимальный элемент среди текущих элементов всех подзадач
            int minVal = INT_MAX;
            int minIndex = -1;
            int subtaskIndex = -1;


            for (int j = 0; j < numSubtasks_; ++j) {
                int start = j * subtaskSize_;
                int end = (j + 1) * subtaskSize_;
                if (j == numSubtasks_ - 1) {
                    end = size_;
                }
                // Проверяем, не достигли ли конца текущей подзадачи
                if (indices[j] < end - start) {
                    int currentVal = data_[start + indices[j]];
                    if (currentVal < minVal) {
                        minVal = currentVal;
                        minIndex = indices[j];
                        subtaskIndex = j;
                    }
                }
            }

            // Добавляем минимальный элемент в результирующий массив
            if (subtaskIndex != -1) {
                merged.push_back(minVal);
                indices[subtaskIndex]++; // Переходим к следующему элементу в выбранной подзадаче
            }
        }

        // Заменяем исходный массив отсортированным
        data_ = merged;
    }


    // Количество потоков
    int numThreads_;
    // Количество подзадач
    int numSubtasks_;
    // Массив данных для сортировки
    std::vector<int> data_;
    // Размер массива данных
    size_t size_;
    // Размер каждой подзадачи
    int subtaskSize_;
    // Вектор потоков
    std::vector<std::thread> threads_;
};