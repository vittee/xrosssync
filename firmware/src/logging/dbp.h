#pragma once

#include <Arduino.h>

#ifdef XROSSSYNC_DEBUG
#include "UDPPrint.h"
#endif

void hexdump(const uint8_t* data, size_t len, Print* p = &Serial);