#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct trees {
    std::vector<std::string> heights;

    [[nodiscard]] std::size_t width() const {
        return heights[0].size();
    }

    [[nodiscard]] std::size_t length() const {
        return heights.size();
    }

    [[nodiscard]] bool can_see_up(std::size_t r, std::size_t c) const {
        if (r == 0)
            return true;

        auto val = heights[r][c];
        auto stop{false};
        for (std::size_t i{r - 1}; !stop; i--) {
            stop = i == 0;
            if (val <= heights[i][c])
                return false;
        }
        return true;
    }

    [[nodiscard]] bool can_see_down(std::size_t r, std::size_t c) const {
        auto maxIdx = length() - 1;
        if (r >= maxIdx)
            return true;

        auto val = heights[r][c];
        for (std::size_t i{r + 1}; i <= maxIdx; i++) {
            if (val <= heights[i][c])
                return false;
        }
        return true;
    }

    [[nodiscard]] bool can_see_left(std::size_t r, std::size_t c) const {
        if (c == 0)
            return true;

        auto val = heights[r][c];
        auto stop{false};
        for (std::size_t i{c - 1}; !stop; i--) {
            stop = i == 0;
            if (val <= heights[r][i])
                return false;
        }
        return true;
    }

    [[nodiscard]] bool can_see_right(std::size_t r, std::size_t c) const {
        auto maxIdx = width() - 1;
        if (c >= maxIdx)
            return true;

        auto val = heights[r][c];
        for (std::size_t i{c + 1}; i <= maxIdx; i++) {
            if (val <= heights[r][i])
                return false;
        }
        return true;
    }

    [[nodiscard]] bool can_see_any(std::size_t r, std::size_t c) const {
        return can_see_left(r, c) || can_see_right(r, c) || can_see_up(r, c) || can_see_down(r, c);
    }

    [[nodiscard]] std::size_t up_dist(std::size_t r, std::size_t c) const {
        if (r == 0)
            return 0;

        std::size_t dist{0};
        auto val = heights[r][c];
        auto stop{false};
        for (std::size_t i{r - 1}; !stop; i--) {
            stop = i == 0;
            dist++;
            if (val <= heights[i][c])
                return dist;
        }
        return dist;
    }

    [[nodiscard]] std::size_t down_dist(std::size_t r, std::size_t c) const {
        auto maxIdx = length() - 1;
        if (r >= maxIdx)
            return 0;

        std::size_t dist{0};
        auto val = heights[r][c];
        for (std::size_t i{r + 1}; i <= maxIdx; i++) {
            dist++;
            if (val <= heights[i][c])
                return dist;
        }
        return dist;
    }

    [[nodiscard]] std::size_t left_dist(std::size_t r, std::size_t c) const {
        if (c == 0)
            return 0;

        std::size_t dist{0};
        auto val = heights[r][c];
        auto stop{false};
        for (std::size_t i{c - 1}; !stop; i--) {
            stop = i == 0;
            dist++;
            if (val <= heights[r][i])
                return dist;
        }
        return dist;
    }

    [[nodiscard]] std::size_t right_dist(std::size_t r, std::size_t c) const {
        auto maxIdx = width() - 1;
        if (c >= maxIdx)
            return 0;

        std::size_t dist{0};
        auto val = heights[r][c];
        for (std::size_t i{c + 1}; i <= maxIdx; i++) {
            dist++;
            if (val <= heights[r][i])
                return dist;
        }
        return dist;
    }

    [[nodiscard]] std::size_t score(std::size_t r, std::size_t c) const {
        return up_dist(r, c) * down_dist(r, c) * left_dist(r, c) * right_dist(r, c);
    }
};

trees get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::vector<std::string> result;
    while (std::getline(input, line))
        result.push_back(line);

    return trees{result};
}

int main() {
    auto input = get_input("input.txt");

    // part 1
    std::size_t count{0};
    for (std::size_t r{0}; r < input.length(); r++) {
        for (std::size_t c{0}; c < input.width(); c++) {
            if (input.can_see_any(r, c))
                count++;
        }
    }
    std::cout << count << '\n';

    // part 2
    std::size_t maxScore{0};
    for (std::size_t r{0}; r < input.length(); r++) {
        for (std::size_t c{0}; c < input.width(); c++) {
            auto score = input.score(r, c);
            if (score > maxScore)
                maxScore = score;
        }
    }
    std::cout << maxScore << '\n';
    return 0;
}
