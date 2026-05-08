#include "cpu.h"
#include "mem.h"

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
    case 0x00:
        PC++;
        I = 1;
        push16(PC);
        push8(status());
        PC = *at(0xFFFE, 0) | (*at(0xFFFF, 0) << 8);
        break;
    case 0x01: ora(indir_x(0)); break;
    case 0x05: ora(zero_page()); break;
    case 0x06: asl(zero_page()); break;
    case 0x08: push8(status()); break;
    case 0x09: ora(immediate()); break;
    case 0x0A: asl(accumulator()); break;
    case 0x0D: ora(absolute(0)); break;
    case 0x0E: asl(absolute(1)); break;
    case 0x10: bch(N == 0); break;
    case 0x11: ora(indir_y(0)); break;
    case 0x15: ora(zero_page_x()); break;
    case 0x16: asl(zero_page_x()); break;
    case 0x18: C = 0; break;
    case 0x19: ora(absolute_y(0)); break;
    case 0x1D: ora(absolute_x(0)); break;
    case 0x1E: asl(absolute_x(1)); break;
    case 0x20: push16(PC + 1); PC = fetchWord(); break;
    case 0x21: nnd(indir_x(0)); break;
    case 0x24: bit(zero_page()); break;
    case 0x25: nnd(zero_page()); break;
    case 0x26: rol(zero_page()); break;
    case 0x28: loadStatus(pull8()); break;
    case 0x29: nnd(immediate()); break;
    case 0x2A: rol(accumulator()); break;
    case 0x2C: bit(absolute(0)); break;
    case 0x2D: nnd(absolute(0)); break;
    case 0x2E: rol(absolute(1)); break;
    case 0x30: bch(N); break;
    case 0x31: nnd(indir_y(0)); break;
    case 0x35: nnd(zero_page_x()); break;
    case 0x36: rol(zero_page_x()); break;
    case 0x38: C = 1; break;
    case 0x39: nnd(absolute_y(0)); break;
    case 0x3D: nnd(absolute_x(0)); break;
    case 0x3E: rol(absolute_x(1)); break;
    case 0x40:
        loadStatus(pull8());
        PC = pull16();
        break;
    case 0x41: eor(indir_x(0)); break;
    case 0x45: eor(zero_page()); break;
    case 0x46: lsr(zero_page()); break;
    case 0x48: push8(A); break;
    case 0x49: eor(immediate()); break;
    case 0x4A: lsr(accumulator()); break;
    case 0x4C: PC = fetchWord(); break;
    case 0x4D: eor(absolute(0)); break;
    case 0x4E: lsr(absolute(1)); break;
    case 0x50: bch(V == 0); break;
    case 0x51: eor(indir_y(0)); break;
    case 0x55: eor(zero_page_x()); break;
    case 0x56: lsr(zero_page_x()); break;
    case 0x58: I = 0; break;
    case 0x59: eor(absolute_y(0)); break;
    case 0x5D: eor(absolute_x(0)); break;
    case 0x5E: lsr(absolute_x(1)); break;
    case 0x60: PC = pull16() + 1; break;
    case 0x61: adc(indir_x(0)); break;
    case 0x65: adc(zero_page()); break;
    case 0x66: ror(zero_page()); break;
    case 0x68:
        A = pull8();
        Z = A == 0;
        N = A >> 7;
        break;
    case 0x69: adc(immediate()); break;
    case 0x6A: ror(accumulator()); break;
    case 0x6C: PC = indir(); break;
    case 0x6D: adc(absolute(0)); break;
    case 0x6E: ror(absolute(1)); break;
    case 0x70: bch(V); break;
    case 0x71: adc(indir_y(0)); break;
    case 0x75: adc(zero_page_x()); break;
    case 0x76: ror(zero_page_x()); break;
    case 0x78: I = 1; break;
    case 0x79: adc(absolute_y(0)); break;
    case 0x7D: adc(absolute_x(0)); break;
    case 0x7E: ror(absolute_x(1)); break;
    case 0x81: *indir_x(1) = A; break;
    case 0x84: *zero_page() = Y; break;
    case 0x85: *zero_page() = A; break;
    case 0x86: *zero_page() = X; break;
    case 0x88: dec(&Y); break;
    case 0x8A: loa(&A, &X); break;
    case 0x8C: *absolute(1) = Y; break;
    case 0x8D: *absolute(1) = A; break;
    case 0x8E: *absolute(1) = X; break;
    case 0x90: bch(C == 0); break;
    case 0x91: *indir_y(1) = A; break;
    case 0x94: *zero_page_x() = Y; break;
    case 0x95: *zero_page_x() = A; break;
    case 0x96: *zero_page_y() = X; break;
    case 0x98: loa(&A, &Y); break;
    case 0x99: *absolute_y(1) = A; break;
    case 0x9A: SP = X; break;
    case 0x9D: *absolute_x(1) = A; break;
    case 0xA0: loa(&Y, immediate()); break;
    case 0xA1: loa(&A, indir_x(0)); break;
    case 0xA2: loa(&X, immediate()); break;
    case 0xA4: loa(&Y, zero_page()); break;
    case 0xA5: loa(&A, zero_page()); break;
    case 0xA6: loa(&X, zero_page()); break;
    case 0xA8: loa(&Y, &A); break;
    case 0xA9: loa(&A, immediate()); break;
    case 0xAA: loa(&X, &A); break;
    case 0xAC: loa(&Y, absolute(0)); break;
    case 0xAD: {
        uint8_t* val = absolute(0);   
        loa(&A, val);
        if ((*at(PC - 2, 0) | (*at(PC - 1, 0) << 8)) == 0xC000) {
            *val = 0x0;
        }
        break;
    }
    case 0xAE: loa(&X, absolute(0)); break;
    case 0xB0: bch(C); break;
    case 0xB1: loa(&A, indir_y(0)); break;
    case 0xB4: loa(&Y, zero_page_x()); break;
    case 0xB5: loa(&A, zero_page_x()); break;
    case 0xB6: loa(&X, zero_page_y()); break;
    case 0xB8: V = 0; break;
    case 0xB9: loa(&A, absolute_y(0)); break;
    case 0xBA: loa(&X, &SP); break;
    case 0xBC: loa(&Y, absolute_x(0)); break;
    case 0xBD: loa(&A, absolute_x(0)); break;
    case 0xBE: loa(&X, absolute_y(0)); break;
    case 0xC0: cmp(&Y, immediate()); break;
    case 0xC1: cmp(&A, indir_x(0)); break;
    case 0xC4: cmp(&Y, zero_page()); break;
    case 0xC5: cmp(&A, zero_page()); break;
    case 0xC6: dec(zero_page()); break;
    case 0xC8: inc(&Y); break;
    case 0xC9: cmp(&A, immediate()); break;
    case 0xCA: dec(&X); break;
    case 0xCC: cmp(&Y, absolute(0)); break;
    case 0xCD: cmp(&A, absolute(0)); break;
    case 0xCE: dec(absolute(1)); break;
    case 0xD0: bch(Z == 0); break;
    case 0xD1: cmp(&A, indir_y(0)); break;
    case 0xD5: cmp(&A, zero_page_x()); break;
    case 0xD6: dec(zero_page_x()); break;
    case 0xD8: D = 0; break;
    case 0xD9: cmp(&A, absolute_y(0)); break;
    case 0xDD: cmp(&A, absolute_x(0)); break;
    case 0xDE: dec(absolute_x(1)); break;
    case 0xE0: cmp(&X, immediate()); break;
    case 0xE1: sbc(indir_x(0)); break;
    case 0xE4: cmp(&X, zero_page()); break;
    case 0xE5: sbc(zero_page()); break;
    case 0xE6: inc(zero_page()); break;
    case 0xE8: inc(&X); break;
    case 0xE9: sbc(immediate()); break;
    case 0xEA: break;
    case 0xEC: cmp(&X, absolute(0)); break;
    case 0xED: sbc(absolute(0)); break;
    case 0xEE: inc(absolute(1)); break;
    case 0xF0: bch(Z); break;
    case 0xF1: sbc(indir_y(0)); break;
    case 0xF5: sbc(zero_page_x()); break;
    case 0xF6: inc(zero_page_x()); break;
    case 0xF8: D = 1; break;
    case 0xF9: sbc(absolute_y(0)); break;
    case 0xFD: sbc(absolute_x(0)); break;
    case 0xFE: inc(absolute_x(1)); break;
    }

    return opcode;
}