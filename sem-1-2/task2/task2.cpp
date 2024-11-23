#include <iostream>
#include <omp.h>
#include <ctime>

#define N 110000000

double a[N + 1], b[N + 1];

int main()
{
    // Инициализация векторов
    for (int i = 0; i < N; i++)
    {
        a[i] = 1.034;
        b[i] = 1.057;
    }

    double gDotProduct = 0;
    double sequentialTime, parallelTime;

    // Последовательное вычисление
    {
        auto start = omp_get_wtime(); // Засекаем время
        for (int i = 0; i < N; i++)
        {
            gDotProduct += a[i] * b[i];
        }
        auto stop = omp_get_wtime(); // Конец измерения времени
        sequentialTime = stop - start;
        std::cout << "Последовательное выполнение:" << std::endl;
        std::cout << "Сумма = " << gDotProduct << std::endl;
        std::cout << "Время = " << sequentialTime << " секунд" << std::endl;
    }

    // Обнуление результата для параллельного вычисления
    gDotProduct = 0;

    // Параллельное вычисление
    {
        auto start = omp_get_wtime(); // Засекаем время
#pragma omp parallel for reduction(+ : gDotProduct)
        for (int i = 0; i < N; i++)
        {
            gDotProduct += a[i] * b[i];
        }
        auto stop = omp_get_wtime(); // Конец измерения времени
        parallelTime = stop - start;
        std::cout << "\nПараллельное выполнение:" << std::endl;
        std::cout << "Сумма = " << gDotProduct << std::endl;
        std::cout << "Время = " << parallelTime << " секунд" << std::endl;
    }

    // Сравнение времени
    std::cout << "\nУскорение: " << sequentialTime / parallelTime << " раз" << std::endl;

    return 0;
}
