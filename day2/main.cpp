#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

const auto LoseScore = 0;
const auto DrawScore = 3;
const auto WinScore = 6;
const auto RockScore = 1;
const auto PaperScore = 2;
const auto ScissorsScore = 3;

enum class Outcome {
    Win = 0,
    Draw = 1,
    Lose = 2
};

bool is_rock(char c) {
    return c == 'A' || c == 'X';
}

bool is_paper(char c) {
    return c == 'B' || c == 'Y';
}

bool is_scissors(char c) {
    return c == 'C' || c == 'Z';
}

bool is_lose(char c) {
    return c == 'X';
}

bool is_draw(char c) {
    return c == 'Y';
}

bool is_win(char c) {
    return c == 'Z';
}

int part1_get_score(char player, Outcome outcome) {
    auto addScore = outcome == Outcome::Win ? WinScore : (outcome == Outcome::Draw ? DrawScore : LoseScore);
    if (is_rock(player))
        return RockScore + addScore;
    if (is_paper(player))
        return PaperScore + addScore;
    return ScissorsScore + addScore;
}

int part1_get_score(char opponent, char player) {
    if (is_rock(opponent) && is_rock(player))
        return part1_get_score(player, Outcome::Draw);
    if (is_rock(opponent) && is_paper(player))
        return part1_get_score(player, Outcome::Win);
    if (is_rock(opponent) && is_scissors(player))
        return part1_get_score(player, Outcome::Lose);
    if (is_paper(opponent) && is_rock(player))
        return part1_get_score(player, Outcome::Lose);
    if (is_paper(opponent) && is_paper(player))
        return part1_get_score(player, Outcome::Draw);
    if (is_paper(opponent) && is_scissors(player))
        return part1_get_score(player, Outcome::Win);
    if (is_scissors(opponent) && is_rock(player))
        return part1_get_score(player, Outcome::Win);
    if (is_scissors(opponent) && is_paper(player))
        return part1_get_score(player, Outcome::Lose);
    return part1_get_score(player, Outcome::Draw);
}

int part2_get_score(char opponent, char outcome) {
    if (is_rock(opponent) && is_win(outcome))
        return PaperScore + WinScore;
    if (is_rock(opponent) && is_draw(outcome))
        return RockScore + DrawScore;
    if (is_rock(opponent) && is_lose(outcome))
        return ScissorsScore + LoseScore;
    if (is_paper(opponent) && is_win(outcome))
        return ScissorsScore + WinScore;
    if (is_paper(opponent) && is_draw(outcome))
        return PaperScore + DrawScore;
    if (is_paper(opponent) && is_lose(outcome))
        return RockScore + LoseScore;
    if (is_scissors(opponent) && is_win(outcome))
        return RockScore + WinScore;
    if (is_scissors(opponent) && is_draw(outcome))
        return ScissorsScore + DrawScore;
    return PaperScore + LoseScore;
}

std::vector<std::pair<char, char>> get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::vector<std::pair<char, char>> result;

    while (std::getline(input, line)) {
        if (line.size() == 3)
            result.emplace_back(line[0], line[2]);
    }
    return result;
}

int main() {
    auto input = get_input("input.txt");
    int score{0};

    // part 1
    for (const auto& game : input)
        score += part1_get_score(game.first, game.second);

    std::cout << score << '\n';

    // part 2
    score = 0;
    for (const auto& game : input)
        score += part2_get_score(game.first, game.second);

    std::cout << score << '\n';
    return 0;
}
