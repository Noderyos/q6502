#ifndef Q6502_H
#define Q6502_H
#include <stdint.h>
#include <stdlib.h>

struct cpu {
    uint16_t pc;
    uint8_t sp;
    uint8_t a;
    uint8_t x, y;
    uint8_t (*read)(uint16_t addr);
    void (*write)(uint16_t addr, uint8_t value);
    struct {
        uint8_t N:1;
        uint8_t V:1;
        uint8_t u:1;
        uint8_t B:1;
        uint8_t D:1;
        uint8_t I:1;
        uint8_t Z:1;
        uint8_t C:1;
    } flags;
};

struct instruction {
    char *name;
    void (*opcode)(void);
    void (*addr)(void);
    uint8_t cycles;
};

extern struct cpu cpu;

void cpu_step();
void cpu_init(uint8_t (*read)(uint16_t addr), void (*write)(uint16_t addr, uint8_t value));

#endif
#ifdef Q6502_IMPLEMENTATION
#undef Q6502_IMPLEMENTATION

#define ERROR(x) do { fprintf(stderr, "ERROR at %s:%d : %s\n", __FILE__, __LINE__, x); exit(1);} while(0)

struct cpu cpu = {.sp = 0xFD};

uint16_t read_word(uint16_t addr) {
    return cpu.read(addr+1) << 8 | cpu.read(addr);
}

uint8_t read_byte_pc() {
    return cpu.read(cpu.pc++);
}

uint16_t read_word_pc() {
    uint16_t value = read_word(cpu.pc);
    cpu.pc += 2;
    return value;
}

uint16_t addr = 0;
uint8_t value = 0;
int is_value = 0;

void ABS() {
    addr = read_word_pc();
}

void ABI() {
    addr = read_word(read_word_pc() + cpu.x);
}
void ABX() {
    addr = read_word_pc() + cpu.x;
}
void ABY() {
    addr = read_word_pc() + cpu.y;
}
void IND() {
    addr = read_word(read_word_pc());
}
void ACC() {
    is_value = 1; value = cpu.a;
}
void IMM() {
    is_value = 1; value = read_byte_pc();
}
void IMP() {/* Do nothing, it's implied ;) */}
void PCR() {
    int8_t offset = (int8_t)read_byte_pc();
    addr = cpu.pc + offset;
}
void STK() {
    addr = 0x100 + cpu.sp;
}

void ZP0() {
    addr = read_byte_pc();
}
void ZII() {
    addr = read_word(read_byte_pc() + cpu.x);
}
void ZPX() {
    addr = read_byte_pc() + cpu.x;
}
void ZPY() {
    addr = read_byte_pc() + cpu.y;
}
void ZPI() {
    addr = read_word(read_byte_pc());
}
void ZIY() {
    addr = read_word(read_byte_pc()) + cpu.y;
}

#define SET_N(x) do {cpu.flags.N=((x)>>7)&1;} while(0)
#define SET_Z(x) do {cpu.flags.Z=(x)==0;} while(0)
#define SET_C(x) do {cpu.flags.C=x>0xFF;} while(0)

void BBR0() {if((cpu.a & 0x01) == 0) cpu.pc = addr;}
void BBR1() {if((cpu.a & 0x02) == 0) cpu.pc = addr;}
void BBR2() {if((cpu.a & 0x04) == 0) cpu.pc = addr;}
void BBR3() {if((cpu.a & 0x08) == 0) cpu.pc = addr;}
void BBR4() {if((cpu.a & 0x10) == 0) cpu.pc = addr;}
void BBR5() {if((cpu.a & 0x20) == 0) cpu.pc = addr;}
void BBR6() {if((cpu.a & 0x40) == 0) cpu.pc = addr;}
void BBR7() {if((cpu.a & 0x80) == 0) cpu.pc = addr;}

void BBS0() {if(cpu.a & 0x01) cpu.pc = addr;}
void BBS1() {if(cpu.a & 0x02) cpu.pc = addr;}
void BBS2() {if(cpu.a & 0x04) cpu.pc = addr;}
void BBS3() {if(cpu.a & 0x08) cpu.pc = addr;}
void BBS4() {if(cpu.a & 0x10) cpu.pc = addr;}
void BBS5() {if(cpu.a & 0x20) cpu.pc = addr;}
void BBS6() {if(cpu.a & 0x40) cpu.pc = addr;}
void BBS7() {if(cpu.a & 0x80) cpu.pc = addr;}

void RMB0() {cpu.write(addr, cpu.read(addr) & ~(1 << 0));}
void RMB1() {cpu.write(addr, cpu.read(addr) & ~(1 << 1));}
void RMB2() {cpu.write(addr, cpu.read(addr) & ~(1 << 2));}
void RMB3() {cpu.write(addr, cpu.read(addr) & ~(1 << 3));}
void RMB4() {cpu.write(addr, cpu.read(addr) & ~(1 << 4));}
void RMB5() {cpu.write(addr, cpu.read(addr) & ~(1 << 5));}
void RMB6() {cpu.write(addr, cpu.read(addr) & ~(1 << 6));}
void RMB7() {cpu.write(addr, cpu.read(addr) & ~(1 << 7));}

void SMB0() {cpu.write(addr, cpu.read(addr) | (1 << 0));}
void SMB1() {cpu.write(addr, cpu.read(addr) | (1 << 1));}
void SMB2() {cpu.write(addr, cpu.read(addr) | (1 << 2));}
void SMB3() {cpu.write(addr, cpu.read(addr) | (1 << 3));}
void SMB4() {cpu.write(addr, cpu.read(addr) | (1 << 4));}
void SMB5() {cpu.write(addr, cpu.read(addr) | (1 << 5));}
void SMB6() {cpu.write(addr, cpu.read(addr) | (1 << 6));}
void SMB7() {cpu.write(addr, cpu.read(addr) | (1 << 7));}

void ADC() {
    if (!is_value) value = cpu.read(addr);

    uint16_t result = value + cpu.a + cpu.flags.C;

    cpu.flags.V = (~(cpu.a ^ value) & (cpu.a ^ result) & 0x80) != 0;
    cpu.a = result & 0xFF;

    SET_N(cpu.a);SET_Z(cpu.a);SET_C(result);
}
void AND() {
    if (is_value) cpu.a &= value;
    else          cpu.a &= cpu.read(addr);

    SET_N(cpu.a);SET_Z(cpu.a);
}
void ASL() {
    if (is_value) value = cpu.a;
    else          value = cpu.read(addr);

    cpu.flags.C = (value>>7) &1;
    value <<= 1;

    if (is_value) cpu.a = value;
    else          cpu.write(addr, value);

    SET_N(value); SET_Z(value);
}
void BCC() {if (cpu.flags.C == 0) cpu.pc = addr;}
void BCS() {if (cpu.flags.C == 1) cpu.pc = addr;}
void BEQ() {
    if (cpu.flags.Z == 1) cpu.pc = addr;
}
void BIT() {
    if (!is_value) value = cpu.read(addr);
    cpu.flags.Z = (cpu.a & value) == 0;
    cpu.flags.N = (value>>7) & 1;
    cpu.flags.V = (value>>6) & 1;
}
void BMI() {if (cpu.flags.N == 1) cpu.pc = addr;}
void BNE() {if (cpu.flags.Z == 0) cpu.pc = addr;}
void BPL() {if (cpu.flags.N == 0) cpu.pc = addr;}
void BRA() {cpu.pc = addr;}
void BRK() {ERROR("BRK");}
void BVC() {if (cpu.flags.V == 0) cpu.pc = addr;}
void BVS() {if (cpu.flags.V == 1) cpu.pc = addr;}
void CLC() {cpu.flags.C = 0;}
void CLD() {cpu.flags.D = 0;}

void CLI() {cpu.flags.I = 0;}
void CLV() {cpu.flags.V = 0;}
void CMP() {
    if (!is_value) value = cpu.read(addr);
    cpu.flags.C = cpu.a >= value;
    cpu.flags.Z = cpu.a == value;
    cpu.flags.N = ((cpu.a - value) & 0x80) != 0;
}
void CPX() {
    if (!is_value) value = cpu.read(addr);
    cpu.flags.C = cpu.x >= value;
    cpu.flags.Z = cpu.x == value;
    cpu.flags.N = ((cpu.x - value) & 0x80) != 0;
}
void CPY() {
    if (!is_value) value = cpu.read(addr);
    cpu.flags.C = cpu.y >= value;
    cpu.flags.Z = cpu.y == value;
    cpu.flags.N = ((cpu.y - value) & 0x80) != 0;
}

void DEC() {
    if (is_value) {
        value = --cpu.a;
    }
    else {
        value = cpu.read(addr);
        value--;
        cpu.write(addr, value);
    }
    SET_N(value);SET_Z(value);
}
void DEX() {cpu.x--;SET_N(cpu.x);SET_Z(cpu.x);}
void DEY() {cpu.y--;SET_N(cpu.y);SET_Z(cpu.y);}

void EOR() {
    if (is_value) cpu.a ^= value;
    else          cpu.a ^= cpu.read(addr);
    SET_N(cpu.a);SET_Z(cpu.a);
}

void INC() {
    if (is_value) {
        value = ++cpu.a;
    }
    else {
        value = cpu.read(addr);
        value++;
        cpu.write(addr, value);
    }
    SET_N(value);SET_Z(value);
}

void INX() {cpu.x++;SET_N(cpu.x);SET_Z(cpu.x);}
void INY() {cpu.y++;SET_N(cpu.y);SET_Z(cpu.y);}
void JMP() {cpu.pc = addr;}
void JSR() { // Not accurate, real 6502 push PC-1 bc CPU will increase PC after RTS
    uint16_t next_pc = cpu.pc - 1;
    cpu.sp -= 2;
    cpu.write(0x100+(uint8_t)(cpu.sp+1), next_pc>>8);
    cpu.write(0x100+cpu.sp, next_pc & 0xFF);

    cpu.pc = addr;
}
void LDA() {
    if (is_value) cpu.a = value;
    else          cpu.a = cpu.read(addr);
    SET_N(cpu.a);SET_Z(cpu.a);
}

void LDX() {
    if (is_value) cpu.x = value;
    else          cpu.x = cpu.read(addr);
    SET_N(cpu.x);SET_Z(cpu.x);
}

void LDY() {
    if (is_value) cpu.y = value;
    else          cpu.y = cpu.read(addr);
    SET_N(cpu.y);SET_Z(cpu.y);
}

void LSR() {
    if (is_value) value = cpu.a;
    else          value = cpu.read(addr);

    cpu.flags.C = value & 1;
    value >>= 1;

    if (is_value) cpu.a = value;
    else          cpu.write(addr, value);

    SET_N(value); SET_Z(value);
}

void NOP() {/* By definition ;) */}
void ORA() {
    if (is_value) cpu.a |= value;
    else          cpu.a |= cpu.read(addr);
    SET_N(cpu.a);SET_Z(cpu.a);
}
void PHA() {cpu.write(0x100+(uint8_t)(cpu.sp-1), cpu.a);cpu.sp--;}
void PHP() {uint8_t old_b = cpu.flags.B;cpu.flags.B=1;cpu.write(0x100+(uint8_t)(cpu.sp-1), *(uint8_t*)&cpu.flags);cpu.flags.B = old_b;cpu.sp--;}
void PHX() {cpu.write(0x100+(uint8_t)(cpu.sp-1), cpu.x);cpu.sp--;}
void PHY() {cpu.write(0x100+(uint8_t)(cpu.sp-1), cpu.y);cpu.sp--;}

void PLA() {cpu.a = cpu.read(0x100+cpu.sp); cpu.sp++;SET_N(cpu.a);SET_Z(cpu.a);}
void PLP() {*(uint8_t*)&cpu.flags = cpu.read(0x100+cpu.sp); cpu.sp++;}
void PLX() {cpu.x = cpu.read(0x100+cpu.sp); cpu.sp++; SET_N(cpu.x);SET_Z(cpu.x);}
void PLY() {cpu.y = cpu.read(0x100+cpu.sp); cpu.sp++; SET_N(cpu.y);SET_Z(cpu.y);}
void ROL() {
    if (is_value) value = cpu.a;
    else value = cpu.read(addr);

    uint8_t old_c = cpu.flags.C;
    cpu.flags.C = (value>>7) & 1;
    value = value << 1 | old_c;

    if (is_value) cpu.a = value;
    else cpu.write(addr, value);

    SET_N(value);SET_Z(value);
}
void ROR() {
    if (is_value) value = cpu.a;
    else value = cpu.read(addr);

    uint8_t old_c = cpu.flags.C;
    cpu.flags.C = value & 1;
    value = (value >> 1) | (old_c << 7);

    if (is_value) cpu.a = value;
    else cpu.write(addr, value);

    SET_N(value);SET_Z(value);
}
void RTI() {ERROR("Don't have IRQS");}
void RTS() {
    cpu.pc = (cpu.read(0x100+(uint8_t)(cpu.sp + 1)) << 8) | cpu.read(0x100+cpu.sp);
    cpu.sp += 2;
    cpu.pc++;
}

void SBC() {
    if (!is_value) value = cpu.read(addr);
    uint16_t carry_in = cpu.flags.C ? 0 : 1;
    uint16_t result = (uint16_t)cpu.a - value - carry_in;

    cpu.flags.C = result < 0x100; // Special
    cpu.flags.V = ((cpu.a ^ value) & 0x80) && ((cpu.a ^ result) & 0x80);

    cpu.a = (uint8_t)result;
    SET_Z(cpu.a); SET_N(cpu.a);
}

void SEC() {cpu.flags.C = 1;}
void SED() {cpu.flags.D = 1;}
void SEI() {cpu.flags.I = 1;}
void STA() {cpu.write(addr, cpu.a);}
void STX() {cpu.write(addr, cpu.x);}
void STY() {cpu.write(addr, cpu.y);}
void STZ() {cpu.write(addr, 0x00);}

void TAX() {cpu.x = cpu.a;SET_N(cpu.x); SET_Z(cpu.x);}
void TXA() {cpu.a = cpu.x;SET_N(cpu.a); SET_Z(cpu.a);}
void TAY() {cpu.y = cpu.a;SET_N(cpu.y); SET_Z(cpu.y);}
void TYA() {cpu.a = cpu.y;SET_N(cpu.a); SET_Z(cpu.a);}
void TSX() {cpu.x = cpu.sp;}
void TXS() {cpu.sp = cpu.x;}
void TRB() {
    value = cpu.read(addr);
    SET_Z(cpu.a & value);
    cpu.write(addr, value | cpu.a);
}
void TSB() {
    value = cpu.read(addr);
    SET_Z(cpu.a & value);
    cpu.write(addr, value & ~cpu.a);
}

void STP() {ERROR("STP");}
void WAI() {ERROR("Don't have IRQS");}
void XXX() {ERROR("INVALID INSTRUCTION");}

struct instruction instructions[256] = {
    {"BRK",&BRK,&STK,3},{"ORA",&ORA,&ZII,6},{"XXX",&XXX,&XXX,0},{"XXX",&XXX,&XXX,0},
    {"TSB",&TSB,&ZP0,3},{"ORA",&ORA,&ZP0,3},{"ASL",&ASL,&ZP0,3},{"RMB0",&RMB0,&ZP0,3},
    {"PHP",&PHP,&STK,3},{"ORA",&ORA,&IMM,2},{"ASL",&ASL,&ACC,2},{"XXX",&XXX,&XXX,0},
    {"TSB",&TSB,&ABS,4},{"ORA",&ORA,&ABS,4},{"ASL",&ASL,&ABS,4},{"BBR0",&BBR0,&PCR,2},
    {"BPL",&BPL,&PCR,2},{"ORA",&ORA,&ZIY,5},{"ORA",&ORA,&ZPI,5},{"XXX",&XXX,&XXX,0},
    {"TRB",&TRB,&ZP0,3},{"ORA",&ORA,&ZPX,4},{"ASL",&ASL,&ZPX,4},{"RMB1",&RMB1,&ZP0,3},
    {"CLC",&CLC,&IMP,2},{"ORA",&ORA,&ABY,4},{"INC",&INC,&ACC,2},{"XXX",&XXX,&XXX,0},
    {"TRB",&TRB,&ABS,4},{"ORA",&ORA,&ABX,4},{"ASL",&ASL,&ABX,4},{"BBR1",&BBR1,&PCR,2},
    {"JSR",&JSR,&ABS,4},{"AND",&AND,&ZII,6},{"XXX",&XXX,&XXX,0},{"XXX",&XXX,&XXX,0},
    {"BIT",&BIT,&ZP0,3},{"AND",&AND,&ZP0,3},{"ROL",&ROL,&ZP0,3},{"RMB2",&RMB2,&ZP0,3},
    {"PLP",&PLP,&STK,3},{"AND",&AND,&IMM,2},{"ROL",&ROL,&ACC,2},{"XXX",&XXX,&XXX,0},
    {"BIT",&BIT,&ABS,4},{"AND",&AND,&ABS,4},{"ROL",&ROL,&ABS,4},{"BBR2",&BBR2,&PCR,2},
    {"BMI",&BMI,&PCR,2},{"AND",&AND,&ZIY,5},{"AND",&AND,&ZPI,5},{"XXX",&XXX,&XXX,0},
    {"BIT",&BIT,&ZPX,4},{"AND",&AND,&ZPX,4},{"ROL",&ROL,&ZPX,4},{"RMB3",&RMB3,&ZP0,3},
    {"SEC",&SEC,&IMP,2},{"AND",&AND,&ABY,4},{"DEC",&DEC,&ACC,2},{"XXX",&XXX,&XXX,0},
    {"BIT",&BIT,&ABX,4},{"AND",&AND,&ABX,4},{"ROL",&ROL,&ABX,4},{"BBR3",&BBR3,&PCR,2},
    {"RTI",&RTI,&STK,3},{"EOR",&EOR,&ZII,6},{"XXX",&XXX,&XXX,0},{"XXX",&XXX,&XXX,0},
    {"XXX",&XXX,&XXX,0},{"EOR",&EOR,&ZP0,3},{"LSR",&LSR,&ZP0,3},{"RMB4",&RMB4,&ZP0,3},
    {"PHA",&PHA,&STK,3},{"EOR",&EOR,&IMM,2},{"LSR",&LSR,&ACC,2},{"XXX",&XXX,&XXX,0},
    {"JMP",&JMP,&ABS,4},{"EOR",&EOR,&ABS,4},{"LSR",&LSR,&ABS,4},{"BBR4",&BBR4,&PCR,2},
    {"BVC",&BVC,&PCR,2},{"EOR",&EOR,&ZIY,5},{"EOR",&EOR,&ZPI,5},{"XXX",&XXX,&XXX,0},
    {"XXX",&XXX,&XXX,0},{"EOR",&EOR,&ZPX,4},{"LSR",&LSR,&ZPX,4},{"RMB5",&RMB5,&ZP0,3},
    {"CLI",&CLI,&IMP,2},{"EOR",&EOR,&ABY,4},{"PHY",&PHY,&STK,3},{"XXX",&XXX,&XXX,0},
    {"XXX",&XXX,&XXX,0},{"EOR",&EOR,&ABX,4},{"LSR",&LSR,&ABX,4},{"BBR5",&BBR5,&PCR,2},
    {"RTS",&RTS,&STK,3},{"ADC",&ADC,&ZII,6},{"XXX",&XXX,&XXX,0},{"XXX",&XXX,&XXX,0},
    {"STZ",&STZ,&ZP0,3},{"ADC",&ADC,&ZP0,3},{"ROR",&ROR,&ZP0,3},{"RMB6",&RMB6,&ZP0,3},
    {"PLA",&PLA,&STK,3},{"ADC",&ADC,&IMM,2},{"ROR",&ROR,&ACC,2},{"XXX",&XXX,&XXX,0},
    {"JMP",&JMP,&IND,6},{"ADC",&ADC,&ABS,4},{"ROR",&ROR,&ABS,4},{"BBR6",&BBR6,&PCR,2},
    {"BVS",&BVS,&PCR,2},{"ADC",&ADC,&ZIY,5},{"ADC",&ADC,&ZPI,5},{"XXX",&XXX,&XXX,0},
    {"STZ",&STZ,&ZPX,4},{"ADC",&ADC,&ZPX,4},{"ROR",&ROR,&ZPX,4},{"RMB7",&RMB7,&ZP0,3},
    {"SEI",&SEI,&IMP,2},{"ADC",&ADC,&ABY,4},{"PLY",&PLY,&STK,3},{"XXX",&XXX,&XXX,0},
    {"JMP",&JMP,&ABI,6},{"ADC",&ADC,&ABX,4},{"ROR",&ROR,&ABX,4},{"BBR7",&BBR7,&PCR,2},
    {"BRA",&BRA,&PCR,2},{"STA",&STA,&ZII,6},{"XXX",&XXX,&XXX,0},{"XXX",&XXX,&XXX,0},
    {"STY",&STY,&ZP0,3},{"STA",&STA,&ZP0,3},{"STX",&STX,&ZP0,3},{"SMB0",&SMB0,&ZP0,3},
    {"DEY",&DEY,&IMP,2},{"BIT",&BIT,&IMM,2},{"TXA",&TXA,&IMP,2},{"XXX",&XXX,&XXX,0},
    {"STY",&STY,&ABS,4},{"STA",&STA,&ABS,4},{"STX",&STX,&ABS,4},{"BBS0",&BBS0,&PCR,2},
    {"BCC",&BCC,&PCR,2},{"STA",&STA,&ZIY,5},{"STA",&STA,&ZPI,5},{"XXX",&XXX,&XXX,0},
    {"STY",&STY,&ZPX,4},{"STA",&STA,&ZPX,4},{"STX",&STX,&ZPY,4},{"SMB1",&SMB1,&ZP0,3},
    {"TYA",&TYA,&IMP,2},{"STA",&STA,&ABY,4},{"TXS",&TXS,&IMP,2},{"XXX",&XXX,&XXX,0},
    {"STZ",&STZ,&ABS,4},{"STA",&STA,&ABX,4},{"STZ",&STZ,&ABX,4},{"BBS1",&BBS1,&PCR,2},
    {"LDY",&LDY,&IMM,2},{"LDA",&LDA,&ZII,6},{"LDX",&LDX,&IMM,2},{"XXX",&XXX,&XXX,0},
    {"LDY",&LDY,&ZP0,3},{"LDA",&LDA,&ZP0,3},{"LDX",&LDX,&ZP0,3},{"SMB2",&SMB2,&ZP0,3},
    {"TAY",&TAY,&IMP,2},{"LDA",&LDA,&IMM,2},{"TAX",&TAX,&IMP,2},{"XXX",&XXX,&XXX,0},
    {"LDY",&LDY,&ACC,2},{"LDA",&LDA,&ABS,4},{"LDX",&LDX,&ABS,4},{"BBS2",&BBS2,&PCR,2},
    {"BCS",&BCS,&PCR,2},{"LDA",&LDA,&ZIY,5},{"LDA",&LDA,&ZPI,5},{"XXX",&XXX,&XXX,0},
    {"LDY",&LDY,&ZPX,4},{"LDA",&LDA,&ZPX,4},{"LDX",&LDX,&ZPY,4},{"SMB3",&SMB3,&ZP0,3},
    {"CLV",&CLV,&IMP,2},{"LDA",&LDA,&ABY,4},{"TSX",&TSX,&IMP,2},{"XXX",&XXX,&XXX,0},
    {"LDY",&LDY,&ABX,4},{"LDA",&LDA,&ABX,4},{"LDX",&LDX,&ABY,4},{"BBS3",&BBS3,&PCR,2},
    {"CPY",&CPY,&IMM,2},{"CMP",&CMP,&ZII,6},{"XXX",&XXX,&XXX,0},{"XXX",&XXX,&XXX,0},
    {"CPY",&CPY,&ZP0,3},{"CMP",&CMP,&ZP0,3},{"DEC",&DEC,&ZP0,3},{"SMB4",&SMB4,&ZP0,3},
    {"INY",&INY,&IMP,2},{"CMP",&CMP,&IMM,2},{"DEX",&DEX,&IMP,2},{"WAI",&WAI,&IMP,2},
    {"CPY",&CPY,&ABS,4},{"CMP",&CMP,&ABS,4},{"DEC",&DEC,&ABS,4},{"BBS4",&BBS4,&PCR,2},
    {"BNE",&BNE,&PCR,2},{"CMP",&CMP,&ZIY,5},{"CMP",&CMP,&ZPI,5},{"XXX",&XXX,&XXX,0},
    {"XXX",&XXX,&XXX,0},{"CMP",&CMP,&ZPX,4},{"DEC",&DEC,&ZPX,4},{"SMB5",&SMB5,&ZP0,3},
    {"CLD",&CLD,&IMP,2},{"CMP",&CMP,&ABY,4},{"PHX",&PHX,&STK,3},{"STP",&STP,&IMP,2},
    {"XXX",&XXX,&XXX,0},{"CMP",&CMP,&ABX,4},{"DEC",&DEC,&ABX,4},{"BBS5",&BBS5,&PCR,2},
    {"CPX",&CPX,&IMM,2},{"SBC",&SBC,&ZII,6},{"XXX",&XXX,&XXX,0},{"XXX",&XXX,&XXX,0},
    {"CPX",&CPX,&ZP0,3},{"SBC",&SBC,&ZP0,3},{"INC",&INC,&ZP0,3},{"SMB6",&SMB6,&ZP0,3},
    {"INX",&INX,&IMP,2},{"SBC",&SBC,&IMM,2},{"NOP",&NOP,&IMP,2},{"XXX",&XXX,&XXX,0},
    {"CPX",&CPX,&ABS,4},{"SBC",&SBC,&ABS,4},{"INC",&INC,&ABS,4},{"BBS6",&BBS6,&PCR,2},
    {"BEQ",&BEQ,&PCR,2},{"SBC",&SBC,&ZIY,5},{"SBC",&SBC,&ZPI,5},{"XXX",&XXX,&XXX,0},
    {"XXX",&XXX,&XXX,0},{"SBC",&SBC,&ZPX,4},{"INC",&INC,&ZPX,4},{"SMB7",&SMB7,&ZP0,3},
    {"SED",&SED,&IMP,2},{"SBC",&SBC,&ABY,4},{"PLX",&PLX,&STK,3},{"XXX",&XXX,&XXX,0},
    {"XXX",&XXX,&XXX,0},{"SBC",&SBC,&ABX,4},{"INC",&INC,&ABX,4},{"BBS7",&BBS7,&PCR,2}
};

void cpu_init(uint8_t (*read)(uint16_t addr), void (*write)(uint16_t addr, uint8_t value)) {
    cpu.read = read;
    cpu.write = write;
    cpu.pc = cpu.read(0xFFFD) << 8 | cpu.read(0xFFFC);
    cpu.sp = 0xFD;
    cpu.a = 0;
    cpu.x = 0;
    cpu.y = 0;
    cpu.flags.N = 0;
    cpu.flags.V = 0;
    cpu.flags.u = 1;
    cpu.flags.B = 0;
    cpu.flags.D = 0;
    cpu.flags.I = 0;
    cpu.flags.Z = 0;
    cpu.flags.C = 0;
}

void cpu_step() {
    is_value = 0;
    uint8_t byte = cpu.read(cpu.pc++);
    struct instruction opcode = instructions[byte];
    opcode.addr();
    opcode.opcode();
}

#endif