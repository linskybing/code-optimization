#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>

using real_t = float;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <output.bin> <ans.bin>" << std::endl;
        return 1;
    }
    const char* out_file = argv[1];
    const char* ans_file = argv[2];

    // Open binary files
    std::ifstream ifs_out(out_file, std::ios::binary);
    std::ifstream ifs_ans(ans_file, std::ios::binary);
    if (!ifs_out || !ifs_ans) {
        std::cerr << "Cannot open binary files." << std::endl;
        return 1;
    }

    // Get file sizes
    ifs_out.seekg(0, std::ios::end);
    ifs_ans.seekg(0, std::ios::end);
    size_t count_out = ifs_out.tellg() / sizeof(real_t);
    size_t count_ans = ifs_ans.tellg() / sizeof(real_t);

    // Ensure both files have the same size
    if (count_out != count_ans) {
        std::cerr << "File sizes do not match!" << std::endl;
        return 1;
    }

    size_t count = count_out;  // Total number of elements

    // Read data from files
    std::vector<real_t> out(count), ans(count);
    ifs_out.seekg(0, std::ios::beg);
    ifs_ans.seekg(0, std::ios::beg);
    ifs_out.read(reinterpret_cast<char*>(out.data()), count * sizeof(real_t));
    ifs_ans.read(reinterpret_cast<char*>(ans.data()), count * sizeof(real_t));

    size_t diff_cnt = 0;
    real_t max_err = 0;
    size_t printed = 0;

    std::cout << std::fixed << std::setprecision(6);
    for (size_t i = 0; i < count; ++i) {
        real_t err = std::fabs(out[i] - ans[i]);
        if (err != 0) {
            ++diff_cnt;
            if (err > max_err) max_err = err;
            if (printed < 10) {
                std::cout << "idx=" << i
                          << " out=" << out[i]
                          << " ans=" << ans[i]
                          << " err=" << err << "\n";
                ++printed;
            }
        }
    }
    if (diff_cnt > 10) std::cout << "... (" << diff_cnt - printed << " more differences)\n";

    std::cout << "Total differences: " << diff_cnt << "\n";
    std::cout << "Max error: " << max_err << "\n";
    return 0;
}
