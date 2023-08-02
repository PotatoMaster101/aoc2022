#include <algorithm>
#include <cctype>
#include <compare>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

// std::strong_ordering::greater    = not in right order
// std::strong_ordering::less       = in right order
// std::strong_ordering::equal      = equal order

struct node {
    std::vector<std::unique_ptr<node>> children;
    std::optional<int> value;
    node() = default;
    explicit node(int value) : value{value} {}
    void add(std::unique_ptr<node>&& child) { children.push_back(std::move(child)); }
};

std::strong_ordering node_cmp(const node& lhs, int rhs) {
    if (lhs.value)
        return *lhs.value <=> rhs;              // int <=> int
    if (lhs.children.empty())
        return std::strong_ordering::less;      // lhs out of items - in right order

    auto result = node_cmp(*lhs.children[0], rhs);
    if (std::is_eq(result))
        return lhs.children.size() == 1 ? std::strong_ordering::equal : std::strong_ordering::greater;
    return result;
}

std::strong_ordering node_cmp(int lhs, const node& rhs) {
    auto result = node_cmp(rhs, lhs);

    // invert result
    if (std::is_lt(result))
        return std::strong_ordering::greater;
    if (std::is_gt(result))
        return std::strong_ordering::less;
    return std::strong_ordering::equal;
}

std::strong_ordering node_cmp(const node& lhs, const node& rhs) {
    if (lhs.value && rhs.value)
        return *lhs.value <=> *rhs.value;   // int <=> int
    if (lhs.value && !rhs.value)
        return node_cmp(*lhs.value, rhs);   // int <=> list
    if (!lhs.value && rhs.value)
        return node_cmp(lhs, *rhs.value);   // list <=> int

    auto limit = std::min(lhs.children.size(), rhs.children.size());
    for (std::size_t i = 0; i < limit; i++) {
        auto result = node_cmp(*lhs.children[i], *rhs.children[i]);
        if (std::is_gt(result) || std::is_lt(result))
            return result;
    }
    if (lhs.children.size() > rhs.children.size())
        return std::strong_ordering::greater;       // right side ran out - not in right order
    else if (lhs.children.size() < rhs.children.size())
        return std::strong_ordering::less;          // left side ran out - in right order
    return std::strong_ordering::equal;
}

std::string node_str(const node& n) {
    if (n.value)
        return std::to_string(*n.value);

    std::string result{"["};
    for (const auto& child : n.children) {
        result += child->value ? std::to_string(*child->value) : node_str(*child);
        if (&child != &n.children.back())
            result += ',';
    }
    result += ']';
    return result;
}

std::unique_ptr<node> parse_num(std::stringstream& stream) {
    std::string numStr;
    while (!stream.eof() && std::isdigit(stream.peek())) {
        numStr += static_cast<char>(stream.peek());
        stream.get();
    }
    return std::make_unique<node>(std::stoi(numStr.empty() ? "0" : numStr));
}

std::unique_ptr<node> parse_list(std::stringstream& stream) {
    if (stream.peek() == '[')
        stream.get();   // skip begin list char '['

    auto result = std::make_unique<node>();
    while (!stream.eof() && stream.peek() != ']') {
        auto peek = stream.peek();
        if (std::isdigit(peek)) {
            result->add(parse_num(stream));
        } else if (peek == '[') {
            result->add(parse_list(stream));
            stream.get();
        } else {
            stream.get();
        }
    }
    return result;
}

std::unique_ptr<node> parse_node(const std::string& line) {
    std::stringstream stream{line};
    return parse_list(stream);
}

std::vector<std::pair<std::string, std::string>> get_input(const std::string& filename) {
    std::ifstream fs{filename};
    std::string line;
    std::vector<std::pair<std::string, std::string>> result;

    std::vector<std::string> lines;
    lines.reserve(2);
    while (std::getline(fs, line)) {
        if (lines.size() == 2) {
            result.emplace_back(lines[0], lines[1]);
            lines.clear();
        }

        if (!line.empty())
            lines.push_back(line);
    }
    result.emplace_back(lines[0], lines[1]);
    return result;
}

int main() {
    // part 1
    auto input = get_input("input.txt");
    std::size_t sum = 0;
    for (std::size_t i = 0; i < input.size(); i++) {
        auto node1 = parse_node(input[i].first);
        auto node2 = parse_node(input[i].second);
        if (std::is_lt(node_cmp(*node1, *node2)))
            sum += i + 1;
    }
    std::cout << sum << '\n';

    // part 2
    std::vector<std::unique_ptr<node>> nodes;
    for (const auto& p : input) {
        nodes.push_back(std::move(parse_node(p.first)));
        nodes.push_back(std::move(parse_node(p.second)));
    }
    nodes.push_back(std::move(parse_node("[[2]]")));
    nodes.push_back(std::move(parse_node("[[6]]")));

    std::ranges::sort(nodes, [](const auto& lhs, const auto& rhs) { return std::is_lt(node_cmp(*lhs, *rhs)); });
    auto twoKey = std::ranges::find_if(nodes, [](const auto& item) { return node_str(*item) == "[[2]]"; });
    auto sixKey = std::ranges::find_if(nodes, [](const auto& item) { return node_str(*item) == "[[6]]"; });
    std::cout << (std::distance(nodes.begin(), twoKey) + 1) * (std::distance(nodes.begin(), sixKey) + 1) << '\n';
    return 0;
}

#pragma clang diagnostic pop
