#pragma once
#include <inuos.h>

#define PIT_DATA_PORT 0x40
#define PIT_CMD_PORT 0x43

#define PIT_INT_TIMER 0x06
#define PIT_BINARY 0
#define PIT_ZERO_COUNTER 0
#define PIT_FREQUENCY 1193182
#define PIT_FULL_RW 0x30

Void PitInitialize();