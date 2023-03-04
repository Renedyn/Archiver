#pragma once
#include <cstddef>
#include <fstream>
#include <string>
#include <iostream>
#include <cstring>

class BufferWritter {
public:
    explicit BufferWritter(std::ostream& output);

    void WriteBit(bool val);
    void WriteChar(char val);
    void WriteSBits(int16_t val);
    void WriteKBits(int16_t val, size_t k);
    void ClearBuffer();

private:
    std::ostream& output_;
    static const size_t BUFFER_SIZE = 1024 * 8;
    char buffer_[BUFFER_SIZE];
    const size_t syslen_ = 9;
    size_t cur_bit_;
};
