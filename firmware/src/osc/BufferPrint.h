#pragma once

#include <Arduino.h>

class BufferPrint : public Print {
public:
    BufferPrint(uint8_t* buf, size_t maxLen);
    size_t write(uint8_t byte) override;
    size_t write(const uint8_t* data, size_t len) override;
    size_t size() const;
    void reset();

private:
    uint8_t* buf;
    size_t maxLen;
    size_t pos;
};