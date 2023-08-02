#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

constexpr auto MaxDist = std::numeric_limits<std::size_t>::max();

struct coor {
    std::size_t row = 0;
    std::size_t col = 0;
};

struct node {
    char value;
    std::size_t dist = MaxDist;
    bool is_start = false;
    bool is_end = false;

    explicit node(char val) : value{val} {
        if (val == 'S') {
            value = 'a';
            is_start = true;
            dist = 0;
        } else if (val == 'E') {
            value = 'z';
            is_end = true;
        }
    }

    void visit(std::size_t d = 0) { dist = d; }
    [[nodiscard]] bool visited() const { return dist != MaxDist; }
    [[nodiscard]] bool compare_height(const node& dst) const { return value >= dst.value || value + 1 == dst.value; }
};

struct input {
    std::vector<std::vector<node>> map;

    explicit input(const std::vector<std::string>& map) {
        this->map.reserve(map.size());
        for (const auto& str : map) {
            std::vector<node> row;
            row.reserve(str.size());
            for (const auto& ch : str)
                row.emplace_back(ch);
            this->map.push_back(row);
        }
    }

    [[nodiscard]] std::size_t rows() const { return map.size(); }
    [[nodiscard]] std::size_t cols() const { return map[0].size(); }
    [[nodiscard]] bool visited(coor c) const { return (*this)[c].visited(); }
    [[nodiscard]] node& operator[](coor c) { return map[c.row][c.col]; }
    [[nodiscard]] const node& operator[](coor c) const { return map[c.row][c.col]; }

    [[nodiscard]] coor start_pos() const {
        for (std::size_t r = 0; r < rows(); r++)
            for (std::size_t c = 0; c < cols(); c++)
                if (map[r][c].is_start)
                    return {r, c};
        return {MaxDist, MaxDist};
    }

    [[nodiscard]] coor end_pos() const {
        for (std::size_t r = 0; r < rows(); r++)
            for (std::size_t c = 0; c < cols(); c++)
                if (map[r][c].is_end)
                    return {r, c};
        return {MaxDist, MaxDist};
    }

    [[nodiscard]] bool can_move(coor from, coor to, std::size_t cost, bool invert = false) const {
        auto& fromNode = (*this)[from];
        auto& toNode = (*this)[to];
        auto lessCost = cost < toNode.dist;
        return lessCost && (invert ? toNode.compare_height(fromNode) : fromNode.compare_height(toNode));
    }

    [[nodiscard]] std::vector<coor> neighbours(coor from) const {
        std::vector<coor> result;
        result.reserve(4);

        if (from.row > 0 && !visited({from.row - 1, from.col}))
            result.emplace_back(from.row - 1, from.col);
        if (from.row < rows() - 1 && !visited({from.row + 1, from.col}))
            result.emplace_back(from.row + 1, from.col);
        if (from.col > 0 && !visited({from.row, from.col - 1}))
            result.emplace_back(from.row, from.col - 1);
        if (from.col < cols() - 1 && !visited({from.row, from.col + 1}))
            result.emplace_back(from.row, from.col + 1);
        return result;
    }
};

input get_input(const std::string& filename) {
    std::ifstream fs{filename};
    std::string line;
    std::vector<std::string> result;
    while (std::getline(fs, line))
        result.push_back(line);
    return input{result};
}

std::size_t run_part1(input& map) {
    std::queue<std::pair<coor, std::size_t>> queue;
    queue.emplace(map.start_pos(), 0);
    while (!queue.empty()) {
        auto [from, cost] = queue.front();
        queue.pop();
        if (map[from].is_end)
            continue;

        for (const auto& to : map.neighbours(from)) {
            if (map.can_move(from, to, cost + 1)) {
                queue.emplace(to, cost + 1);
                map[to].visit(cost + 1);
            }
        }
    }
    return map[map.end_pos()].dist;
}

std::size_t run_part2(input& map) {
    std::queue<std::pair<coor, std::size_t>> queue;
    queue.emplace(map.end_pos(), 0);    // start at end
    std::size_t shortest = MaxDist;
    while (!queue.empty()) {
        auto [from, cost] = queue.front();
        queue.pop();
        if (map[from].value == 'a') {   // update shortest for any 'a'
            if (shortest > cost)
                shortest = cost;
            continue;
        }

        for (const auto& to : map.neighbours(from)) {
            if (map.can_move(from, to, cost + 1, true)) {
                queue.emplace(to, cost + 1);
                map[to].visit(cost + 1);
            }
        }
    }
    return shortest;
}

int main() {
    // part 1
    auto map = get_input("input.txt");
    std::cout << run_part1(map) << '\n';

    // part 2
    map = get_input("input.txt");
    std::cout << run_part2(map) << '\n';
    return 0;
}
