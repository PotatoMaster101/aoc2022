#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

constexpr auto Air = '.';
constexpr auto Rock = '#';
constexpr auto Sand = 'O';
constexpr auto Start = '+';

struct pos {
    std::size_t row = 0;
    std::size_t col = 0;

    friend bool operator==(pos lhs, pos rhs) { return lhs.row == rhs.row && lhs.col == rhs.col; }
    friend bool operator!=(pos lhs, pos rhs) { return lhs.row != rhs.row || lhs.col != rhs.col; }
};

struct rock_path {
    std::vector<pos> path;
    std::vector<pos> full_path;
    std::size_t min_col;
    std::size_t max_col;
    std::size_t max_row;

    explicit rock_path(const std::vector<pos>& path) : path{path} {
        min_col = std::ranges::min_element(path, [](pos lhs, pos rhs) { return lhs.col < rhs.col; })->col;
        max_col = std::ranges::max_element(path, [](pos lhs, pos rhs) { return lhs.col < rhs.col; })->col;
        max_row = std::ranges::max_element(path, [](pos lhs, pos rhs) { return lhs.row < rhs.row; })->row;
        build_full_path();
    }

    void build_full_path() {
        full_path.insert(std::end(full_path), std::begin(path), std::end(path));
        for (std::size_t i = 1; i < path.size(); i++) {
            auto curr = path[i - 1];
            while (curr != path[i]) {
                if (curr.row < path[i].row)
                    curr.row++;
                else if (curr.row > path[i].row)
                    curr.row--;
                else if (curr.col < path[i].col)
                    curr.col++;
                else
                    curr.col--;
                full_path.push_back(curr);
            }
        }
    }
};

struct cave {
    std::vector<rock_path> paths;
    std::vector<std::vector<char>> map;
    std::size_t min_col;
    std::size_t max_col;
    std::size_t max_row;
    std::size_t col_offset = 0;

    explicit cave(const std::vector<rock_path>& paths) : paths(paths) {
        min_col = std::ranges::min_element(paths, [](const auto& lhs, const auto& rhs) { return lhs.min_col < rhs.min_col; })->min_col;
        max_col = std::ranges::max_element(paths, [](const auto& lhs, const auto& rhs) { return lhs.max_col < rhs.max_col; })->max_col;
        max_row = std::ranges::max_element(paths, [](const auto& lhs, const auto& rhs) { return lhs.max_row < rhs.max_row; })->max_row;
    }

    [[nodiscard]] pos reduce_pos(pos p) const {
        return {p.row, p.col - min_col};
    }

    [[nodiscard]] pos transform_pos(pos p) const {
        p = reduce_pos(p);
        return {p.row, p.col + col_offset};
    }

    void pad_map(std::size_t amount) {
        col_offset += amount;
        std::vector<char> air(amount, Air);
        std::vector<char> rock(amount, Rock);
        for (auto& r : map) {
            auto& container = &r == &map.back() ? rock : air;
            r.insert(std::begin(r), std::begin(container), std::end(container));
            r.insert(std::end(r), std::begin(container), std::end(container));
        }
    }

    [[nodiscard]] bool run_part1(pos p = {0, 500}) {
        p = reduce_pos(p);
        while (p.row < max_row) {
            if (map[p.row + 1][p.col] == Air) {     // go down
                p.row++;
            } else if (p.col > 0 && map[p.row + 1][p.col - 1] == Air) {     // go down-left
                p.row++;
                p.col--;
            } else if (p.col < map[0].size() && map[p.row + 1][p.col + 1] == Air) {     // go down-right
                p.row++;
                p.col++;
            } else if (p.col == 0 || p.col == map[0].size()) {
                return false;   // sand is en route to oblivion
            } else {
                break;
            }
        }
        if (p.row != max_row) {
            map[p.row][p.col] = Sand;
            return true;
        } else {
            return false;       // sand is en route to oblivion
        }
    }

    [[nodiscard]] bool run_part2(pos p = {0, 500}) {
        p = transform_pos(p);
        if (map[p.row][p.col] != Start)
            return false;

        while (true) {
            if (map[p.row + 1][p.col] == Air) {     // go down
                p.row++;
            } else if (p.col > 0 && map[p.row + 1][p.col - 1] == Air) {     // go down-left
                p.row++;
                p.col--;
            } else if (p.col < map[0].size() && map[p.row + 1][p.col + 1] == Air) {     // go down-right
                p.row++;
                p.col++;
            } else {
                map[p.row][p.col] = Sand;
                return true;
            }
        }
    }

    void build_map(bool floor = false) {
        auto colSize = max_col - min_col + 1;
        map = std::vector<std::vector<char>>(max_row + 1, std::vector<char>(colSize, Air));
        map[0][500 - min_col] = Start;
        for (const auto& path : paths) {
            for (pos p : path.full_path) {
                p = reduce_pos(p);
                map[p.row][p.col] = Rock;
            }
        }

        if (floor) {
            map.emplace_back(colSize, Air);
            map.emplace_back(colSize, Rock);
        }
    }
};

rock_path parse(const std::string& line) {
    std::vector<pos> path;
    std::regex rgx{"(\\d+),(\\d+)"};
    std::smatch match;
    auto startFrom = line.cbegin();
    while (std::regex_search(startFrom, line.cend(), match, rgx)) {
        path.emplace_back(std::stoull(match[2]), std::stoull(match[1]));
        startFrom = match.suffix().first;
    }
    return rock_path{path};
}

std::vector<rock_path> get_input(const std::string& filename) {
    std::ifstream fs{filename};
    std::string line;
    std::vector<rock_path> result;
    while (std::getline(fs, line))
        result.push_back(parse(line));
    return result;
}

int main() {
    auto input = get_input("input.txt");

    // part 1
    cave c{input};
    c.build_map();
    std::size_t counter = 0;
    while (c.run_part1())
        counter++;
    std::cout << counter << '\n';

    // part 2
    counter = 0;
    c.build_map(true);
    c.pad_map(200);
    while (c.run_part2())
        counter++;
    std::cout << counter << '\n';
    return 0;
}
