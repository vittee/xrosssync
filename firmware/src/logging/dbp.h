#pragma once

#include <Arduino.h>

#ifdef XROSSSYNC_DEBUG
#include "UDPPrint.h"
inline Print *dbp = &udpPrint;
#else
inline Print *dbp = &Serial;
#endif

void hexdump(const uint8_t* data, size_t len, Print* p = &Serial);