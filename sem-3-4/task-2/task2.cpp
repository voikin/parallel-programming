#include <iostream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <cstdlib>

// Блочное умножение матриц
void block_matrix_multiplication(const std::vector<std::vector<double>> &A,
                                 const std::vector<std::vector<double>> &B,
                                 std::vector<std::vector<double>> &C,
                                 int block_size, int threads) {
    int n = A.size();

    #pragma omp parallel for collapse(2) num_threads(threads)
    for (int i = 0; i < n; i += block_size) {
        for (int j = 0; j < n; j += block_size) {
            for (int k = 0; k < n; k += block_size) {
                // Умножение блоков
                for (int bi = i; bi < std::min(i + block_size, n); bi++) {
                    for (int bj = j; bj < std::min(j + block_size, n); bj++) {
                        for (int bk = k; bk < std::min(k + block_size, n); bk++) {
                            C[bi][bj] += A[bi][bk] * B[bk][bj];
                        }
                    }
                }
            }
        }
    }
}

int main() {
    std::vector<int> matrix_sizes = {1000, 2000, 4000, 8000};
    std::vector<int> threads_count = {2, 4, 8};
    std::vector<int> block_sizes = {32, 64, 128}; // Примеры размеров блоков

    // Вывод шапки таблицы
    std::cout << std::setw(15) << "Размер матрицы"
              << std::setw(15) << "Потоки"
              << std::setw(15) << "Размер блока"
              << std::setw(25) << "Время вычисления (сек)"
              << std::endl;

    for (int size : matrix_sizes) {
        for (int block_size : block_sizes) {
            // Создание матриц
            std::vector<std::vector<double>> A(size, std::vector<double>(size));
            std::vector<std::vector<double>> B(size, std::vector<double>(size));
            std::vector<std::vector<double>> C(size, std::vector<double>(size, 0.0));

            // Заполнение матриц
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    A[i][j] = static_cast<double>(rand()) / RAND_MAX;
                    B[i][j] = static_cast<double>(rand()) / RAND_MAX;
                }
            }

            for (int threads : threads_count) {
                // Измерение времени
                double start_time = omp_get_wtime();
                block_matrix_multiplication(A, B, C, block_size, threads);
                double end_time = omp_get_wtime();

                // Вывод результата
                std::cout << std::setw(15) << size
                          << std::setw(15) << threads
                          << std::setw(15) << block_size
                          << std::setw(25) << (end_time - start_time)
                          << std::endl;
            }
        }
    }

    return 0;
}
