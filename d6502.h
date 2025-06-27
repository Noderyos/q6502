#ifndef D6502_H
#define D6502_H
#include <stdint.h>
#include <string.h>

#define END "\033[0m"
#define BOLD "\033[1m"
#define UNDER "\033[4m"
#define RED "\033[91m"
#define GREEN "\033[92m"
#define YELLOW "\033[93m"
#define BLUE "\033[94m"
#define CYAN "\033[96m"

typedef enum {
    A_ABS, A_ABI, A_ABX, A_ABY,

    A_IND, A_ACC, A_IMM, A_IMP,
    A_PCR, A_STK,

    A_ZP0, A_ZII, A_ZPX, A_ZPY,
    A_ZPI, A_ZIY,

    A_XXX

} disasm_addr;
struct disasm_inst {
    char *name;
    disasm_addr addr;
};

void disasm_step(char *buffer, size_t size);
void disasm_init(uint8_t (*read)(uint16_t addr), void (*write)(uint16_t addr, uint8_t value));
void disasm_set_pc(uint16_t pc);
uint16_t disasm_get_pc();

#endif
#ifdef D6502_IMPLEMENTATION
#undef D6502_IMPLEMENTATION

struct disasm {
    uint16_t pc;
    uint8_t (*read)(uint16_t addr);
    void (*write)(uint16_t addr, uint8_t value);
} disasm = {};

uint16_t d_rw(uint16_t addr) {return disasm.read(addr+1) << 8 | disasm.read(addr);}
uint8_t d_rb_pc() {return disasm.read(disasm.pc++);}
uint16_t d_rw_pc() {
    uint16_t value = d_rw(disasm.pc);
    disasm.pc += 2;
    return value;
}

struct disasm_inst disasm_instructions[256] = {
    {"BRK",A_STK},{"ORA",A_ZII},{"XXX",A_XXX},{"XXX",A_XXX},
    {"TSB",A_ZP0},{"ORA",A_ZP0},{"ASL",A_ZP0},{"RMB0",A_ZP0},
    {"PHP",A_STK},{"ORA",A_IMM},{"ASL",A_ACC},{"XXX",A_XXX},
    {"TSB",A_ABS},{"ORA",A_ABS},{"ASL",A_ABS},{"BBR0",A_PCR},
    {"BPL",A_PCR},{"ORA",A_ZIY},{"ORA",A_ZPI},{"XXX",A_XXX},
    {"TRB",A_ZP0},{"ORA",A_ZPX},{"ASL",A_ZPX},{"RMB1",A_ZP0},
    {"CLC",A_IMP},{"ORA",A_ABY},{"INC",A_ACC},{"XXX",A_XXX},
    {"TRB",A_ABS},{"ORA",A_ABX},{"ASL",A_ABX},{"BBR1",A_PCR},
    {"JSR",A_ABS},{"AND",A_ZII},{"XXX",A_XXX},{"XXX",A_XXX},
    {"BIT",A_ZP0},{"AND",A_ZP0},{"ROL",A_ZP0},{"RMB2",A_ZP0},
    {"PLP",A_STK},{"AND",A_IMM},{"ROL",A_ACC},{"XXX",A_XXX},
    {"BIT",A_ABS},{"AND",A_ABS},{"ROL",A_ABS},{"BBR2",A_PCR},
    {"BMI",A_PCR},{"AND",A_ZIY},{"AND",A_ZPI},{"XXX",A_XXX},
    {"BIT",A_ZPX},{"AND",A_ZPX},{"ROL",A_ZPX},{"RMB3",A_ZP0},
    {"SEC",A_IMP},{"AND",A_ABY},{"DEC",A_ACC},{"XXX",A_XXX},
    {"BIT",A_ABX},{"AND",A_ABX},{"ROL",A_ABX},{"BBR3",A_PCR},
    {"RTI",A_STK},{"EOR",A_ZII},{"XXX",A_XXX},{"XXX",A_XXX},
    {"XXX",A_XXX},{"EOR",A_ZP0},{"LSR",A_ZP0},{"RMB4",A_ZP0},
    {"PHA",A_STK},{"EOR",A_IMM},{"LSR",A_ACC},{"XXX",A_XXX},
    {"JMP",A_ABS},{"EOR",A_ABS},{"LSR",A_ABS},{"BBR4",A_PCR},
    {"BVC",A_PCR},{"EOR",A_ZIY},{"EOR",A_ZPI},{"XXX",A_XXX},
    {"XXX",A_XXX},{"EOR",A_ZPX},{"LSR",A_ZPX},{"RMB5",A_ZP0},
    {"CLI",A_IMP},{"EOR",A_ABY},{"PHY",A_STK},{"XXX",A_XXX},
    {"XXX",A_XXX},{"EOR",A_ABX},{"LSR",A_ABX},{"BBR5",A_PCR},
    {"RTS",A_STK},{"ADC",A_ZII},{"XXX",A_XXX},{"XXX",A_XXX},
    {"STZ",A_ZP0},{"ADC",A_ZP0},{"ROR",A_ZP0},{"RMB6",A_ZP0},
    {"PLA",A_STK},{"ADC",A_IMM},{"ROR",A_ACC},{"XXX",A_XXX},
    {"JMP",A_IND},{"ADC",A_ABS},{"ROR",A_ABS},{"BBR6",A_PCR},
    {"BVS",A_PCR},{"ADC",A_ZIY},{"ADC",A_ZPI},{"XXX",A_XXX},
    {"STZ",A_ZPX},{"ADC",A_ZPX},{"ROR",A_ZPX},{"RMB7",A_ZP0},
    {"SEI",A_IMP},{"ADC",A_ABY},{"PLY",A_STK},{"XXX",A_XXX},
    {"JMP",A_ABI},{"ADC",A_ABX},{"ROR",A_ABX},{"BBR7",A_PCR},
    {"BRA",A_PCR},{"STA",A_ZII},{"XXX",A_XXX},{"XXX",A_XXX},
    {"STY",A_ZP0},{"STA",A_ZP0},{"STX",A_ZP0},{"SMB0",A_ZP0},
    {"DEY",A_IMP},{"BIT",A_IMM},{"TXA",A_IMP},{"XXX",A_XXX},
    {"STY",A_ABS},{"STA",A_ABS},{"STX",A_ABS},{"BBS0",A_PCR},
    {"BCC",A_PCR},{"STA",A_ZIY},{"STA",A_ZPI},{"XXX",A_XXX},
    {"STY",A_ZPX},{"STA",A_ZPX},{"STX",A_ZPY},{"SMB1",A_ZP0},
    {"TYA",A_IMP},{"STA",A_ABY},{"TXS",A_IMP},{"XXX",A_XXX},
    {"STZ",A_ABS},{"STA",A_ABX},{"STZ",A_ABX},{"BBS1",A_PCR},
    {"LDY",A_IMM},{"LDA",A_ZII},{"LDX",A_IMM},{"XXX",A_XXX},
    {"LDY",A_ZP0},{"LDA",A_ZP0},{"LDX",A_ZP0},{"SMB2",A_ZP0},
    {"TAY",A_IMP},{"LDA",A_IMM},{"TAX",A_IMP},{"XXX",A_XXX},
    {"LDY",A_ACC},{"LDA",A_ABS},{"LDX",A_ABS},{"BBS2",A_PCR},
    {"BCS",A_PCR},{"LDA",A_ZIY},{"LDA",A_ZPI},{"XXX",A_XXX},
    {"LDY",A_ZPX},{"LDA",A_ZPX},{"LDX",A_ZPY},{"SMB3",A_ZP0},
    {"CLV",A_IMP},{"LDA",A_ABY},{"TSX",A_IMP},{"XXX",A_XXX},
    {"LDY",A_ABX},{"LDA",A_ABX},{"LDX",A_ABY},{"BBS3",A_PCR},
    {"CPY",A_IMM},{"CMP",A_ZII},{"XXX",A_XXX},{"XXX",A_XXX},
    {"CPY",A_ZP0},{"CMP",A_ZP0},{"DEC",A_ZP0},{"SMB4",A_ZP0},
    {"INY",A_IMP},{"CMP",A_IMM},{"DEX",A_IMP},{"WAI",A_IMP},
    {"CPY",A_ABS},{"CMP",A_ABS},{"DEC",A_ABS},{"BBS4",A_PCR},
    {"BNE",A_PCR},{"CMP",A_ZIY},{"CMP",A_ZPI},{"XXX",A_XXX},
    {"XXX",A_XXX},{"CMP",A_ZPX},{"DEC",A_ZPX},{"SMB5",A_ZP0},
    {"CLD",A_IMP},{"CMP",A_ABY},{"PHX",A_STK},{"STP",A_IMP},
    {"XXX",A_XXX},{"CMP",A_ABX},{"DEC",A_ABX},{"BBS5",A_PCR},
    {"CPX",A_IMM},{"SBC",A_ZII},{"XXX",A_XXX},{"XXX",A_XXX},
    {"CPX",A_ZP0},{"SBC",A_ZP0},{"INC",A_ZP0},{"SMB6",A_ZP0},
    {"INX",A_IMP},{"SBC",A_IMM},{"NOP",A_IMP},{"XXX",A_XXX},
    {"CPX",A_ABS},{"SBC",A_ABS},{"INC",A_ABS},{"BBS6",A_PCR},
    {"BEQ",A_PCR},{"SBC",A_ZIY},{"SBC",A_ZPI},{"XXX",A_XXX},
    {"XXX",A_XXX},{"SBC",A_ZPX},{"INC",A_ZPX},{"SMB7",A_ZP0},
    {"SED",A_IMP},{"SBC",A_ABY},{"PLX",A_STK},{"XXX",A_XXX},
    {"XXX",A_XXX},{"SBC",A_ABX},{"INC",A_ABX},{"BBS7",A_PCR}
};

void disasm_set_pc(uint16_t pc) {disasm.pc = pc;}
uint16_t disasm_get_pc() {return disasm.pc;}

void disasm_init(uint8_t (*read)(uint16_t addr), void (*write)(uint16_t addr, uint8_t value)) {
    disasm.read = read;
    disasm.write = write;
    disasm.pc = disasm.read(0xFFFD) << 8 | disasm.read(0xFFFC);
}

void disasm_step(char *buffer, size_t size) {
    memset(buffer, 0, size);
    struct disasm_inst opcode = disasm_instructions[disasm.read(disasm.pc++)];

    buffer = stpcpy(buffer, opcode.name);

    switch (opcode.addr) {
        case A_ABS:sprintf(buffer, GREEN" $%04X"END, d_rw_pc());break;
        case A_ABI:sprintf(buffer, " ("GREEN"$%04X"END", X)", d_rw_pc());break;
        case A_ABX:sprintf(buffer, " "GREEN"$%04X"END", X", d_rw_pc());break;
        case A_ABY:sprintf(buffer, " "GREEN"$%04X"END", Y", d_rw_pc());break;
        case A_IND:sprintf(buffer, " ("GREEN"$%04X"END")", d_rw_pc());break;
        case A_ACC:break;
        case A_IMM:sprintf(buffer, " "BLUE"#$%02X"END, d_rb_pc());break;
        case A_IMP:break;
        case A_PCR:sprintf(buffer, " "GREEN"$%04X"END" P", disasm.pc + (int8_t)d_rb_pc()+1);break;
        case A_STK:break;
        case A_ZP0:sprintf(buffer, " "GREEN"$%02X"END, d_rb_pc());break;
        case A_ZII:sprintf(buffer, " ("GREEN"$%02X"END", X)", d_rb_pc());break;
        case A_ZPX:sprintf(buffer, " "GREEN"$%02X"END", X", d_rb_pc());break;
        case A_ZPY:sprintf(buffer, " "GREEN"$%02X"END", Y", d_rb_pc());break;
        case A_ZPI:sprintf(buffer, " ("GREEN"$%02X"END")", d_rb_pc());break;
        case A_ZIY:sprintf(buffer, " ("GREEN"$%02X"END"), Y", d_rb_pc());break;
        case A_XXX:sprintf(buffer, " XXX");break;
    }
    buffer[strlen(buffer)] = 0;
}

#endif