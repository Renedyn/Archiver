#pragma once
#include <sys/types.h>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <string>
#include <vector>
#include <iostream>

struct Node {
    size_t childs[2] = {0, 0};
    int16_t val;
    bool IsTerminal() const;
};

class Trie {
public:
    Trie();

    Trie(std::vector<Node> nodes, size_t root);

    void Add(int64_t str, int16_t val, size_t len);
    bool Go(bool bit, int16_t &symb);

    void GetCodes(std::vector<std::pair<int16_t, std::string>> &result) const;

private:
    void Dfs(std::vector<std::pair<int16_t, std::string>> &result, size_t vertex, std::string &path,
             size_t depth) const;

    size_t root_;
    size_t last_node_;
    size_t cur_node_;
    std::vector<Node> nodes_;
};
