#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

struct file_node {
    std::string name;
    std::size_t size;

    file_node(std::string name, std::size_t size)
        : name{std::move(name)}, size{size} {}
};

struct file_sys {
    std::vector<std::string> curr_path;
    std::map<std::string, std::vector<file_node>> path_map;

    void add_file(const file_node& file) {
        for (const auto& path : curr_path)
            path_map[path].push_back(file);
    }

    void go_root() {
        curr_path.clear();
        curr_path.emplace_back("/");
        path_map.clear();
        path_map.insert({"/", {}});
    }

    void go_back() {
        if (curr_path.size() > 1)
            curr_path.pop_back();
    }

    void go_dir(const std::string& dir) {
        auto currName = curr_path.back();
        auto newName = currName.back() == '/' ? currName + dir : currName + "/" + dir;
        curr_path.push_back(newName);
        path_map.insert({newName, {}});
    }

    [[nodiscard]] std::size_t sum_size(const std::string& dir) const {
        std::size_t result{0};
        for (const auto& file : path_map.at(dir))
            result += file.size;

        return result;
    }
};

std::vector<std::string> get_input(const std::string& filename) {
    std::ifstream input{filename};
    std::string line;
    std::vector<std::string> result;
    while (std::getline(input, line))
        result.push_back(line);

    return result;
}

void handle_cd(file_sys& fileSys, const std::string& dir) {
    if (dir == "/")
        fileSys.go_root();
    else if (dir == "..")
        fileSys.go_back();
    else
        fileSys.go_dir(dir);
}

void handle_filesize(file_sys& fileSys, const std::string& line) {
    auto space = line.find(' ');
    auto first = line.substr(0, space);
    auto second = line.substr(space);
    fileSys.add_file(file_node{second, std::stoull(first)});
}

file_sys run_input(const std::vector<std::string>& input) {
    file_sys fileSys;

    for (std::size_t i{0}; i < input.size(); i++) {
        if (input[i].starts_with("$ cd "))
            handle_cd(fileSys, input[i].substr(5));
        else if (std::isdigit(input[i][0]))
            handle_filesize(fileSys, input[i]);
    }
    return fileSys;
}

int main() {
    auto input = get_input("input.txt");
    auto sizes = run_input(input);

    // part 1
    std::vector<std::size_t> atMost100k;
    for (const auto& path : sizes.path_map) {
        auto size = sizes.sum_size(path.first);
        if (size <= 100000)
            atMost100k.push_back(size);
    }
    std::cout << std::accumulate(atMost100k.begin(), atMost100k.end(), 0ull) << '\n';

    // part 2
    auto freeSpace = 70000000 - sizes.sum_size("/");
    auto deleteSpace = 30000000 - freeSpace;
    std::size_t minSizeDir{70000000};
    for (const auto& path : sizes.path_map) {
        auto size = sizes.sum_size(path.first);
        if (size >= deleteSpace && size < minSizeDir)
            minSizeDir = size;
    }
    std::cout << minSizeDir << '\n';
    return 0;
}
