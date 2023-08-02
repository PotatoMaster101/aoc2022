#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const auto CrtMaxLen = 40;

enum class instruction_type {
    add,
    nop
};

struct instruction {
    instruction_type type;
    int value;

    instruction(instruction_type type, int value)
        : type{type}, value{value} {}
};

struct cpu {
    int reg_val{1};
    std::size_t current_cycle{0};
    std::size_t sig_strength{0};
    std::string crt;

    void execute(const instruction& inst) {
        current_cycle++;
        calc_sig_strength();
        if (inst.type == instruction_type::add) {
            current_cycle++;
            calc_sig_strength();
            reg_val += inst.value;
        }
    }

    [[nodiscard]] bool crt_in_range() const {
        auto currCrtIdx = (current_cycle - 1) % CrtMaxLen;
        return currCrtIdx >= reg_val - 1 && currCrtIdx <= reg_val + 1;
    }

    void print_crt() {
        if (crt.size() == CrtMaxLen) {
            std::cout << crt << '\n';
            crt.clear();
        }
    }

    void execute_with_crt(const instruction& inst) {
        current_cycle++;
        crt += crt_in_range() ? "#" : ".";
        print_crt();

        if (inst.type == instruction_type::add) {
            current_cycle++;
            crt += crt_in_range() ? '#' : '.';
            reg_val += inst.value;
            print_crt();
        }
    }

    void calc_sig_strength() {
        int calc_cyc[] = {20, 60, 100, 140, 180, 220};
        auto size = (sizeof calc_cyc) / (sizeof *calc_cyc);
        if (std::binary_search(calc_cyc, calc_cyc + size, current_cycle))
            sig_strength += reg_val * current_cycle;
    }
};

std::vector<instruction> get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::vector<instruction> result;
    while (std::getline(input, line)) {
        auto space = line.find(' ');
        if (space != std::string::npos) {
            auto value = std::stoi(line.substr(space));
            result.emplace_back(instruction_type::add, value);
        } else {
            result.emplace_back(instruction_type::nop, 0);
        }
    }

    return result;
}

int main() {
    auto input = get_input("input.txt");

    // part 1
    cpu run;
    std::ranges::for_each(input, [&run](auto&& item){ run.execute(item); });
    std::cout << run.sig_strength << '\n';

    // part 2
    run = cpu{};
    std::ranges::for_each(input, [&run](auto&& item){
        run.execute_with_crt(item);
    });
    return 0;
}
