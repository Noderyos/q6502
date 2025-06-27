#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define Q6502_IMPLEMENTATION
#include "q6502.h"

#define D6502_IMPLEMENTATION
#include "d6502.h"

uint8_t ram[0x8000] = {0};
uint8_t rom[0x8000] = {0};

void write_mem(uint16_t addr, uint8_t value) {
    if (addr < 0x4000) ram[addr] = value;
    if (addr & 0x8000) rom[addr-0x8000] = value;
}

uint8_t read_mem(uint16_t addr) {
    if (addr < 0x4000) return ram[addr];
    if (addr & 0x8000) return rom[addr-0x8000];
    return 0;
}

void show_registers() {
    printf(BLUE"[--------- Registers --------]\n"END);
    printf(GREEN"A"END" :"BLUE" %02X    "
           GREEN"X"END":"BLUE" %02X    "
           GREEN"Y"END":"BLUE" %02X\n"
           GREEN"SP"END":"BLUE" %02X\n"
           GREEN"PC"END":"BLUE" %04X\n"
           GREEN"FLAGS"END":"BLUE" %c%c%c%c%c%c%c%c\n"END,
           cpu.a, cpu.x, cpu.y, cpu.sp, cpu.pc,
           cpu.flags.N?'N':'-', cpu.flags.V?'V':'-', cpu.flags.u?'1':'e', cpu.flags.B?'B':'-',
           cpu.flags.D?'D':'-', cpu.flags.I?'I':'-', cpu.flags.Z?'Z':'-', cpu.flags.C?'C':'-');
}

void show_stack(unsigned int depth) {
    printf(BLUE"[----------- Stack ----------]\n"END);
    for (unsigned int i = 0; i < depth; i++) {
        printf("%02d| "BLUE"%02X"END" -> %02X\n", i, (uint8_t)(cpu.sp+i), cpu.read(0x100+(uint8_t)(cpu.sp+i)));
    }
}

void show_code(unsigned int depth) {
    char dis_buffer[16];
    printf(BLUE"[----------- Code -----------]\n"END);
    disasm_set_pc(cpu.pc);
    for (unsigned int i = 0; i < depth; i++) {
        uint16_t pc = disasm_get_pc();
        disasm_step(dis_buffer, 15);
        printf("%s "BLUE"%04X"END": %s\n", i == 0 ? "==>" : "   ", pc, dis_buffer);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    fread(&rom, 1, 0x8000, f);
    fclose(f);

    char prompt[512];

    disasm_init(read_mem, write_mem);
    cpu_init(read_mem, write_mem);
    while (1) {
        show_registers();
        show_stack(8);
        show_code(8);
        printf(BLUE"[----------------------------]\n"END);
        printf(CYAN">>> "END);
        if (fgets(prompt, 512, stdin)) {
            prompt[strcspn(prompt, "\n")] = '\0';
            printf("'%s'\n", prompt);
        }

        cpu_step();
    }
}