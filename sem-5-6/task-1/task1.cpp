#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <random>

using namespace std;
using namespace chrono;

// Функция для последовательного решения методом Гаусса
vector<double> SequentialGaussianElimination(vector<vector<double>> coefficients, vector<double> constants) {
    int n = constants.size();

    // Прямой ход
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double factor = coefficients[j][i] / coefficients[i][i];
            for (int k = i; k < n; k++) {
                coefficients[j][k] -= factor * coefficients[i][k];
            }
            constants[j] -= factor * constants[i];
        }
    }

    // Обратный ход
    vector<double> solutions(n);
    for (int i = n - 1; i >= 0; i--) {
        solutions[i] = constants[i];
        for (int j = i + 1; j < n; j++) {
            solutions[i] -= coefficients[i][j] * solutions[j];
        }
        solutions[i] /= coefficients[i][i];
    }
    return solutions;
}

// Функция для параллельного решения методом Гаусса
vector<double> ParallelGaussianElimination(vector<vector<double>> coefficients, vector<double> constants) {
    int n = constants.size();

    // Прямой ход с использованием OpenMP
    for (int i = 0; i < n; i++) {
        double divisor = coefficients[i][i];

#pragma omp parallel for
        for (int j = i + 1; j < n; j++) {
            double factor = coefficients[j][i] / divisor;
            for (int k = i; k < n; k++) {
                coefficients[j][k] -= factor * coefficients[i][k];
            }
            constants[j] -= factor * constants[i];
        }
    }

    // Обратный ход
    vector<double> solutions(n);
    for (int i = n - 1; i >= 0; i--) {
        solutions[i] = constants[i];
        for (int j = i + 1; j < n; j++) {
            solutions[i] -= coefficients[i][j] * solutions[j];
        }
        solutions[i] /= coefficients[i][i];
    }
    return solutions;
}

// Функция для генерации случайной матрицы и вектора
void GenerateRandomMatrixAndVector(vector<vector<double>>& coefficients, vector<double>& constants, int size) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(1.0, 10.0);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            coefficients[i][j] = dis(gen);
        }
        constants[i] = dis(gen);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Пример уравнений
    vector<vector<double>> coefficients = {
        {3, -4},
        {2, 5}
    };
    vector<double> constants = { -11, 8 };

    // Последовательное решение
    cout << "Последовательное решение:" << endl;
    auto start = high_resolution_clock::now();
    vector<double> seqSolutions = SequentialGaussianElimination(coefficients, constants);
    auto end = high_resolution_clock::now();
    cout << "Корни: ";
    for (double x : seqSolutions) cout << x << " ";
    cout << "\nВремя: " << duration_cast<milliseconds>(end - start).count() << " мс\n";

    // Параллельное решение
    cout << "\nПараллельное решение:" << endl;
    coefficients = { {3, -4}, {2, 5} };
    constants = { -11, 8 };
    start = high_resolution_clock::now();
    vector<double> parSolutions = ParallelGaussianElimination(coefficients, constants);
    end = high_resolution_clock::now();
    cout << "Корни: ";
    for (double x : parSolutions) cout << x << " ";
    cout << "\nВремя: " << duration_cast<milliseconds>(end - start).count() << " мс\n";

    // Оценка производительности на больших матрицах
    vector<int> sizes = { 1000, 3000 };
    for (int size : sizes) {
        vector<vector<double>> randomCoefficients(size, vector<double>(size));
        vector<double> randomConstants(size);
        GenerateRandomMatrixAndVector(randomCoefficients, randomConstants, size);

        // Последовательное выполнение
        start = high_resolution_clock::now();
        SequentialGaussianElimination(randomCoefficients, randomConstants);
        end = high_resolution_clock::now();
        double seqTime = duration_cast<milliseconds>(end - start).count();

        // Параллельное выполнение
        start = high_resolution_clock::now();
        ParallelGaussianElimination(randomCoefficients, randomConstants);
        end = high_resolution_clock::now();
        double parTime = duration_cast<milliseconds>(end - start).count();

        // Расчет ускорения и эффективности
        double speedup = seqTime / parTime;
        double efficiency = speedup / omp_get_max_threads();

        cout << "\nРазмер матрицы: " << size << "x" << size;
        cout << "\nПоследовательное время: " << seqTime << " мс";
        cout << "\nПараллельное время: " << parTime << " мс";
        cout << "\nУскорение: " << speedup;
        cout << "\nЭффективность: " << efficiency << "\n";
    }

    return 0;
}