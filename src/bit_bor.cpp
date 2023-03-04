#include "bit_bor.h"

bool Node::IsTerminal() const {
    return (childs[0] == childs[1]);
}

Trie::Trie() {
    root_ = 1;
    last_node_ = 2;
    cur_node_ = 1;
    nodes_.push_back(Node());
    nodes_.push_back(Node());
}

Trie::Trie(std::vector<Node> nodes, size_t root) : root_(root), cur_node_(root), nodes_(nodes) {
    nodes_.push_back(Node());
}

void Trie::Add(int64_t str, int16_t val, size_t len) {
    size_t cur = root_;
    for (size_t i = 0; i < len; ++i) {
        bool bit = (str >> (len - 1 - i)) & 1;
        if (nodes_[cur].childs[bit] != 0) {
            cur = nodes_[cur].childs[bit];
        } else {
            nodes_.push_back(Node());
            nodes_[cur].childs[bit] = last_node_;
            cur = last_node_;
            ++last_node_;
        }
    }
    nodes_[cur].val = val;
}

bool Trie::Go(bool bit, int16_t &symb) {
    cur_node_ = nodes_[cur_node_].childs[bit];
    if (nodes_[cur_node_].IsTerminal()) {
        symb = nodes_[cur_node_].val;
        cur_node_ = root_;
        return true;
    } else {
        return false;
    }
}

void Trie::Dfs(std::vector<std::pair<int16_t, std::string>> &result, size_t vertex, std::string &path,
               size_t depth) const {
    if (vertex == 0) {
        vertex = root_;
    }
    if (nodes_[vertex].IsTerminal()) {
        result.push_back({nodes_[vertex].val, path});
        return;
    }
    path += '0';
    Dfs(result, nodes_[vertex].childs[0], path, depth + 1);
    path.pop_back();
    path += '1';
    Dfs(result, nodes_[vertex].childs[1], path, depth + 1);
    path.pop_back();
}

void Trie::GetCodes(std::vector<std::pair<int16_t, std::string>> &result) const {
    std::string path;
    Dfs(result, 0, path, 0);
}