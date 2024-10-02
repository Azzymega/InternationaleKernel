#include <inufw/api.h>
#include <pio/api.h>
#include <pit/api.h>

#define SYSTEM_TIMER_RATE 1

Void PitInitialize() {
    FwDebugPrint("Initializing system timer on Irq %i, Interrupt %i \r\n",0,32);
    FwDebugPrint("System timer frequency is %i hertz \r\n",SYSTEM_TIMER_RATE*60);
    const UInt32 hz = SYSTEM_TIMER_RATE;
    const UInt32 divisor = PIT_FREQUENCY / hz;
    outb(PIT_CMD_PORT, PIT_BINARY | PIT_INT_TIMER | PIT_FULL_RW | PIT_ZERO_COUNTER);
    outb(PIT_DATA_PORT, divisor);
    outb(PIT_DATA_PORT, divisor >> 8);
}