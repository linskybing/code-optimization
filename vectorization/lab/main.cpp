#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <immintrin.h>
#define TILE 32
#define min(a, b) ((a < b)? a : b)

using real_t = float;

void write_matrix(const std::string &filename, const real_t *data, size_t count, bool append=false) {
    std::ofstream ofs;
    if (append)
        ofs.open(filename, std::ios::binary | std::ios::app);
    else
        ofs.open(filename, std::ios::binary);

    if (!ofs) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        std::exit(1);
    }
    ofs.write(reinterpret_cast<const char*>(data), count * sizeof(real_t));
}


inline void matmul_avx2(real_t *A, real_t *B, real_t *C, int M, int K, int N) {
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            __m256 a_vec = _mm256_set1_ps(A[i * K + k]);
            for (int j = 0; j < N - 7; j += 8) {
                __m256 b_vec = _mm256_load_ps(&B[k * N + j]);
                __m256 c_vec = _mm256_load_ps(&C[i * N + j]);
                _mm256_store_ps(&C[i * N + j], _mm256_fmadd_ps(a_vec, b_vec, c_vec));
            }
        }
    }
}

void matrix_mult_avx2_optimized(real_t *A, real_t *B, real_t*C, int M, int K, int N) {
    for (int ii = 0; ii < M; ii += TILE) {
        for (int kk = 0; kk < N; kk += TILE) {
            for (int jj = 0; jj < K; jj += TILE) {
                int i_end = min(ii + TILE, M);
                int j_end = min(jj + TILE, K);
                int k_end = min(kk + TILE, N);
                for (int i = ii; i < i_end; i++) {
                    // TODO 1: Process 8 elements at a time (AVX2 handles 8 floats)
                    for (int j = jj; j < j_end; j += /* VLEN */) {
                        // TODO 2: Load C matrix values into AVX2 register
                        __m256 c = /* TODO */;

                        // Core computation: C[i][j:j+7] += A[i][k] * B[k][j:j+7]
                        for (int k = kk; k < k_end; k++) {
                            // TODO 3: Broadcast A[i][k] to all 8 positions
                            __m256 a = /* TODO */;

                            // TODO 4: Load B[k][j:j+7] into AVX2 register
                            __m256 b =  /* TODO */;

                            // TODO 5: Use fused multiply-add (FMA) operation
                            c = /* TODO */;
                        }
                        
                        // TODO 6: Store results back to C matrix
                        /* TODO */
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <M> <K> <N>" << std::endl;
        return 1;
    }

    int M = std::stoi(argv[1]);
    int K = std::stoi(argv[2]);
    int N = std::stoi(argv[3]);

    // Aligned allocations for better vectorization
    real_t *A = reinterpret_cast<real_t*>(aligned_alloc(32, M * K * sizeof(real_t)));
    real_t *B = reinterpret_cast<real_t*>(aligned_alloc(32, K * N * sizeof(real_t)));
    real_t *C = reinterpret_cast<real_t*>(aligned_alloc(32, M * N * sizeof(real_t)));

    memset(C, 0, M * N * sizeof(real_t));

    if (!A || !B || !C) {
        std::cerr << "Aligned allocation failed" << std::endl;
        return 1;
    }

    // Read data from files: A then B from input.bin
    std::ifstream ifs("input.bin", std::ios::binary);
    if (!ifs) {
        std::cerr << "Cannot open input.bin" << std::endl;
        return 1;
    }
    ifs.read(reinterpret_cast<char*>(A), static_cast<size_t>(M) * K * sizeof(real_t));
    ifs.read(reinterpret_cast<char*>(B), static_cast<size_t>(K) * N * sizeof(real_t));
    ifs.close();  // Close the file after reading

    // Matrix multiplication
    auto start = std::chrono::high_resolution_clock::now();
    
    matrix_mult_avx2_optimized(A, B, C, M, K, N);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " s\n";

    write_matrix("c.bin", C, static_cast<size_t>(M) * N, false);

    // Free allocated memory
    free(A);
    free(B);
    free(C);

    return 0;
}
