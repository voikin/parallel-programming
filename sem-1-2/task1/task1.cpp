#include <iostream>
#include <omp.h>

using namespace std;

// Функция для проверки, является ли число простым
bool is_prime(int n)
{
    if (n < 2)
        return false;
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
            return false;
    }
    return true;
}

int main()
{
    int num_threads = 6; // Количество потоков
    long long sum = 0;   // Для хранения суммы простых чисел
    double wtime;        // Для измерения времени выполнения

#ifdef _OPENMP
    cout << "Поддержка OpenMP включена" << endl;
#else
    cout << "OpenMP не поддерживается" << endl;
#endif

    // Установим количество потоков
    omp_set_num_threads(num_threads);

    // Засекаем время
    wtime = omp_get_wtime();

    // Параллельный блок для вычисления суммы простых чисел
#pragma omp parallel for reduction(+ : sum)
    for (int i = 2; i <= 100000; i++)
    {
        if (is_prime(i))
        {
            sum += i;
        }
    }

    // Вычисляем общее время выполнения
    wtime = omp_get_wtime() - wtime;

    cout << "Сумма простых чисел в диапазоне от 2 до 100000: " << sum << endl;
    cout << "Количество потоков: " << num_threads << endl;
    cout << "Время выполнения: " << wtime << " секунд" << endl;

    return 0;
}
