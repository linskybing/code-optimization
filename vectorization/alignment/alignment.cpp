#include <iostream>
#include <immintrin.h>
#include <chrono>
#include <random>

constexpr size_t ARRAY_SIZE = 1024 * 1024 * 32;
constexpr int ITERATIONS = 100;
constexpr int WARMUP = 10;

volatile float sink;

void aligned_access(float* src, float* dst) {
    for (size_t i = 0; i < ARRAY_SIZE; i += 8) {
        __m256 data = _mm256_load_ps(src + i);
        data = _mm256_mul_ps(data, data);
        _mm256_store_ps(dst + i, data);
    }
    sink = dst[ARRAY_SIZE-1];
}

void unaligned_access(float* src, float* dst) {
    for (size_t i = 1; i < ARRAY_SIZE - 8; i += 8) {
        __m256 data = _mm256_loadu_ps(src + i);
        data = _mm256_mul_ps(data, data);
        _mm256_storeu_ps(dst + i, data);
    }
    sink = dst[ARRAY_SIZE-1];
}

int main() {
    float* src_aligned = (float*)_mm_malloc(ARRAY_SIZE * sizeof(float), 32);
    float* dst_aligned = (float*)_mm_malloc(ARRAY_SIZE * sizeof(float), 32);
    
    float* src_unaligned = new float[ARRAY_SIZE + 8];
    float* dst_unaligned = new float[ARRAY_SIZE + 8];
    float* misaligned_src = src_unaligned + 1;
    float* misaligned_dst = dst_unaligned + 1;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(1.0f, 2.0f);
    
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        src_aligned[i] = dist(gen);
        misaligned_src[i + 1] = src_aligned[i];
    }
    
    for (int i = 0; i < WARMUP; ++i) {
        aligned_access(src_aligned, dst_aligned);
        unaligned_access(misaligned_src, misaligned_dst);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        aligned_access(src_aligned, dst_aligned);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> aligned_time = end - start;
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        unaligned_access(misaligned_src, misaligned_dst);
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> unaligned_time = end - start;
    
    std::cout << "Array size: " << ARRAY_SIZE << " floats (" 
              << (ARRAY_SIZE * sizeof(float) / (1024 * 1024)) << " MB)\n";
    std::cout << "Aligned time: " << aligned_time.count() << "s\n";
    std::cout << "Unaligned time: " << unaligned_time.count() << "s\n";
    std::cout << "Performance difference: " << (unaligned_time.count() / aligned_time.count()) << "x\n";
    
    _mm_free(src_aligned);
    _mm_free(dst_aligned);
    delete[] src_unaligned;
    delete[] dst_unaligned;
    
    return 0;
}