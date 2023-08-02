#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <vector>

struct monkey {
    [[maybe_unused]] int num{0};
    std::vector<long long> items;
    std::vector<std::string> operation;
    long div_target{1};
    long true_target{0};
    long false_target{0};
    long long inspect_times{0};

    [[nodiscard]] bool has_items() const noexcept {
        return !items.empty();
    }

    [[nodiscard]] long long run_operation() {
        if (items.empty())
            return -1;

        auto operand1 = operation[0] == "old" ? items[0] : std::stoll(operation[0]);
        auto operand2 = operation[2] == "old" ? items[0] : std::stoll(operation[2]);
        long long result = -1;

        if (operation[1] == "*")
            result = operand1 * operand2;
        else if (operation[1] == "+")
            result = operand1 + operand2;

        inspect_times++;
        items.erase(items.begin());
        return result;
    }
};

int regex_get_int(const std::string& text, const std::string& regex) {
    std::regex rgx{regex};
    std::smatch match;
    auto result = -1;
    if (std::regex_match(text, match, rgx))
        result = std::stoi(match[1].str());

    return result;
}

std::vector<long long> parse_items(const std::string& text) {
    std::vector<long long> result;
    std::string currNum;
    for (auto ch : text) {
        if (ch == ',' || ch == ' ') {
            if (!currNum.empty())
                result.push_back(std::stoll(currNum));

            currNum = "";
        } else {
            currNum += ch;
        }
    }
    result.push_back(std::stoll(currNum));
    return result;
}

std::vector<std::string> parse_operation(const std::string& text) {
    std::vector<std::string> result;
    std::string currToken;
    for (auto ch : text) {
        if (ch == ' ') {
            result.push_back(currToken);
            currToken = "";
        } else {
            currToken += ch;
        }
    }
    result.push_back(currToken);
    return result;
}

std::vector<monkey> get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::vector<monkey> result;
    monkey current;
    while (std::getline(input, line)) {
        if (line.starts_with("Monkey"))
            current.num = regex_get_int(line, "Monkey ([0-9]+):");
        else if (line.starts_with("  Starting items"))
            current.items = parse_items(line.substr(18, line.size()));
        else if (line.starts_with("  Operation"))
            current.operation = parse_operation(line.substr(19, line.size()));
        else if (line.starts_with("  Test"))
            current.div_target = regex_get_int(line, "  Test: divisible by ([0-9]+)");
        else if (line.starts_with("    If true"))
            current.true_target = regex_get_int(line, "    If true: throw to monkey ([0-9]+)");
        else if (line.starts_with("    If false"))
            current.false_target = regex_get_int(line, "    If false: throw to monkey ([0-9]+)");
        else if (line.empty())
            result.push_back(current);
    }
    result.push_back(current);
    return result;
}

template <typename Reduce> requires std::invocable<Reduce, long long>
void run_round(std::vector<monkey>& monks, Reduce reduce) {
    for (auto& m : monks) {
        while (m.has_items()) {
            auto itemWorry = reduce(m.run_operation());
            if (itemWorry % m.div_target == 0)
                monks[m.true_target].items.push_back(itemWorry);
            else
                monks[m.false_target].items.push_back(itemWorry);
        }
    }
}

int main() {
    auto input = get_input("input.txt");
    auto sortComp = [](const auto& lhs, const auto& rhs) {
        return lhs.inspect_times > rhs.inspect_times;
    };

    // part 1
    for (auto i = 0; i < 20; i++)
        run_round(input, [](const auto& num) { return num / 3; });

    std::ranges::sort(input, sortComp);
    std::cout << input[0].inspect_times * input[1].inspect_times << '\n';

    // part 2
    input = get_input("input.txt");
    auto mod = std::accumulate(input.begin(), input.end(), 1L, [](const auto& sum, const auto& curr) {
        return sum * curr.div_target;
    });

    for (auto i = 0; i < 10000; i++)
        run_round(input, [mod](const auto& num) { return num % mod; });

    std::ranges::sort(input, sortComp);
    std::cout << input[0].inspect_times * input[1].inspect_times << '\n';
    return 0;
}
