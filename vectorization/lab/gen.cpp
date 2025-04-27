#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>       // for aligned_alloc, free
#include <chrono>
using real_t = float;   // change to double if needed

// Write a binary float array to a file
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

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <M> <K> <N>" << std::endl;
        std::cerr << "  Generate A(MxK), B(KxN), compute C=A*B, output input.bin and ans.bin" << std::endl;
        return 1;
    }

    int M = std::stoi(argv[1]);
    int K = std::stoi(argv[2]);
    int N = std::stoi(argv[3]);

    // Aligned allocations for better vectorization
    real_t *A = reinterpret_cast<real_t*>(aligned_alloc(32, M * K * sizeof(real_t)));
    real_t *B = reinterpret_cast<real_t*>(aligned_alloc(32, K * N * sizeof(real_t)));
    real_t *C = reinterpret_cast<real_t*>(aligned_alloc(32, M * N * sizeof(real_t)));
    if (!A || !B || !C) {
        std::cerr << "Aligned allocation failed" << std::endl;
        return 1;
    }

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<real_t> dist(0.0f, 1.0f);

    // Initialize A, B and zero C
    for (size_t i = 0, count = M * K; i < count; ++i) A[i] = dist(rng);
    for (size_t i = 0, count = K * N; i < count; ++i) B[i] = dist(rng);
    std::fill_n(C, M * N, static_cast<real_t>(0));

    // Compute C = A * B
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            real_t aik = A[static_cast<size_t>(i) * K + k];
            for (int j = 0; j < N; ++j) {
                C[static_cast<size_t>(i) * N + j] += aik * B[static_cast<size_t>(k) * N + j];
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " s\n";

    // Write data to files: A then B into input.bin, C into ans.bin
    write_matrix("input.bin", A, static_cast<size_t>(M) * K, false);
    write_matrix("input.bin", B, static_cast<size_t>(K) * N, true);
    write_matrix("ans.bin", C, static_cast<size_t>(M) * N, false);

    std::cout << "Generated input.bin (A then B) and ans.bin (C) for A("
              << M << " x " << K << ") * B(" << K << " x " << N << ")" << std::endl;

    free(A);
    free(B);
    free(C);
    return 0;
}