#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <regex>
#include <set>
#include <string>
#include <utility>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

constexpr auto Start = "AA";
constexpr auto MaxTimePart1 = 30;
constexpr auto MaxTimePart2 = 26;

struct scan_output {
    std::map<std::string, int64_t> rates;
    std::multimap<std::string, std::string> links;
    std::map<std::string, std::map<std::string, int64_t>> shortest;
    std::map<std::string, int64_t> non_zero_rates;

    [[nodiscard]] std::map<std::string, std::map<std::string, int64_t>> shortest_dist() const {
        // use Floyd-Warshall
        std::map<std::string, std::map<std::string, int64_t>> dist;
        for (const auto& from : rates)
            for (const auto& to : rates)
                dist[from.first][to.first] = std::numeric_limits<int>::max();     // good enough "infinite" for this problem

        for (const auto& [from, to] : links)
            dist[from][to] = 1;
        for (const auto& n : rates)
            dist[n.first][n.first] = 0;

        for (const auto& k : rates)
            for (const auto& i : rates)
                for (const auto& j : rates)
                    if (dist[i.first][j.first] > dist[i.first][k.first] + dist[k.first][j.first])
                        dist[i.first][j.first] = dist[i.first][k.first] + dist[k.first][j.first];
        return dist;
    }

    [[nodiscard]] int64_t curr_flow(const std::set<std::string>& opened) const {
        const auto& rts = rates;
        return std::accumulate(std::begin(opened), std::end(opened), 0, [&rts](const auto& a, const auto& b) {
            return a + rts.at(b);
        });
    }

    void init() {
        shortest = shortest_dist();
        for (const auto& r : rates)
            if (r.second > 0)
                non_zero_rates.insert(std::make_pair(r.first, r.second));
    }

    [[nodiscard]] int64_t most_pressure(
            const std::string& curr,
            int64_t time,
            int64_t total,
            std::set<std::string>& opened,
            const std::map<std::string, int64_t>& nonZeroRates,
            bool human = false,
            bool includeElephant = false,
            int max = MaxTimePart1) const {
        auto currFlow = curr_flow(opened);
        auto result = total + currFlow * (max - time);
        if (includeElephant && human) {
            // give elephant not opened non-zero
            auto notOpenedNonZeroRates = nonZeroRates;
            for (const auto& o : opened)
                notOpenedNonZeroRates.erase(o);

            std::set<std::string> elephantOpened;
            auto elephantResult = most_pressure(Start, 0, 0, elephantOpened, notOpenedNonZeroRates, false,
                                                includeElephant, max);
            result = total + currFlow * (max - time) + elephantResult;
        }

        for (const auto& v : nonZeroRates) {
            if (v.second == 0 || opened.contains(v.first))
                continue;

            // time for moving to next valve and opening it
            auto newTime = shortest.at(curr).at(v.first) + 1;   // use at() to enforce const func
            if (time + newTime >= max)
                continue;   // will take too long

            // open this valve
            auto newTotal = total + currFlow * newTime;
            opened.insert(v.first);
            auto nextVal = most_pressure(v.first, time + newTime, newTotal, opened, nonZeroRates, human,
                                         includeElephant, max);
            opened.erase(v.first);
            if (result < nextVal)
                result = nextVal;
        }
        return result;
    }
};

void parse_line(const std::string& line, scan_output& output) {
    std::regex rgx{R"(Valve (.*) has flow rate=([0-9]+); tunnels? leads? to valves? (.*))"};
    std::smatch match;
    if (std::regex_search(std::cbegin(line), std::cend(line), match, rgx)) {
        output.rates[match[1]] = std::stoi(match[2]);

        // parse links
        auto links = match[3].str();
        for (std::size_t i = 1; i < links.size(); i += 2) {
            std::string str{links[i - 1], links[i]};
            if (str != ", ")
                output.links.insert(std::make_pair(match[1], str));
        }
    }
}

scan_output get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    scan_output result;
    while (std::getline(input, line))
        parse_line(line, result);
    return result;
}

int main() {
    auto input = get_input("input.txt");
    input.init();

    // part 1
    std::set<std::string> opened;
    std::cout << input.most_pressure(Start, 0, 0, opened, input.non_zero_rates) << '\n';

    // part 2
    input = get_input("input.txt");
    input.init();
    opened.clear();
    std::cout << input.most_pressure(Start, 0, 0, opened, input.non_zero_rates, true, true, MaxTimePart2) << '\n';
    return 0;
}

#pragma clang diagnostic pop
