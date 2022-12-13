#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

struct instruction {
    char direction;
    int count;

    instruction(char direction, int count)
        : direction{direction}, count{count} {}
};

struct coor {
    int x;
    int y;

    [[nodiscard]] bool operator<(const coor& rhs) const {
        return x < rhs.x || (x == rhs.x && y < rhs.y);
    }
};

void tail_follow(char direction, const coor& head, coor& tail) {
    if (std::abs(head.x - tail.x) > 1 || std::abs(head.y - tail.y) > 1) {
        // tail out of range
        switch (direction) {
            case 'L': tail.x = head.x + 1; tail.y = head.y; break;
            case 'R': tail.x = head.x - 1; tail.y = head.y; break;
            case 'U': tail.y = head.y - 1; tail.x = head.x; break;
            case 'D': tail.y = head.y + 1; tail.x = head.x; break;
            default: break;
        }
    }
}

void move_coor(char direction, coor& c) {
    switch (direction) {
        case 'L': c.x--; break;
        case 'R': c.x++; break;
        case 'U': c.y++; break;
        case 'D': c.y--; break;
        default: break;
    }
}

struct short_rope {
    coor head{0, 0};
    coor tail{0, 0};
    std::set<coor> tail_visited{{0, 0}};

    void move_head(const instruction& inst) {
        for (int i{0}; i < inst.count; i++) {
            move_coor(inst.direction, head);
            tail_follow(inst.direction, head, tail);
            tail_visited.insert(tail);
        }
    }
};

std::vector<instruction> get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::vector<instruction> result;
    while (std::getline(input, line)) {
        auto space = line.find(' ');
        if (space != std::string::npos) {
            auto count = std::stoi(line.substr(space));
            result.emplace_back(line[0], count);
        }
    }
    return result;
}

int main() {
    auto input = get_input("input.txt");

    // part 1
    short_rope sr;
    std::ranges::for_each(input, [&sr](auto&& i) { sr.move_head(i); });
    std::cout << sr.tail_visited.size() << '\n';

    // part 2
    // TODO
    return 0;
}
