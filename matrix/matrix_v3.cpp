#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <iostream>
#define SIZE (1 << 11)
#define TILE 32

typedef float real_t;

void matmul_tile(real_t *A, real_t *B, real_t *C, int M, int K, int N) {
    memset(C, 0, M * N * sizeof(real_t));

    for (int block_j = 0; block_j < N; block_j += TILE) {
        for (int block_i = 0; block_i < M; block_i += TILE) {
            for (int block_k = 0; block_k < K; block_k += TILE) {
                for (int i = block_i; i < (block_i + TILE); ++i) {
                    for (int k = block_k; k < (block_k + TILE); ++k) {
                        for (int j = block_j; j < (block_j + TILE); ++j) {
                            C[i * N + j] += A[i * K + k] * B[k * N + j];
                        }
                    }
                }
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
    matmul_tile(A, B, C, M, K, N);
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