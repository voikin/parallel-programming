#include <iostream>
#include <omp.h>
#include <cmath>
#include <vector>

using namespace std;

int main()
{
    const double tolerance = 1e-9;              // Точность (0.000000001)
    vector<int> max_threads = {1, 2, 4, 8, 16}; // Число потоков для тестов
    double start, end;

    cout << "Число потоков\tВремя выполнения (секунды)" << endl;

    for (int t : max_threads)
    {
        omp_set_num_threads(t); // Устанавливаем число потоков

        double pi_parallel = 0.0; // Обнуляем значение
        double partial_sum = 0.0; // Частичная сумма для каждого потока
        int iterations = 1e9;     // Ограничение на итерации

        start = omp_get_wtime(); // Начало измерения времени

#pragma omp parallel for reduction(+ : pi_parallel)
        for (int i = 0; i < iterations; ++i)
        {
            double term = pow(-1, i) / (2.0 * i + 1.0); // Член ряда Лейбница
            pi_parallel += term;
        }

        end = omp_get_wtime(); // Конец измерения времени

        cout << t << "\t\t" << (end - start) << " сек." << endl;
    }

    return 0;
}
