#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <iostream>
#define SIZE (1 << 11)

typedef float real_t;

void matmul(real_t *A, real_t *B, real_t *C, int M, int K, int N) {
    memset(C, 0, M * N * sizeof(real_t));
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            for (int j = 0; j < N; ++j) {
                C[i * N + j] += A[i * K + k] * B[k * N + j];
            }
        }
    }
}

int main() {
    const int M = SIZE, K = SIZE, N = SIZE;
    bool flag = false;
    real_t *A = (real_t*) aligned_alloc(64, M * K * sizeof(real_t));
    real_t *B = (real_t*) aligned_alloc(64, K * N * sizeof(real_t));
    real_t *C = (real_t*) aligned_alloc(64, M * N * sizeof(real_t));

    for (int i = 0; i < M * K; ++i) A[i] = 1.0;
    for (int i = 0; i < K * N; ++i) B[i] = 2.0;

    auto start = std::chrono::high_resolution_clock::now();
    matmul(A, B, C, M, K, N);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<real_t> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " s\n";

    for (int i = 0; i < M * N; ++i) {
        if (C[i] != (2.0) * SIZE) {
            flag = true;
            printf("%f\n", C[i]);
            break;
        }
    }
    printf(((flag)? "wrong answer!!\n": "correct answer\n"));
    free(A); free(B); free(C);
    return 0;
}