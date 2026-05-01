#include "dbp.h"

void hexdump(const uint8_t* data, size_t len, Print* p)
{
    for (size_t i = 0; i < len; i += 16) {
        p->printf("\033[0;33m%04X\033[0;37m  ", i);
        size_t row = min((size_t)16, len - i);
        for (size_t j = 0; j < 16; j++) {
            if (j < row) p->printf("%02X ", data[i + j]);
            else p->print("   ");
            if (j == 7) p->print(" ");
        }
        p->print(" \033[0;2m|\033[0;36m");
        for (size_t j = 0; j < row; j++) {
            char c = data[i + j];
            p->print(isprint(c) ? c : '.');
        }
        p->println("\033[0;2m|\033[0;37m");
    }
}


