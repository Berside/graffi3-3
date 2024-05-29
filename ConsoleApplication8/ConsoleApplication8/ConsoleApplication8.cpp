#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
using namespace std;

struct SparseMatrix {
    vector<int> row_indices;
    vector<int> col_ptrs;
    vector<int> values;
};


vector<int> readBandMatrix(const string& filename, int& n, int& bandwidth) {
    ifstream infile(filename);
    infile >> n >> bandwidth;
    vector<int> bandMatrix(n * bandwidth, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < bandwidth; ++j) {
            infile >> bandMatrix[i * bandwidth + j];
        }
    }

    infile.close();
    return bandMatrix;
}
SparseMatrix multiplyBandMatricesAndConvertToSparse(const vector<int>& bandMatrix1, const vector<int>& bandMatrix2, int n, int bandwidth) {
    SparseMatrix sparse;
    sparse.col_ptrs.push_back(0);
    int halfBandwidth = (bandwidth - 1) / 2;
    for (int j = 0; j < n; ++j) {
        for (int i = max(0, j - halfBandwidth); i <= min(n - 1, j + halfBandwidth); ++i) {
            int bandIndex = i - j + halfBandwidth;
            int productValue = bandMatrix1[j * bandwidth + bandIndex] * bandMatrix2[i * bandwidth + bandIndex]; 
            if (productValue != 0) { 
                sparse.row_indices.push_back(i);
                sparse.values.push_back(productValue);
            }
        }
        sparse.col_ptrs.push_back(sparse.row_indices.size());
    }
    return sparse;
}

vector<int> generateRandomBandMatrix(int N, int bandwidth, float zeroPercentage) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 9); 
    vector<int> bandMatrix(N * bandwidth, 0); 
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < bandwidth; ++j) {
            float randZeroOrOne = static_cast<float>(dis(gen)) / 10.0f; 
            bool isZero = (randZeroOrOne < zeroPercentage);
            bandMatrix[i * bandwidth + j] = isZero ? 0 : dis(gen);
        }
    }
    return bandMatrix;
}

void writeSparseMatrix(const string& filename, const SparseMatrix& sparse) {
    ofstream outfile(filename);
    outfile << sparse.row_indices.size() << " " << sparse.col_ptrs.size() - 1 << " " << sparse.values.size() << endl;
    for (const auto& val : sparse.values) {
        outfile << val << " ";
    }
    outfile << endl;
    for (const auto& idx : sparse.row_indices) {
        outfile << idx << " ";
    }
    outfile << endl;
    for (const auto& ptr : sparse.col_ptrs) {
        outfile << ptr << " ";
    }
    outfile << endl;

    outfile.close();
}

void printBandMatrix(const vector<int>& bandMatrix, int N, int bandwidth, float zeroPercentage) {
    cout << "Размерность матрицы: " << N << " x " << bandwidth << endl;
    cout << "Процент нулей: " << fixed << setprecision(2) << zeroPercentage * 100 << "%" << endl;
    cout << "Ленточная матрица:" << endl;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < bandwidth; ++j) {
            int value = bandMatrix[i * bandwidth + j];
            cout << setw(6) << value; 
        }
        cout << endl;
    }
}


int main() {
    setlocale(LC_ALL, "Russian");
    string inputFilename1 = "123.txt";
    string inputFilename2 = "321.txt";
    string outputFilename = "CCS.txt";
    int n1, n2, bandwidth1, bandwidth2, k;
    cout << "Выберите способ запуск программы:" << endl;
    cout << "Если вы хотите умножить разреженные матрицы используя входные данные. Напишите: 1" << endl;
    cout << "Если вы хотите случайно сгенерировать 2 разреженные матрицы используя случайную генерацию. Напишите: 2" << endl;
    cin >> k;
    if (k == 1) {
        vector<int> bandMatrix1 = readBandMatrix(inputFilename1, n1, bandwidth1);
        vector<int> bandMatrix2 = readBandMatrix(inputFilename2, n2, bandwidth2);
        if (n1 != n2 || bandwidth1 != bandwidth2) {
            cout << "Матрицы должны быть одинакового размера и иметь одинаковую ширину ленты." << endl;
            return 1;
        }
        auto start1 = chrono::high_resolution_clock::now();
        SparseMatrix sparseMatrix = multiplyBandMatricesAndConvertToSparse(bandMatrix1, bandMatrix2, n1, bandwidth1);
        auto end1 = chrono::high_resolution_clock::now();
        writeSparseMatrix(outputFilename, sparseMatrix);
        cout << "Преобразование завершено. Результирующая матрица записана в " << outputFilename << endl;
        auto duration1 = chrono::duration_cast<chrono::microseconds>(end1 - start1).count();
        cout << "Время выполения алгоритма: " << duration1 << " microseconds" << endl;
    }
    if (k == 2) {
        cout << "Введите размерность матриц" << endl;
        int N;
        cin >> N;
        cout << "Введите процент нулей в матрицах (например: 0.2)" << endl;
        float zeroPercentage; 
        cin >> zeroPercentage;
        cout << "Введите желаемую ширину ленты" << endl;
        int width;
        cin >> width;
        cout << endl;
        vector<int> bandMatrix1 = generateRandomBandMatrix(N, N, zeroPercentage);
        vector<int> bandMatrix2 = generateRandomBandMatrix(N, N, zeroPercentage);
        if (N <= 6) {
            printBandMatrix(bandMatrix1, N, N, zeroPercentage);
            cout << endl;
            printBandMatrix(bandMatrix2, N, N, zeroPercentage);
        }
        auto start1 = chrono::high_resolution_clock::now();
        SparseMatrix sparseMatrix = multiplyBandMatricesAndConvertToSparse(bandMatrix1, bandMatrix2, N, N);
        auto end1 = chrono::high_resolution_clock::now();
        writeSparseMatrix("generate.txt", sparseMatrix);
        cout << "Результат записан в файл: generate.txt" << endl;
        auto duration1 = chrono::duration_cast<chrono::microseconds>(end1 - start1).count();
        cout << "Время выполения алгоритма: " << duration1 << " microseconds" << endl;
        cout << "Новые случайно сгенерированные матрицы были записаны в файлы: Generatematrix1 ; Generatematrix2.  " << endl;
    }
    return 0;
}
