#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int get_priority(char c) {
    return (c >= 'a' && c <= 'z') ? c - 96 : c - 38;
}

char get_common_char(const std::string& first, const std::string& second) {
    for (const auto& c : first) {
        if (second.find(c) != std::string::npos)
            return c;
    }
    return 0;
}

char get_common_char(const std::string& first, const std::string& second, const std::string& third) {
    for (const auto& c : first) {
        if (second.find(c) != std::string::npos && third.find(c) != std::string::npos)
            return c;
    }
    return 0;
}

std::vector<std::string> get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::vector<std::string> result;

    while (std::getline(input, line))
        result.push_back(line);

    return result;
}

int main() {
    auto input = get_input("input.txt");

    // part 1
    auto priority{0};
    for (const auto& line : input) {
        auto half = line.size() / 2;
        auto c = get_common_char(line.substr(0, half), line.substr(half));
        priority += get_priority(c);
    }
    std::cout << priority << '\n';

    // part 2
    priority = 0;
    for (std::size_t i{3}; i < input.size() + 3; i += 3) {
        auto c = get_common_char(input[i - 3], input[i - 2], input[i - 1]);
        priority += get_priority(c);
    }
    std::cout << priority << '\n';
    return 0;
}
