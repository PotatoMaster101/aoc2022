#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

struct section {
    int start;
    int end;
    section(int start, int end) : start{start}, end{end} {}

    [[nodiscard]] bool full_contain(const section& another) const {
        return (another.start >= start && another.end <= end) || (another.start <= start && another.end >= end);
    }

    [[nodiscard]] bool partial_contain(const section& another) const {
        return (another.start >= start && another.start <= end)
            || (another.end >= start && another.end <= end)
            || (start >= another.start && start <= another.end)
            || (end >= another.start && end <= another.end);
    }
};

std::vector<std::string> split_str(const std::string& input, char delim) {
    auto split = input
        | std::ranges::views::split(delim)
        | std::ranges::views::transform([](auto&& str) { return std::string(&*str.begin(), std::ranges::distance(str)); });

    return std::vector<std::string>{split.begin(), split.end()};
}

std::vector<std::string> get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::vector<std::string> result;
    while (std::getline(input, line))
        result.push_back(line);

    return result;
}

std::pair<section, section> get_sections(const std::string& input) {
    auto sections = split_str(input, ',');
    auto firstSplit = split_str(sections[0], '-');
    auto secondSplit = split_str(sections[1], '-');
    section first{std::stoi(firstSplit[0]), std::stoi(firstSplit[1])};
    section second{std::stoi(secondSplit[0]), std::stoi(secondSplit[1])};
    return std::make_pair(first, second);
}

int main() {
    auto input = get_input("input.txt");
    auto result{0};

    // part 1
    for (const auto& line : input) {
        auto sections = get_sections(line);
        if (sections.first.full_contain(sections.second))
            result++;
    }
    std::cout << result << '\n';

    // part 2
    result = 0;
    for (const auto& line : input) {
        auto sections = get_sections(line);
        if (sections.first.partial_contain(sections.second))
            result++;
    }
    std::cout << result << '\n';
    return 0;
}
