#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>

void solve_system_mpi(std::vector<double>& local_matrix, int m, double eps, int rank, int size, int rows_per_proc) {
    double dmax;
    do {
        dmax = 0.0;
        for (int i = 1; i < rows_per_proc - 1; i++) {
            for (int j = 1; j < m - 1; j++) {
                int idx = i * m + j;
                double temp = local_matrix[idx];
                local_matrix[idx] = 0.25 * (local_matrix[idx - 1] + local_matrix[idx + 1] +
                                            local_matrix[idx - m] + local_matrix[idx + m]);
                double dm = fabs(temp - local_matrix[idx]);
                if (dm > dmax) dmax = dm;
            }
        }

        // Обмен граничными строками
        if (rank > 0) {
            MPI_Sendrecv(local_matrix.data() + m, m, MPI_DOUBLE, rank - 1, 0,
                         local_matrix.data(), m, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1) {
            MPI_Sendrecv(local_matrix.data() + (rows_per_proc - 2) * m, m, MPI_DOUBLE, rank + 1, 0,
                         local_matrix.data() + (rows_per_proc - 1) * m, m, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Сбор максимального изменения
        double global_dmax;
        MPI_Allreduce(&dmax, &global_dmax, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        dmax = global_dmax;
    } while (dmax > eps);
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " <grid_size> <epsilon>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    int n = std::atoi(argv[1]);
    double eps = std::atof(argv[2]);
    int m = n;

    // Число строк на процесс, включая границы
    int rows_per_proc = n / size + 2;
    std::vector<double> local_matrix(rows_per_proc * m, 0.0);

    // Матрица для главного процесса
    std::vector<double> global_matrix;
    if (rank == 0) {
        global_matrix.resize(n * m, 0.0);
        for (int i = 0; i < n; i++) {
            global_matrix[i] = 1.0;     // Верхняя граница
            global_matrix[i * m] = 1.0; // Левая граница
        }
    }

    // Распределение матрицы по процессам
    MPI_Scatter(global_matrix.data(), (n / size) * m, MPI_DOUBLE,
                local_matrix.data() + m, (n / size) * m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();
    solve_system_mpi(local_matrix, m, eps, rank, size, rows_per_proc);
    double end_time = MPI_Wtime();

    // Сбор данных в главном процессе
    MPI_Gather(local_matrix.data() + m, (n / size) * m, MPI_DOUBLE,
               global_matrix.data(), (n / size) * m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Execution time: " << (end_time - start_time) << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}