#include <iostream>
#include <omp.h>

constexpr int THREADS = 4;
constexpr int ITERATIONS = 1E8;
constexpr int CACHELINE = 64;
struct FalseSharingData {
    int a, b, c, d;
};

struct NoFalseSharingData {
    int a;
    char padding1[CACHELINE - sizeof(int)];
    int b;
    char padding2[CACHELINE - sizeof(int)];
    int c;
    char padding3[CACHELINE - sizeof(int)];
    int d;
};

void benchmark_false_sharing() {
    FalseSharingData data = {0};
    double start = omp_get_wtime();

    #pragma omp parallel num_threads(THREADS)
    {
        int tid = omp_get_thread_num();
        for (int i = 0; i < ITERATIONS; i++) {
            if (tid == 0) data.a++;
            else if (tid == 1) data.b++;
            else if (tid == 2) data.c++;
            else if (tid == 3) data.d++;
        }
    }

    double elapsed = omp_get_wtime() - start;
    std::cout << "False Sharing Time: " << elapsed << " s\n";
}

void benchmark_no_false_sharing() {
    NoFalseSharingData data = {0};
    double start = omp_get_wtime();

    #pragma omp parallel num_threads(THREADS)
    {
        int tid = omp_get_thread_num();
        for (int i = 0; i < ITERATIONS; i++) {
            if (tid == 0) data.a++;
            else if (tid == 1) data.b++;
            else if (tid == 2) data.c++;
            else if (tid == 3) data.d++;
        }
    }

    double elapsed = omp_get_wtime() - start;
    std::cout << "No False Sharing Time: " << elapsed << " s\n";
}

int main() {
    std::cout << "--- False Sharing Benchmark ---\n";
    benchmark_false_sharing();
    benchmark_no_false_sharing();
    return 0;
}

struct Data {
    int a[1000];
    int b[1000];
    int c[1000];
    int d[1000];
};