#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <tuple>
#include "priority_queue.h"
#include "bit_bor.h"

struct CanonCode {
    std::vector<int16_t> chars;
    std::vector<uint64_t> codes;
    std::vector<size_t> size;
    std::size_t Size() const {
        return chars.size();
    }
};

struct NodeInfo {
    size_t count;
    int16_t symb;
    size_t node;
    NodeInfo(size_t count, int16_t symb, size_t node) : count(count), symb(symb), node(node) {
    }

    bool operator<(const NodeInfo& other) const {
        if (count != other.count) {
            return count < other.count;
        }
        int16_t symb_first = symb;
        if (symb_first < 0) {
            symb_first += 256;
        }
        int16_t symb_second = other.symb;
        if (symb_second < 0) {
            symb_second += 256;
        }
        return symb_first < symb_second;
    }
};

Trie Haffman(std::unordered_map<int16_t, size_t> symb_cnt) {
    std::vector<Node> trie(symb_cnt.size() * 2 + 10);
    size_t last_node = 1;
    Heap<NodeInfo, std::vector<NodeInfo>> heap;
    for (auto [symb, count] : symb_cnt) {
        trie[last_node].val = symb;
        heap.Add(NodeInfo({count, symb, last_node}));
        ++last_node;
    }
    while (heap.Size() >= 2) {
        auto min1 = heap.ExtractMin();
        auto min2 = heap.ExtractMin();
        trie[last_node].childs[0] = min1.node;
        trie[last_node].childs[1] = min2.node;
        heap.Add(NodeInfo({min1.count + min2.count, std::min(min1.symb, min2.symb), last_node}));
        ++last_node;
    }
    return Trie(trie, last_node - 1);
}

CanonCode CanonicalCode(const Trie& trie) {
    std::vector<std::pair<int16_t, std::string>> codes;
    trie.GetCodes(codes);
    std::sort(codes.begin(), codes.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second.size() == rhs.second.size()
                   ? lhs.first + (256) * (lhs.first < 0) < rhs.first + (256) * (rhs.first < 0)
                   : lhs.second.size() < rhs.second.size();
    });
    CanonCode canon;
    size_t len = codes.size();
    canon.chars.resize(len);
    canon.codes.resize(len);
    canon.size.resize(len);
    for (size_t i = 0; i < len; ++i) {
        canon.chars[i] = codes[i].first;
        canon.size[i] = codes[i].second.size();
        if (i == 0) {
            canon.codes[i] = 0;
        } else {
            canon.codes[i] = (canon.codes[i - 1] + 1) << (canon.size[i] - canon.size[i - 1]);
        }
    }
    return canon;
}
