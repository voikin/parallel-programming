#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <chrono> // Для измерения времени
#include <cstdlib> // Для работы с аргументами командной строки
#include <omp.h> // Подключение OpenMP

void solve_system_omp(double* Matrix, double eps, int m, int n, int num_threads) {
    double dmax;
    do {
        dmax = 0.0;

        // Используем OpenMP для распараллеливания цикла
#pragma omp parallel for num_threads(num_threads) reduction(max : dmax)
        for (int i = m + 1; i < (n - 1) * m - 1; i++) {
            if ((i % m == 0) || (i % m == m - 1)) continue;

            double temp = Matrix[i];
            Matrix[i] = 0.25 * (Matrix[i - 1] + Matrix[i + 1] + Matrix[i - m] + Matrix[i + m]);
            double dm = fabs(temp - Matrix[i]);

            if (dmax < dm) dmax = dm;
        }
    } while (dmax > eps);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <grid_size> <epsilon> <num_threads>" << std::endl;
        return 1;
    }

    int n = std::atoi(argv[1]); // Размер сетки из аргументов
    double eps = std::atof(argv[2]); // Точность из аргументов
    int num_threads = std::atoi(argv[3]); // Число потоков

    // Инициализация матрицы
    std::vector<double> Matrix(n * n, 0.0);

    // Установка граничных условий
    for (int i = 0; i < n; i++) {
        Matrix[i] = 1.0;      // Верхняя граница
        Matrix[i * n] = 1.0;  // Левая граница
    }

    // Измерение времени выполнения
    auto start = std::chrono::high_resolution_clock::now();
    solve_system_omp(Matrix.data(), eps, n, n, num_threads);
    auto end = std::chrono::high_resolution_clock::now();

    // Расчет времени выполнения
    std::chrono::duration<double> elapsed = end - start;

    // Вывод времени выполнения
    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}