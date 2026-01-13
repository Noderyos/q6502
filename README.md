# Q6502 (very WIP)

> A single-header 65C02 emulator

[main.c](main.c) contains a minimal implementation with a W65C51 (ACIA) mapped in `$5000-$5003`
using async character input to emulate serial interface. Input file is loaded at address `$8000`

## How to use it

```c
#define Q6502_IMPLEMENTATION
#define Q6502_WDC65C02 // Enable 65C02 CPU
#include "q6502.h"

// You need to provide two functions
// to allow device mapping
// 1. To read memory
// 2. To write memory
extern void write_mem(uint16_t addr, uint8_t value);
extern uint8_t read_mem(uint16_t addr);


int main(void) {
    cpu_init(read_mem, write_mem); // Initial PC set by reading address at $FFFC-$FFFD
    while(1) {
        cpu_step();
    }
}
```

# Implementation

- WDC65C02: 100%
- NMOS6502: 98% of ADC, 99.99% of `JSR a` (1 failed), 100% others except Invalid/Undocumented OpCodes