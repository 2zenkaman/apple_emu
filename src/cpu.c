#include "cpu.h"
#include "mem.h"
// #include "carry.h"
// #include "instr.h"

extern uint8_t A;
extern uint8_t X;
extern uint8_t Y;

extern uint16_t PC;
extern uint8_t  SP;

extern uint8_t C;
extern uint8_t Z;
extern uint8_t I;
extern uint8_t D;
extern uint8_t B;
extern uint8_t V;
extern uint8_t N;

extern uint8_t MEM[MEMSIZ];

uint16_t reset() {
    return PC = MEM[0xFFFC] | (MEM[0xFFFD] << 8);
}

uint8_t fetchByte() {
    return *at(PC++, 0);
}

uint16_t fetchWord() {
    return fetchByte() | (fetchByte() << 8);
}

void push8(uint8_t val) {
    *at(SP + 0x0100, 1) = val;
    SP--;
}

void push16(uint16_t val) {
    push8(val >> 8);
    push8(val);
}

uint8_t pull8() {
    SP++;
    return *at(SP + 0x100, 0);
}

uint16_t pull16() {
    return pull8() | (pull8() << 8);
}

uint8_t* immediate() {
    return at(PC++, 0);
}

uint8_t* accumulator() {
    return &A;
}

uint8_t* zero_page() {
    return at(fetchByte(), 1);
}

uint8_t* zero_page_x() {
    uint8_t ptr = fetchByte() + X;
    return at(ptr, 1);
}

uint8_t* zero_page_y() {
    uint8_t ptr = fetchByte() + Y;
    return at(ptr, 1);
}

uint8_t* absolute(uint8_t w) {
    return at(fetchWord(), w);
}

uint8_t* absolute_x(uint8_t w) {
    return at(fetchWord() + X, w);
}

uint8_t* absolute_y(uint8_t w) {
    return at(fetchWord() + Y, w);
}

uint16_t indir() {
    uint16_t addr = fetchWord();
    uint8_t tarl = *at(addr, 1);
    uint8_t tarh = *at(addr + 1, 1);
    return tarl | (tarh << 8);
}

uint8_t* indir_x(uint8_t w) {
    uint8_t zpptr = fetchByte() + X;
    uint16_t target = *at(zpptr, 0) | (*at(zpptr + 1, 0) << 8);
    return at(target, w);
}

uint8_t* indir_y(uint8_t w) {
    uint8_t zpptr = fetchByte();
    uint16_t target = *at(zpptr, 0) | (*at(zpptr + 1, 0) << 8);
    return at(target + Y, w);
}

uint8_t overflow(uint8_t a, uint8_t b) {
    uint8_t sum = a + b;
    uint8_t sumsign = sum >> 7;
    uint8_t asign = a >> 7;
    uint8_t bsign = b >> 7;

    return (asign == bsign) && (asign != sumsign);
}

uint8_t carry(uint8_t a, uint8_t b) {
    uint16_t sum = a + b;
    return (sum >> 8) != 0;
}

void adc(uint8_t* val) {
    uint8_t tempC = C;
    uint8_t sum = A + *val;
    V = overflow(A, *val) || overflow(sum, C);
    C = carry(A, *val) || carry(sum, C);
    A += *val + tempC;

    Z = A == 0;
    N = A >> 7;
}

void nnd(uint8_t* val) {
    A &= *val;

    Z = A == 0;
    N = A >> 7;
}

void asl(uint8_t* target) {
    C = !!(*target & 0x80);
    *target <<= 1;
    Z = *target == 0;
    N = *target >> 7;
}

void bch(uint8_t when) {
    uint8_t displacement = fetchByte();
    if (when) PC += *(int8_t*)&displacement;
}

void bit(uint8_t* val) {
    Z = !(A & *val);
    V = *val & 0x40;
    N = *val >> 7;
}

void cmp(uint8_t* r, uint8_t* m) {
    C = *r >= *m;
    Z = *r == *m;
    N = (*r - *m) >> 7;
}

void dec(uint8_t* target) {
    (*target)--;
    
    Z = *target == 0;
    N = *target >> 7;
}

void eor(uint8_t* val) {
    A ^= *val;

    Z = A == 0;
    N = A >> 7;
}

void inc(uint8_t* target) {
    (*target)++;
    
    Z = *target == 0;
    N = *target >> 7;
}

void loa(uint8_t* target, uint8_t* src) {
    *target = *src;

    Z = *target == 0;
    N = *target >> 7;
}

void lsr(uint8_t* target) {
    C = !!(*target & 1);
    *target >>= 1;

    Z = *target == 0;
    N = *target >> 7;
}

void ora(uint8_t* val) {
    A |= *val;

    Z = A == 0;
    N = A >> 7;
}

void rol(uint8_t* target) {
    uint8_t temp = C;
    C = !!(*target & 0x80);
    *target <<= 1;
    *target |= temp;

    Z = *target == 0;
    N = *target >> 7;
}

void ror(uint8_t* target) {
    uint8_t temp = C;
    C = !!(*target & 1);
    *target >>= 1;
    *target |= (temp << 7);

    Z = *target == 0;
    N = *target >> 7;
}

void sbc(uint8_t* val) {
    uint8_t comp = ~(*val);
    adc(&comp);
}

uint8_t status() {
    return C | (Z << 1) | (I << 2) | (D << 3) | (B << 4) | (V << 6) | (N << 7);
}

void loadStatus(uint8_t value) {
    C = (value >> 0) & 1;
    Z = (value >> 1) & 1;
    I = (value >> 2) & 1;
    D = (value >> 3) & 1;
    B = (value >> 4) & 1;
    V = (value >> 6) & 1;
    N = (value >> 7) & 1;
}

uint8_t exec() {
    uint8_t opcode = fetchByte();
    
    switch (opcode) {
    case BRK:
        PC++;
        I = 1;
        push16(PC);
        push8(status());
        PC = *at(0xFFFE, 0) | (*at(0xFFFF, 0) << 8);
        break;
    case ORA_INDX: ora(indir_x(0)); break;
    case ORA_ZP: ora(zero_page()); break;
    case ASL_ZP: asl(zero_page()); break;
    case PHP: push8(status()); break;
    case ORA_IM: ora(immediate()); break;
    case ASL_AC: asl(accumulator()); break;
    case ORA_ABS: ora(absolute(0)); break;
    case ASL_ABS: asl(absolute(1)); break;
    case BPL: bch(N == 0); break;
    case ORA_INDY: ora(indir_y(0)); break;
    case ORA_ZPX: ora(zero_page_x()); break;
    case ASL_ZPX: asl(zero_page_x()); break;
    case CLC: C = 0; break;
    case ORA_ABSY: ora(absolute_y(0)); break;
    case ORA_ABSX: ora(absolute_x(0)); break;
    case ASL_ABSX: asl(absolute_x(1)); break;
    case JSR: push16(PC + 1); PC = fetchWord(); break;
    case AND_INDX: nnd(indir_x(0)); break;
    case BIT_ZP: bit(zero_page()); break;
    case AND_ZP: nnd(zero_page()); break;
    case ROL_ZP: rol(zero_page()); break;
    case PLP: loadStatus(pull8()); break;
    case AND_IM: nnd(immediate()); break;
    case ROL_AC: rol(accumulator()); break;
    case BIT_ABS: bit(absolute(0)); break;
    case AND_ABS: nnd(absolute(0)); break;
    case ROL_ABS: rol(absolute(1)); break;
    case BMI: bch(N); break;
    case AND_INDY: nnd(indir_y(0)); break;
    case AND_ZPX: nnd(zero_page_x()); break;
    case ROL_ZPX: rol(zero_page_x()); break;
    case SEC: C = 1; break;
    case AND_ABSY: nnd(absolute_y(0)); break;
    case AND_ABSX: nnd(absolute_x(0)); break;
    case ROL_ABSX: rol(absolute_x(1)); break;
    case RTI:
        loadStatus(pull8());
        PC = pull16();
        break;
    case EOR_INDX: eor(indir_x(0)); break;
    case EOR_ZP: eor(zero_page()); break;
    case LSR_ZP: lsr(zero_page()); break;
    case PHA: push8(A); break;
    case EOR_IM: eor(immediate()); break;
    case LSR_AC: lsr(accumulator()); break;
    case JMP_ABS: PC = fetchWord(); break;
    case EOR_ABS: eor(absolute(0)); break;
    case LSR_ABS: lsr(absolute(1)); break;
    case BVC: bch(V == 0); break;
    case EOR_INDY: eor(indir_y(0)); break;
    case EOR_ZPX: eor(zero_page_x()); break;
    case LSR_ZPX: lsr(zero_page_x()); break;
    case CLI: I = 0; break;
    case EOR_ABSY: eor(absolute_y(0)); break;
    case EOR_ABSX: eor(absolute_x(0)); break;
    case LSR_ABSX: lsr(absolute_x(1)); break;
    case RTS: PC = pull16() + 1; break;
    case ADC_INDX: adc(indir_x(0)); break;
    case ADC_ZP: adc(zero_page()); break;
    case ROR_ZP: ror(zero_page()); break;
    case PLA:
        A = pull8();
        Z = A == 0;
        N = A >> 7;
        break;
    case ADC_IM: adc(immediate()); break;
    case ROR_AC: ror(accumulator()); break;
    case JMP_IND: PC = indir(); break;
    case ADC_ABS: adc(absolute(0)); break;
    case ROR_ABS: ror(absolute(1)); break;
    case BVS: bch(V); break;
    case ADC_INDY: adc(indir_y(0)); break;
    case ADC_ZPX: adc(zero_page_x()); break;
    case ROR_ZPX: ror(zero_page_x()); break;
    case SEI: I = 1; break;
    case ADC_ABSY: adc(absolute_y(0)); break;
    case ADC_ABSX: adc(absolute_x(0)); break;
    case ROR_ABSX: ror(absolute_x(1)); break;
    case STA_INDX: *indir_x(1) = A; break;
    case STY_ZP: *zero_page() = Y; break;
    case STA_ZP: *zero_page() = A; break;
    case STX_ZP: *zero_page() = X; break;
    case DEY: dec(&Y); break;
    case TXA: loa(&A, &X); break;
    case STY_ABS: *absolute(1) = Y; break;
    case STA_ABS: *absolute(1) = A; break;
    case STX_ABS: *absolute(1) = X; break;
    case BCC: bch(C == 0); break;
    case STA_INDY: *indir_y(1) = A; break;
    case STY_ZPX: *zero_page_x() = Y; break;
    case STA_ZPX: *zero_page_x() = A; break;
    case STX_ZPY: *zero_page_y() = X; break;
    case TYA: loa(&A, &Y); break;
    case STA_ABSY: *absolute_y(1) = A; break;
    case TXS: SP = X; break;
    case STA_ABSX: *absolute_x(1) = A; break;
    case LDY_IM: loa(&Y, immediate()); break;
    case LDA_INDX: loa(&A, indir_x(0)); break;
    case LDX_IM: loa(&X, immediate()); break;
    case LDY_ZP: loa(&Y, zero_page()); break;
    case LDA_ZP: loa(&A, zero_page()); break;
    case LDX_ZP: loa(&X, zero_page()); break;
    case TAY: loa(&Y, &A); break;
    case LDA_IM: loa(&A, immediate()); break;
    case TAX: loa(&X, &A); break;
    case LDY_ABS: loa(&Y, absolute(0)); break;
    case LDA_ABS: loa(&A, absolute(0)); break;
    case LDX_ABS: loa(&X, absolute(0)); break;
    case BCS: bch(C); break;
    case LDA_INDY: loa(&A, indir_y(0)); break;
    case LDY_ZPX: loa(&Y, zero_page_x()); break;
    case LDA_ZPX: loa(&A, zero_page_x()); break;
    case LDX_ZPY: loa(&X, zero_page_y()); break;
    case CLV: V = 0; break;
    case LDA_ABSY: loa(&A, absolute_y(0)); break;
    case TSX: loa(&X, &SP); break;
    case LDY_ABSX: loa(&Y, absolute_x(0)); break;
    case LDA_ABSX: loa(&A, absolute_x(0)); break;
    case LDX_ABSY: loa(&X, absolute_y(0)); break;
    case CPY_IM: cmp(&Y, immediate()); break;
    case CMP_INDX: cmp(&A, indir_x(0)); break;
    case CPY_ZP: cmp(&Y, zero_page()); break;
    case CMP_ZP: cmp(&A, zero_page()); break;
    case DEC_ZP: dec(zero_page()); break;
    case INY: inc(&Y); break;
    case CMP_IM: cmp(&A, immediate()); break;
    case DEX: dec(&X); break;
    case CPY_ABS: cmp(&Y, absolute(0)); break;
    case CMP_ABS: cmp(&A, absolute(0)); break;
    case DEC_ABS: dec(absolute(1)); break;
    case BNE: bch(Z == 0); break;
    case CMP_INDY: cmp(&A, indir_y(0)); break;
    case CMP_ZPX: cmp(&A, zero_page_x()); break;
    case DEC_ZPX: dec(zero_page_x()); break;
    case CLD: D = 0; break;
    case CMP_ABSY: cmp(&A, absolute_y(0)); break;
    case CMP_ABSX: cmp(&A, absolute_x(0)); break;
    case DEC_ABSX: dec(absolute_x(1)); break;
    case CPX_IM: cmp(&X, immediate()); break;
    case SBC_INDX: sbc(indir_x(0)); break;
    case CPX_ZP: cmp(&X, zero_page()); break;
    case SBC_ZP: sbc(zero_page()); break;
    case INC_ZP: inc(zero_page()); break;
    case INX: inc(&X); break;
    case SBC_IM: sbc(immediate()); break;
    case NOP: break;
    case CPX_ABS: cmp(&X, absolute(0)); break;
    case SBC_ABS: sbc(absolute(0)); break;
    case INC_ABS: inc(absolute(1)); break;
    case BEQ: bch(Z); break;
    case SBC_INDY: sbc(indir_y(0)); break;
    case SBC_ZPX: sbc(zero_page_x()); break;
    case INC_ZPX: inc(zero_page_x()); break;
    case SED: D = 1; break;
    case SBC_ABSY: sbc(absolute_y(0)); break;
    case SBC_ABSX: sbc(absolute_x(0)); break;
    case INC_ABSX: inc(absolute_x(1)); break;
    }

    return opcode;
}