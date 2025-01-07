#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Функция для умножения матрицы на вектор
void matrixVectorProduct(int rank, int size, int n)
{
	vector<vector<int>> matrix(n, vector<int>(n));
	vector<int> vec(n), result(n, 0), local_result(n / size, 0);

	// Генерация данных на главном процессе
	if (rank == 0)
	{
		srand(time(nullptr));
		for (int i = 0; i < n; i++)
		{
			vec[i] = rand() % 10;
			for (int j = 0; j < n; j++)
			{
				matrix[i][j] = rand() % 10;
			}
		}
	}

	// Распределение данных
	vector<int> local_matrix((n / size) * n);
	MPI_Scatter(matrix.data()->data(), (n / size) * n, MPI_INT, local_matrix.data(), (n / size) * n, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(vec.data(), n, MPI_INT, 0, MPI_COMM_WORLD);

	// Локальное умножение матрицы на вектор
	for (int i = 0; i < n / size; i++)
	{
		for (int j = 0; j < n; j++)
		{
			local_result[i] += local_matrix[i * n + j] * vec[j];
		}
	}

	// Сбор результата
	MPI_Gather(local_result.data(), n / size, MPI_INT, result.data(), n / size, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		cout << "Произведение матрицы на вектор вычислено." << endl;
	}
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int rank, size, n;
	n = 1000;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	double start_time, end_time;

	start_time = MPI_Wtime();
	vector<vector<int>> matrix(n, vector<int>(n));
	vector<int> vec(n), result(n, 0), local_result(n / size, 0);

	// Генерация данных на главном процессе
	if (rank == 0)
	{
		srand(time(nullptr));
		for (int i = 0; i < n; i++)
		{
			vec[i] = rand() % 10;
			for (int j = 0; j < n; j++)
			{
				matrix[i][j] = rand() % 10;
			}
		}
	}

	// Распределение данных
	vector<int> local_matrix((n / size) * n);
	MPI_Scatter(matrix.data()->data(), (n / size) * n, MPI_INT, local_matrix.data(), (n / size) * n, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(vec.data(), n, MPI_INT, 0, MPI_COMM_WORLD);

	// Локальное умножение матрицы на вектор
	for (int i = 0; i < n / size; i++)
	{
		for (int j = 0; j < n; j++)
		{
			local_result[i] += local_matrix[i * n + j] * vec[j];
		}
	}

	// Сбор результата
	MPI_Gather(local_result.data(), n / size, MPI_INT, result.data(), n / size, MPI_INT, 0, MPI_COMM_WORLD);

	end_time = MPI_Wtime();
	if (rank == 0)
		cout << end_time - start_time << endl;

	MPI_Finalize();
	return 0;
}