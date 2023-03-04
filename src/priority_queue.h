#pragma once
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

using std::size_t;

template <typename S, typename Container>
class Heap {
public:
    void Add(S element) {
        st_.push_back(element);
        SiftUp(st_.size() - 1);
    }
    S ExtractMin() {
        S min = st_[0];
        st_[0] = st_[st_.size() - 1];
        st_.pop_back();
        SiftDown(0);
        return min;
    }
    size_t Size() {
        return st_.size();
    }

private:
    void SiftDown(size_t v) {
        while (2 * v + 1 < st_.size()) {
            size_t left = 2 * v + 1;
            size_t right = 2 * v + 2;
            size_t u = left;
            if (right < st_.size() && st_[right] < st_[left]) {
                u = right;
            }
            if (!(st_[u] < st_[v])) {
                break;
            }
            std::swap(st_[v], st_[u]);
            v = u;
        }
    }

    void SiftUp(size_t v) {
        while (v > 0 && st_[v] < st_[(v - 1) / 2]) {
            std::swap(st_[v], st_[(v - 1) / 2]);
            v = (v - 1) / 2;
        }
    }

    Container st_;
};