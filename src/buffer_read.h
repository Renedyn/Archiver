#pragma once
#include <cstddef>
#include <fstream>
#include <string>
#include <iostream>

class BufferRead {
public:
    explicit BufferRead(std::istream& input);

    bool ReadBit();
    bool ReadRBit();
    char ReadChar();
    int16_t ReadSBits();
    bool IsFileEnded() const;

private:
    void FillBuffer();

    std::istream& input_;
    static const size_t BUFFER_SIZE = 1024 * 8;
    char buffer_[BUFFER_SIZE];
    const size_t syslen_ = 9;
    bool is_file_ended_ = false;
    size_t size_;
    size_t cur_bit_;
};
