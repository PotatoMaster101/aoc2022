#include <fstream>
#include <iostream>
#include <regex>
#include <stack>
#include <string>
#include <vector>

struct stack_of_crates {
    std::stack<char> crates{};

    void move_to(stack_of_crates& another, int count) {
        auto counter{0};
        while (!crates.empty() && counter < count) {
            another.crates.push(crates.top());
            crates.pop();
            counter++;
        }
    }

    void keep_order_move_to(stack_of_crates& another, int count) {
        auto counter{0};
        std::stack<char> tmp;
        while (!crates.empty() && counter < count) {
            tmp.push(crates.top());
            crates.pop();
            counter++;
        }

        while (!tmp.empty()) {
            another.crates.push(tmp.top());
            tmp.pop();
        }
    }
};

struct instruction {
    int moves = 0;
    int from = 0;
    int to = 0;
};

std::vector<stack_of_crates> parse_header(const std::vector<std::string>& lines) {
    auto lastLine = lines[lines.size() - 1];
    auto numStacks = std::count_if(lastLine.begin(), lastLine.end(), [](auto&& c) { return c != ' '; });
    std::vector<stack_of_crates> result(numStacks);

    auto stop{false};
    for (std::size_t i{lines.size() - 1}; !stop; i--) {     // ignore last line in header
        for (std::size_t j{0}; j < lines[i].size(); j++) {
            if (lines[i][j] >= 'A' && lines[i][j] <= 'Z')
                result[(j - 1) / 4].crates.push(lines[i][j]);
        }
        stop = i == 0;
    }
    return result;
}

std::vector<instruction> parse_instructions(const std::vector<std::string>& lines) {
    std::vector<instruction> result;
    result.reserve(lines.size());

    std::regex rgx{R"(move (\d+) from (\d+) to (\d+))"};
    std::smatch match;
    for (const auto& line : lines) {
        if (std::regex_search(line, match, rgx)) {
            auto moves = std::stoi(match[1].str());
            auto from = std::stoi(match[2].str()) - 1;      // 0 based index
            auto to = std::stoi(match[3].str()) - 1;        // 0 based index
            result.push_back(instruction{moves, from, to});
        }
    }
    return result;
}

std::vector<std::string> get_input(std::ifstream& input, bool isHeader) {
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(input, line)) {
        if (isHeader && line.empty())
            return lines;

        lines.push_back(line);
    }
    return lines;
}

int main() {
    std::ifstream input{"input.txt"};

    // part 1
    auto header = get_input(input, true);
    auto crates = parse_header(header);

    auto moves = get_input(input, false);
    auto instructions = parse_instructions(moves);
    for (const auto& ins : instructions)
        crates[ins.from].move_to(crates[ins.to], ins.moves);
    for (const auto& stc : crates)
        std::cout << stc.crates.top();
    std::cout << '\n';

    // part 2
    crates = parse_header(header);
    for (const auto& ins : instructions)
        crates[ins.from].keep_order_move_to(crates[ins.to], ins.moves);
    for (const auto& stc : crates)
        std::cout << stc.crates.top();

    return 0;
}
