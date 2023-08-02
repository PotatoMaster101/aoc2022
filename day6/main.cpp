#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::getline(input, line);
    return line;
}

std::size_t get_sig_idx(const std::string& input, std::size_t uniqCount) {
    for (std::size_t i{uniqCount}; i < input.size(); i++) {
        auto sub = input.substr(i - uniqCount, uniqCount + 1);
        std::sort(sub.begin(), sub.end());

        if (std::unique(sub.begin(), sub.end()) == sub.end())
            return i + 1;
    }
    return 0;
}

int main() {
    auto input = get_input("input.txt");

    // part 1
    std::cout << get_sig_idx(input, 3) << '\n';

    // part 2
    std::cout << get_sig_idx(input, 13) << '\n';
    return 0;
}
