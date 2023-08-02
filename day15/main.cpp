#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <regex>
#include <set>
#include <string>
#include <utility>
#include <vector>

constexpr auto Min = std::numeric_limits<std::ptrdiff_t>::min();
constexpr auto Max = std::numeric_limits<std::ptrdiff_t>::max();
constexpr auto Sensor = 'S';
constexpr auto Beacon = 'B';
constexpr auto Air = '.';
constexpr auto SensorRange = '#';

struct pos {
    std::ptrdiff_t x = 0;
    std::ptrdiff_t y = 0;

    friend bool operator==(pos lhs, pos rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
    friend bool operator!=(pos lhs, pos rhs) { return lhs.x != rhs.x || lhs.y != rhs.y; }
};

struct report_entry {
    pos sensor;
    pos beacon;

    [[nodiscard]] std::ptrdiff_t dist() const {
        return dist(beacon);
    }

    [[nodiscard]] std::ptrdiff_t dist(pos p) const {
        return std::llabs(sensor.x - p.x) + std::llabs(sensor.y - p.y);
    }

    [[nodiscard]] bool in_range(pos p) const {
        return dist(p) <= dist();
    }
};

struct report {
    std::vector<report_entry> entries;

    [[nodiscard]] char pos_char(pos p) const {
        for (const auto& entry : entries) {
            if (entry.sensor == p)
                return Sensor;
            if (entry.beacon == p)
                return Beacon;
            if (entry.in_range(p))
                return SensorRange;
        }
        return Air;
    }

    [[nodiscard]] std::pair<std::ptrdiff_t, std::ptrdiff_t> x_range() const {
        auto max = Min;
        auto min = Max;
        std::ranges::for_each(entries, [&](const auto& item) {
            auto d = item.dist();
            if ((item.sensor.x + d) > max)
                max = item.sensor.x + d;
            if ((item.sensor.x - d) < min)
                min = item.sensor.x - d;
        });
        return {min, max};
    }
};

report_entry parse_line(const std::string& line) {
    std::regex rgx{R"(Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+))"};
    std::smatch match;
    report_entry result;
    if (std::regex_search(std::cbegin(line), std::cend(line), match, rgx)) {
        result.sensor = {std::stoll(match[1]), std::stoll(match[2])};
        result.beacon = {std::stoll(match[3]), std::stoll(match[4])};
    }
    return result;
}

report get_input(const std::string& filename) {
    std::ifstream fs{filename};
    std::string line;
    std::vector<std::string> lines;
    std::vector<report_entry> result;
    while (std::getline(fs, line))
        result.push_back(parse_line(line));
    return {std::move(result)};
}

pos get_part2_air(const report& r) {
    auto outOfRange = [](std::ptrdiff_t v) { return v < 0 || v > 4000000; };
    for (const auto& entry : r.entries) {
        auto d = entry.dist();
        auto maxY = entry.sensor.y + d;
        auto minY = entry.sensor.y - d;
        if (outOfRange(maxY) || outOfRange(minY))
            continue;

        // check for top Y and bottom Y
        if (r.pos_char({entry.sensor.x, maxY + 1}) == Air)
            return {entry.sensor.x, maxY + 1};
        if (r.pos_char({entry.sensor.x, minY - 1}) == Air)
            return {entry.sensor.x, minY - 1};

        // check for all left X and right X
        for (std::ptrdiff_t y = minY; y <= maxY; y++) {
            auto yDiff = d - std::llabs(entry.sensor.y - y);
            auto minX = entry.sensor.x - yDiff - 1;
            auto maxX = entry.sensor.x + yDiff + 1;
            if (outOfRange(minX) || outOfRange(maxX))
                break;

            if (r.pos_char({minX, y}) == Air)
                return {minX, y};
            if (r.pos_char({maxX, y}) == Air)
                return {maxX, y};
        }
    }
    return {0, 0};
}

int main() {
    auto input = get_input("input.txt");

    // part 1
    auto xRange = input.x_range();
    std::size_t counter = 0;
    for (std::ptrdiff_t i = xRange.first; i <= xRange.second; i++)
        if (input.pos_char({i, 2000000}) == SensorRange)
            counter++;
    std::cout << counter << '\n';

    // part 2
    auto p = get_part2_air(input);
    std::cout << p.x * 4000000 + p.y << '\n';
    return 0;
}
