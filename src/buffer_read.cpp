#include "buffer_read.h"

BufferRead::BufferRead(std::istream& input) : input_(input) {
    size_ = 0;
    cur_bit_ = 0;
}

bool BufferRead::IsFileEnded() const {
    return is_file_ended_;
}

void BufferRead::FillBuffer() {
    input_.read(buffer_, BUFFER_SIZE);
    size_ = input_.gcount();
    cur_bit_ = 0;
}

bool BufferRead::ReadBit() {
    if (cur_bit_ == 8 * size_) {
        FillBuffer();
        if (size_ == 0) {
            is_file_ended_ = true;
            return false;
        }
    }

    bool bit = (buffer_[cur_bit_ >> 3] >> ((cur_bit_ & 7))) & 1;
    ++cur_bit_;
    return bit;
}

bool BufferRead::ReadRBit() {
    if (cur_bit_ == 8 * size_) {
        FillBuffer();
        if (size_ == 0) {
            is_file_ended_ = true;
            return false;
        }
    }

    bool bit = (buffer_[cur_bit_ >> 3] >> (7 - (cur_bit_ & 7))) & 1;
    ++cur_bit_;
    return bit;
}

int16_t BufferRead::ReadSBits() {
    int16_t res = 0;
    for (size_t i = 0; i < syslen_; ++i) {
        res ^= ReadRBit() << (syslen_ - 1 - i);
    }
    return res;
}

char BufferRead::ReadChar() {
    uint8_t res = 0;
    for (size_t i = 0; i < 8; ++i) {
        res ^= ReadBit() << i;
    }
    return static_cast<char>(res);
}