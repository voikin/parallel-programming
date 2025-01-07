#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdlib> // Для работы с аргументами командной строки

// Функция для слияния двух отсортированных подмассивов
void merge(std::vector<int>& array, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = array[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = array[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            array[k] = L[i];
            i++;
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
    }
}

// Последовательная сортировка слиянием
void merge_sort(std::vector<int>& array, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        merge_sort(array, left, mid);
        merge_sort(array, mid + 1, right);

        merge(array, left, mid, right);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <array_size> <epsilon> <num_threads>" << std::endl;
        return 1;
    }

    int size = std::atoi(argv[1]); // Размер массива из аргументов

    std::vector<int> array(size);
    std::generate(array.begin(), array.end(), std::rand);

    // Измерение времени выполнения
    auto start = std::chrono::high_resolution_clock::now();
    merge_sort(array, 0, size - 1);
    auto end = std::chrono::high_resolution_clock::now();

    // Расчет времени выполнения
    std::chrono::duration<double> elapsed = end - start;

    // Вывод времени выполнения
    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}