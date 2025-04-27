#include <cstdint>
#include <cstdio>
#include <nmmintrin.h>

int main() {
    uint32_t x = 0b110101101;
    uint64_t y = 0b1010101010101010;

    // Using Intel Intrinsics
    int popcount_x = _mm_popcnt_u32(x);  // For 32-bit integer
    int popcount_y = _mm_popcnt_u64(y);  // For 64-bit integer

    printf("Popcount of x: %d\n", popcount_x);
    printf("Popcount of y: %d\n", popcount_y);

    // GCC Builtins
    int popcount_x_gcc = __builtin_popcount(x);
    int popcount_y_gcc = __builtin_popcountll(y);

    printf("Popcount of x (GCC): %d\n", popcount_x_gcc);
    printf("Popcount of y (GCC): %d\n", popcount_y_gcc);
    
    return 0;
}
