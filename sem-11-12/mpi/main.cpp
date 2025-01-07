#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <mpi.h>
#include <cstdlib>

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
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " <array_size>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    int array_size = std::atoi(argv[1]);

    std::vector<int> array;
    if (rank == 0) {
        array.resize(array_size);
        std::generate(array.begin(), array.end(), std::rand);
    }

    int local_size = array_size / size;
    std::vector<int> local_array(local_size);

    MPI_Scatter(array.data(), local_size, MPI_INT, local_array.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

    auto start = std::chrono::high_resolution_clock::now();
    merge_sort(local_array, 0, local_size - 1);
    auto end = std::chrono::high_resolution_clock::now();

    MPI_Gather(local_array.data(), local_size, MPI_INT, array.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 1; i < size; ++i) {
            merge(array, 0, i * local_size - 1, (i + 1) * local_size - 1);
        }

        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}