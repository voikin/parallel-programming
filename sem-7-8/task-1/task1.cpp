#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char **argv)
{
    int size = 1200000000; // Размер вектора
    int rank, num_procs;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Проверка корректности деления данных
    if (size % num_procs != 0)
    {
        if (rank == 0)
            cerr << "Ошибка: размер вектора (" << size << ") должен быть кратным числу процессов (" << num_procs << ")." << endl;
        MPI_Finalize();
        return -1;
    }

    start_time = MPI_Wtime();

    // Разделение данных
    int local_size = size / num_procs;
    vector<int> data;
    vector<int> local_data(local_size, 0);

    // Процесс 0 инициализирует данные
    if (rank == 0)
    {
        data.resize(size);
        for (int i = 0; i < size; i++)
            data[i] = 1; // Заполняем единицами
    }

    // Распределение данных по процессам
    MPI_Scatter(data.data(), local_size, MPI_INT, local_data.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Локальное суммирование
    int local_sum = 0;
    for (int i = 0; i < local_size; i++)
        local_sum += local_data[i];

    // Глобальное суммирование с использованием MPI_Reduce
    int global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    if (rank == 0)
    {
        cout << "Размер вектора: " << size << endl;
        cout << "Число процессов: " << num_procs << endl;
        cout << "Сумма элементов вектора: " << global_sum << endl;
        cout << "Время выполнения: " << end_time - start_time << " секунд" << endl << endl;
    }

    MPI_Finalize();
    return 0;
}