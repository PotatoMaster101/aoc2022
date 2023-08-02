#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<int> get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::vector<int> result;
    int current{0};

    while (std::getline(input, line)) {
        if (line.empty()) {
            result.push_back(current);
            current = 0;
        } else {
            current += std::stoi(line);
        }
    }
    return result;
}

int main() {
    auto input = get_input("input.txt");
    std::ranges::sort(input);

    // part 1
    std::cout << input[input.size() - 1] << '\n';

    // part 2
    auto size = input.size();
    std::cout << (input[size - 1] + input[size - 2] + input[size - 3]) << '\n';
    return 0;
}
