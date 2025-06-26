#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <signal.h>
#include <stdlib.h>

#define Q6502_IMPLEMENTATION
#include "q6502.h"

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
}

sig_atomic_t stop = 0;

void handle_sigint(int sig) {
    (void)sig;
    stop = 1;
}


uint8_t ram[0x8000] = {0};
uint8_t rom[0x8000] = {0};

void write_mem(uint16_t addr, uint8_t value) {
    if (addr < 0x4000)
        ram[addr] = value;

    // ACIA_DATA
    if (addr == 0x5000) {
        putchar(value);
        fflush(stdout);
    }

    if (addr & 0x8000)
        rom[addr-0x8000] = value;
}

uint8_t read_mem(uint16_t addr) {
    if (addr < 0x4000) return ram[addr];

    // ACIA_DATA
    if (addr == 0x5000) {
        uint8_t v = getchar();
        return v == 0xA ? 0xD : v;
    }
    // ACIA_STATUS
    if (addr == 0x5001) return kbhit() ? 8 : 0;

    if (addr & 0x8000) return rom[addr-0x8000];
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    fread(&rom, 1, 0x8000, f);
    fclose(f);

    signal(SIGINT, handle_sigint);
    // Raw mode
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    cpu_init(read_mem, write_mem);
    while (!stop) {
        cpu_step();
    }

    // Disable raw mode
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    return 0;
}