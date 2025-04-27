#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <immintrin.h>  // For AVX2 intrinsics
#include <numeric>      // For std::accumulate

// Using char instead of bool for better memory access performance
using BoolVector = std::vector<char>;

// Baseline version with branch prediction misses
void branch_predictor_test(const BoolVector& data) {
    volatile int sum = 0; // volatile prevents compiler optimization

    auto start = std::chrono::high_resolution_clock::now();

    // Unoptimized version with branch prediction
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i]) {  
            sum += 3;
        } else {
            sum += 1;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "[Baseline] Time: " << diff.count() << " sec\n";
}

// Optimized branchless version
void optimized_version(const BoolVector& data) {
    volatile int sum = 0;

    auto start = std::chrono::high_resolution_clock::now();

    // Branchless computation using arithmetic
    for (size_t i = 0; i < data.size(); ++i) {
        sum += 1 + 2 * static_cast<int>(data[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "[Optimized] Time: " << diff.count() << " sec\n";
}

// Advanced version with SIMD and parallel processing
void advanced_version(const BoolVector& data) {
    volatile int sum = 0;

    auto start = std::chrono::high_resolution_clock::now();

    // Using AVX2 for vectorized processing
    constexpr size_t SIMD_WIDTH = 32; // 256-bit AVX2 can process 32 bytes at once
    size_t i = 0;
    
    // Process data in SIMD chunks
    for (; i + SIMD_WIDTH <= data.size(); i += SIMD_WIDTH) {
        // Load 32 bytes into AVX register
        __m256i chunk = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(&data[i]));
        
        // Create mask (0xFF for true, 0x00 for false)
        __m256i mask = _mm256_cmpgt_epi8(chunk, _mm256_setzero_si256());
        
        // Convert mask to 0x01 for true (from 0xFF)
        __m256i ones = _mm256_and_si256(mask, _mm256_set1_epi8(1));
        
        // Sum the bits horizontally (8x 32-bit sums)
        __m256i sum_vec = _mm256_sad_epu8(ones, _mm256_setzero_si256());
        
        // Accumulate the partial sums
        alignas(32) int partial_sums[8];
        _mm256_store_si256(reinterpret_cast<__m256i*>(partial_sums), sum_vec);
        
        // 3 = 1 + 2*1, 1 = 1 + 2*0
        sum += 1 * SIMD_WIDTH + 2 * (partial_sums[0] + partial_sums[2] + 
                                     partial_sums[4] + partial_sums[6]);
    }

    // Process remaining elements
    for (; i < data.size(); ++i) {
        sum += 1 + 2 * static_cast<int>(data[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "[Advanced] Time: " << diff.count() << " sec\n";
}

int main() {
    const size_t size = 1e8; // 100 million elements
    BoolVector data(size);

    // High-quality random number generation
    std::random_device rd;
    std::mt19937_64 gen(rd()); // 64-bit Mersenne Twister
    std::uniform_int_distribution<int> dis(0, 1);

    // Parallel initialization with OpenMP
    #pragma omp parallel for
    for (size_t i = 0; i < size; ++i) {
        data[i] = dis(gen);
    }

    // Warm-up run (avoid cold cache effects)
    volatile int warmup = std::accumulate(data.begin(), data.end(), 0);
    (void)warmup; // Suppress unused warning

    // Performance comparison
    branch_predictor_test(data);
    optimized_version(data);
    advanced_version(data);

    return 0;
}