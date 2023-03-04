#include "bit_writter.h"

BufferWritter::BufferWritter(std::ostream& output) : output_(output) {
    cur_bit_ = 0;
    std::memset(buffer_, 0, sizeof(buffer_));
}

void BufferWritter::ClearBuffer() {
    output_.write(buffer_, static_cast<int64_t>((cur_bit_ + 7) / 8));
    cur_bit_ = 0;
    std::memset(buffer_, 0, sizeof(buffer_));
}

void BufferWritter::WriteBit(bool val) {
    if (cur_bit_ == 8 * BUFFER_SIZE) {
        ClearBuffer();
    }
    buffer_[cur_bit_ >> 3] |= (static_cast<int>(val) << (7 - (cur_bit_ & 7)));  // NOLINT
    ++cur_bit_;
}

void BufferWritter::WriteSBits(int16_t val) {
    if (val < 0) {
        val += 256;
    }
    for (size_t i = 0; i < syslen_; ++i) {
        WriteBit((val >> (syslen_ - 1 - i)) & 1);
    }
}

void BufferWritter::WriteKBits(int16_t val, size_t k) {
    for (size_t i = 0; i < k; ++i) {
        WriteBit((val >> (k - 1 - i)) & 1);
    }
}

void BufferWritter::WriteChar(char val) {
    for (size_t i = 0; i < 8; ++i) {
        WriteBit((val >> (7 - i)) & 1);
    }
}