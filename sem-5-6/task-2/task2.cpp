#include <iostream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <cmath>

using namespace std;

// Допустимая погрешность сравнения
const double EPSILON = 1e-9;

// Функция для LU-разложения с использованием OpenMP
void LUDecompose(const vector<vector<double>>& A, vector<vector<double>>& L, vector<vector<double>>& U) {
    int n = A.size();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            U[i][j] = A[i][j];
            L[i][j] = (i == j) ? 1 : 0;
        }
    }

    for (int k = 0; k < n; k++) {
#pragma omp parallel for
        for (int i = k + 1; i < n; i++) {
            L[i][k] = U[i][k] / U[k][k];
            for (int j = k; j < n; j++) {
                U[i][j] -= L[i][k] * U[k][j];
            }
        }
    }
}

// Функция для перемножения матриц L и U
vector<vector<double>> MultiplyMatrices(const vector<vector<double>>& L, const vector<vector<double>>& U) {
    int n = L.size();
    vector<vector<double>> result(n, vector<double>(n, 0.0));

#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += L[i][k] * U[k][j];
            }
        }
    }

    return result;
}

// Функция для сравнения двух матриц
bool CompareMatrices(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int n = A.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (fabs(A[i][j] - B[i][j]) > EPSILON) {
                return false;
            }
        }
    }
    return true;
}

// Функция для вывода матрицы
void PrintMatrix(const vector<vector<double>>& matrix, const string& name) {
    cout << "\n" << name << ":\n";
    for (const auto& row : matrix) {
        for (double val : row) {
            cout << fixed << setprecision(2) << val << " ";
        }
        cout << endl;
    }
}

// Функция для решения нижнетреугольной системы
vector<double> SolveLower(const vector<vector<double>>& L, const vector<double>& b) {
    int n = b.size();
    vector<double> y(n);

    for (int i = 0; i < n; i++) {
        y[i] = b[i];
        for (int j = 0; j < i; j++) {
            y[i] -= L[i][j] * y[j];
        }
    }

    return y;
}

// Функция для решения верхнетреугольной системы
vector<double> SolveUpper(const vector<vector<double>>& U, const vector<double>& y) {
    int n = y.size();
    vector<double> x(n);

    for (int i = n - 1; i >= 0; i--) {
        x[i] = y[i];
        for (int j = i + 1; j < n; j++) {
            x[i] -= U[i][j] * x[j];
        }
        x[i] /= U[i][i];
    }

    return x;
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Исходная матрица A и вектор b
    vector<vector<double>> A = {
        {4, -2, 1},
        {20, -7, 12},
        {-8, 13, 17}
    };
    vector<double> b = {15, 57, 24};

    int n = A.size();
    vector<vector<double>> L(n, vector<double>(n, 0.0));
    vector<vector<double>> U(n, vector<double>(n, 0.0));

    // LU-разложение
    LUDecompose(A, L, U);
    PrintMatrix(L, "Матрица L");
    PrintMatrix(U, "Матрица U");

    // Проверка A ≈ L * U
    vector<vector<double>> LU = MultiplyMatrices(L, U);
    PrintMatrix(LU, "Результат L * U");
    if (CompareMatrices(A, LU)) {
        cout << "\nLU-разложение выполнено корректно!\n";
    } else {
        cout << "\nОшибка в LU-разложении!\n";
    }

    // Решение системы
    vector<double> y = SolveLower(L, b);
    vector<double> x = SolveUpper(U, y);

    // Вывод корней
    cout << "\nКорни системы: ";
    for (double xi : x) {
        cout << fixed << setprecision(2) << xi << " ";
    }
    cout << endl;

    return 0;
}