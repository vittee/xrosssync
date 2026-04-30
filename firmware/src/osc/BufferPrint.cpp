#include "BufferPrint.h"

BufferPrint::BufferPrint(uint8_t* buf, size_t maxLen)
    : buf(buf), maxLen(maxLen), pos(0)
{

}

size_t BufferPrint::write(uint8_t byte) {
    if (pos >= maxLen) {
        return 0;
    }

    buf[pos++] = byte;
    return 1;
}

size_t BufferPrint::write(const uint8_t* data, size_t len) {
    size_t toWrite = min(len, maxLen - pos);
    memcpy(buf + pos, data, toWrite);
    pos += toWrite;
    return toWrite;
}

size_t BufferPrint::size() const {
    return pos;
}

void BufferPrint::reset() {
    pos = 0;
}