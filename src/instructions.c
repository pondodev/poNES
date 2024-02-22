#include "instructions.h"

#include "device.h"

#include "raylib.h"

#include <string.h>

typedef void (*InstrExecFunc)(const InstrInfo* instr);

static inline void _instr_fetch_bytes(void* buf, size_t size);
static void _instr_unknown(const InstrInfo* instr);
static void _instr_adc(const InstrInfo* instr);
static void _instr_and(const InstrInfo* instr);
static void _instr_asl(const InstrInfo* instr);
static void _instr_bcc(const InstrInfo* instr);
static void _instr_bcs(const InstrInfo* instr);
static void _instr_beq(const InstrInfo* instr);
static void _instr_bit(const InstrInfo* instr);
static void _instr_bmi(const InstrInfo* instr);
static void _instr_bne(const InstrInfo* instr);
static void _instr_bpl(const InstrInfo* instr);
static void _instr_brk(const InstrInfo* instr);
static void _instr_bvc(const InstrInfo* instr);
static void _instr_bvs(const InstrInfo* instr);
static void _instr_clc(const InstrInfo* instr);
static void _instr_cld(const InstrInfo* instr);
static void _instr_cli(const InstrInfo* instr);
static void _instr_clv(const InstrInfo* instr);
static void _instr_cmp(const InstrInfo* instr);
static void _instr_cpx(const InstrInfo* instr);
static void _instr_cpy(const InstrInfo* instr);
static void _instr_dec(const InstrInfo* instr);
static void _instr_dex(const InstrInfo* instr);
static void _instr_dey(const InstrInfo* instr);
static void _instr_eor(const InstrInfo* instr);
static void _instr_nop(const InstrInfo* instr);

static InstrExecFunc s_instr_exec_funcs[kINSTRTYPE_COUNT];
#define REG_INSTR(_alias, _func) s_instr_exec_funcs[_alias] = _func

void instr_init(void) {
    // TODO: remove once we have all instruction handlers implemented
    for (size_t i = 0; i < kINSTRTYPE_COUNT; ++i) {
        s_instr_exec_funcs[i] = _instr_unknown;
    }

    REG_INSTR(kINSTRTYPE_UNKNOWN, _instr_unknown);

    REG_INSTR(kINSTRTYPE_ADC, _instr_adc);
    REG_INSTR(kINSTRTYPE_AND, _instr_and);
    REG_INSTR(kINSTRTYPE_ASL, _instr_asl);
    REG_INSTR(kINSTRTYPE_BCC, _instr_bcc);
    REG_INSTR(kINSTRTYPE_BCS, _instr_bcs);
    REG_INSTR(kINSTRTYPE_BEQ, _instr_beq);
    REG_INSTR(kINSTRTYPE_BIT, _instr_bit);
    REG_INSTR(kINSTRTYPE_BMI, _instr_bmi);
    REG_INSTR(kINSTRTYPE_BPL, _instr_bpl);
    REG_INSTR(kINSTRTYPE_BNE, _instr_bne);
    REG_INSTR(kINSTRTYPE_BRK, _instr_brk);
    REG_INSTR(kINSTRTYPE_BVC, _instr_bvc);
    REG_INSTR(kINSTRTYPE_BVS, _instr_bvs);
    REG_INSTR(kINSTRTYPE_CLC, _instr_clc);
    REG_INSTR(kINSTRTYPE_CLD, _instr_cld);
    REG_INSTR(kINSTRTYPE_CLI, _instr_cli);
    REG_INSTR(kINSTRTYPE_CLV, _instr_clv);
    REG_INSTR(kINSTRTYPE_CMP, _instr_cmp);
    REG_INSTR(kINSTRTYPE_CPX, _instr_cpx);
    REG_INSTR(kINSTRTYPE_CPY, _instr_cpy);
    REG_INSTR(kINSTRTYPE_DEC, _instr_dec);
    REG_INSTR(kINSTRTYPE_DEX, _instr_dex);
    REG_INSTR(kINSTRTYPE_DEY, _instr_dey);
    REG_INSTR(kINSTRTYPE_EOR, _instr_eor);
    REG_INSTR(kINSTRTYPE_NOP, _instr_nop);
}

InstrInfo instr_decode(void) {
    const uint8_t opcode = g_device.cart->buffer[g_device.pc];
    InstrInfo instr = {
        .type       = kINSTRTYPE_UNKNOWN,
        .addr_mode  = kADDRMODE_UNKNOWN,
        .opcode     = opcode,
        .stride     = 1,
    };

    switch (opcode) {
        // ADC instructions
        case 0x69:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x65:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x75:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x6D:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x7D:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x79:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x61:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x71:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }

        // AND instructions
        case 0x29:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x25:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x35:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x2D:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x3D:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x39:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x21:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x31:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }

        // ASL instructions
        case 0x0A:
        {
            instr.type      = kINSTRTYPE_ASL;
            instr.addr_mode = kADDRMODE_ACCUMULATOR;
            break;
        }
        case 0x06:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_ASL;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x16:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_ASL;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x0E:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_ASL;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x1E:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_ASL;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }

        // BCC instructions
        case 0x90:
        {
            int8_t offset = 0;
            _instr_fetch_bytes(&offset, sizeof(offset));
            instr.type          = kINSTRTYPE_BCC;
            instr.addr_mode     = kADDRMODE_RELATIVE;
            instr.data.offset   = offset;
            instr.stride        = 2;
            break;
        }

        // BCS instructions
        case 0xB0:
        {
            int8_t offset = 0;
            _instr_fetch_bytes(&offset, sizeof(offset));
            instr.type          = kINSTRTYPE_BCS;
            instr.addr_mode     = kADDRMODE_RELATIVE;
            instr.data.offset   = offset;
            instr.stride        = 2;
            break;
        }

        // BEQ instructions
        case 0xF0:
        {
            int8_t offset = 0;
            _instr_fetch_bytes(&offset, sizeof(offset));
            instr.type          = kINSTRTYPE_BEQ;
            instr.addr_mode     = kADDRMODE_RELATIVE;
            instr.data.offset   = offset;
            instr.stride        = 2;
            break;
        }

        // BIT instructions
        case 0x24:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_BIT;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x2C:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_BIT;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.data.addr = addr;
            instr.stride    = 2;
            break;
        }

        // BMI instructions
        case 0x30:
        {
            int8_t offset = 0;
            _instr_fetch_bytes(&offset, sizeof(offset));
            instr.type          = kINSTRTYPE_BMI;
            instr.addr_mode     = kADDRMODE_RELATIVE;
            instr.data.offset   = offset;
            instr.stride        = 2;
            break;
        }

        // BNE instructions
        case 0xD0:
        {
            int8_t offset = 0;
            _instr_fetch_bytes(&offset, sizeof(offset));
            instr.type          = kINSTRTYPE_BNE;
            instr.addr_mode     = kADDRMODE_RELATIVE;
            instr.data.offset   = offset;
            instr.stride        = 2;
            break;
        }

        // BPL instructions
        case 0x10:
        {
            int8_t offset = 0;
            _instr_fetch_bytes(&offset, sizeof(offset));
            instr.type          = kINSTRTYPE_BPL;
            instr.addr_mode     = kADDRMODE_RELATIVE;
            instr.data.offset   = offset;
            instr.stride        = 2;
            break;
        }

        // BRK insructions
        case 0x00:
        {
            instr.type      = kINSTRTYPE_BRK;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            break;
        }

        // BVC instructions
        case 0x50:
        {
            int8_t offset = 0;
            _instr_fetch_bytes(&offset, sizeof(offset));
            instr.type          = kINSTRTYPE_BVC;
            instr.addr_mode     = kADDRMODE_RELATIVE;
            instr.data.offset   = offset;
            instr.stride        = 2;
            break;
        }

        // BVS instructions
        case 0x70:
        {
            int8_t offset = 0;
            _instr_fetch_bytes(&offset, sizeof(offset));
            instr.type          = kINSTRTYPE_BVS;
            instr.addr_mode     = kADDRMODE_RELATIVE;
            instr.data.offset   = offset;
            instr.stride        = 2;
            break;
        }

        // CLC instructions
        case 0x18:
        {
            instr.type      = kINSTRTYPE_CLC;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // CLD instructions
        case 0xD8:
        {
            instr.type      = kINSTRTYPE_CLD;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // CLI instructions
        case 0x58:
        {
            instr.type      = kINSTRTYPE_CLI;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // CLV instructions
        case 0xB8:
        {
            instr.type      = kINSTRTYPE_CLV;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // CMP instructions
        case 0xC9:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xC5:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xD5:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xCD:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0xDD:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0xD9:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0xC1:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xD1:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }

        // CPX instructions
        case 0xE0:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_CPX;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xE4:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_CPX;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xEC:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_CPX;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }

        // CPY instructions
        case 0xC0:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_CPY;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xC4:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_CPY;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xCC:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_CPY;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }

        // DEC instructions
        case 0xC6:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_DEC;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xD6:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_DEC;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0xCE:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_DEC;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0xDE:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_DEC;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }

        // DEX instructions
        case 0xCA:
        {
            instr.type      = kINSTRTYPE_DEX;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // DEY instructions
        case 0x88:
        {
            instr.type      = kINSTRTYPE_DEY;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // EOR instructions
        case 0x49:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x45:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x55:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x4D:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x5D:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x59:
        {
            uint16_t addr = 0;
            _instr_fetch_bytes(&addr, sizeof(addr));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.data.addr = addr;
            instr.stride    = 3;
            break;
        }
        case 0x41:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x51:
        {
            uint8_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }

        // NOP instructions
        case 0xEA:
        {
            instr.type      = kINSTRTYPE_NOP;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            break;
        }

        // TODO: the rest of the instructions

        default: break;
    }

    return instr;
}

void instr_exec(const InstrInfo* instr) {
    s_instr_exec_funcs[instr->type](instr);
}

static inline void _instr_fetch_bytes(void* buf, size_t size) {
    // +1 offset since pc should point at current instruction opcode
    uint8_t* bytes_start = g_device.cart->buffer + g_device.pc + 1;
    memcpy(buf, bytes_start , size);
}

static void _instr_unknown(const InstrInfo* instr) {
    TraceLog(LOG_WARNING, "unhandled instruction '0x%02X'", instr->opcode);
}

static void _instr_adc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
            TraceLog(LOG_INFO, "ADC #$%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE:
            TraceLog(LOG_INFO, "ADC $%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE_X:
            TraceLog(LOG_INFO, "ADC $%02X, X", instr->data.byte);
            break;

        case kADDRMODE_ABSOLUTE:
            TraceLog(LOG_INFO, "ADC $%04X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_X:
            TraceLog(LOG_INFO, "ADC $%04X, X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_Y:
            TraceLog(LOG_INFO, "ADC $%04X, Y", instr->data.addr);
            break;

        case kADDRMODE_IDX_INDIRECT:
            TraceLog(LOG_INFO, "ADC ($%02X, X)", instr->data.byte);
            break;

        case kADDRMODE_INDIRECT_IDX:
            TraceLog(LOG_INFO, "ADC ($%02X), Y", instr->data.byte);
            break;

        default:
            TraceLog(LOG_ERROR, "invalid addressing mode for instr ADC (%d)", instr->addr_mode);
            break;
    }
}

static void _instr_and(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
            TraceLog(LOG_INFO, "AND #$%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE:
            TraceLog(LOG_INFO, "AND $%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE_X:
            TraceLog(LOG_INFO, "AND $%02X, X", instr->data.byte);
            break;

        case kADDRMODE_ABSOLUTE:
            TraceLog(LOG_INFO, "AND $%04X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_X:
            TraceLog(LOG_INFO, "AND $%04X, X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_Y:
            TraceLog(LOG_INFO, "AND $%04X, Y", instr->data.addr);
            break;

        case kADDRMODE_IDX_INDIRECT:
            TraceLog(LOG_INFO, "AND ($%02X, X)", instr->data.byte);
            break;

        case kADDRMODE_INDIRECT_IDX:
            TraceLog(LOG_INFO, "AND ($%02X), Y", instr->data.byte);
            break;

        default:
            TraceLog(LOG_ERROR, "invalid addressing mode for instr AND (%d)", instr->addr_mode);
            break;
    }
}

static void _instr_asl(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
            TraceLog(LOG_INFO, "ASL A");
            break;

        case kADDRMODE_ZEROPAGE:
            TraceLog(LOG_INFO, "ASL $%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE_X:
            TraceLog(LOG_INFO, "ASL $%02X, X", instr->data.byte);
            break;

        case kADDRMODE_ABSOLUTE:
            TraceLog(LOG_INFO, "ASL $%04X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_X:
            TraceLog(LOG_INFO, "ASL $%04X, X", instr->data.addr);
            break;

        default:
            TraceLog(LOG_ERROR, "invalid addressing mode for instr ASL (%d)", instr->addr_mode);
            break;
    }
}

static void _instr_bcc(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_RELATIVE) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr BCC (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "BCC *%s%d", instr->data.offset < 0 ? "" : "+", instr->data.offset);
}

static void _instr_bcs(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_RELATIVE) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr BCS (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "BCS *%s%d", instr->data.offset < 0 ? "" : "+", instr->data.offset);
}

static void _instr_beq(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_RELATIVE) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr BEQ (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "BEQ *%s%d", instr->data.offset < 0 ? "" : "+", instr->data.offset);
}

static void _instr_bit(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
            TraceLog(LOG_INFO, "BIT $%02X", instr->data.byte);
            break;

        case kADDRMODE_ABSOLUTE:
            TraceLog(LOG_INFO, "BIT $%04X", instr->data.addr);
            break;

        default:
            TraceLog(LOG_ERROR, "invalid addressing mode for instr BIT (%d)", instr->addr_mode);
            break;
    }
}

static void _instr_bmi(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_RELATIVE) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr BMI (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "BMI *%s%d", instr->data.offset < 0 ? "" : "+", instr->data.offset);
}

static void _instr_bne(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_RELATIVE) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr BNE (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "BNE *%s%d", instr->data.offset < 0 ? "" : "+", instr->data.offset);
}

static void _instr_bpl(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_RELATIVE) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr BPL (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "BPL *%s%d", instr->data.offset < 0 ? "" : "+", instr->data.offset);
}

static void _instr_brk(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_IMPLICIT) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr BRK (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "BRK");
}

static void _instr_bvc(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_RELATIVE) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr BVC (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "BVC *%s%d", instr->data.offset < 0 ? "" : "+", instr->data.offset);
}

static void _instr_bvs(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_RELATIVE) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr BVS (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "BVS *%s%d", instr->data.offset < 0 ? "" : "+", instr->data.offset);
}

static void _instr_clc(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_IMPLICIT) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr CLC (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "CLC");
}

static void _instr_cld(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_IMPLICIT) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr CLD (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "CLD");
}

static void _instr_cli(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_IMPLICIT) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr CLI (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "CLI");
}

static void _instr_clv(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_IMPLICIT) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr CLV (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "CLV");
}

static void _instr_cmp(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
            TraceLog(LOG_INFO, "CMP #$%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE:
            TraceLog(LOG_INFO, "CMP $%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE_X:
            TraceLog(LOG_INFO, "CMP $%02X, X", instr->data.byte);
            break;

        case kADDRMODE_ABSOLUTE:
            TraceLog(LOG_INFO, "CMP $%04X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_X:
            TraceLog(LOG_INFO, "CMP $%04X, X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_Y:
            TraceLog(LOG_INFO, "CMP $%04X, Y", instr->data.addr);
            break;

        case kADDRMODE_IDX_INDIRECT:
            TraceLog(LOG_INFO, "CMP ($%02X, X)", instr->data.byte);
            break;

        case kADDRMODE_INDIRECT_IDX:
            TraceLog(LOG_INFO, "CMP ($%02X), Y", instr->data.byte);
            break;

        default:
            TraceLog(LOG_ERROR, "invalid addressing mode for instr CMP (%d)", instr->addr_mode);
            break;
    }
}

static void _instr_cpx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
            TraceLog(LOG_INFO, "CPX #$%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE:
            TraceLog(LOG_INFO, "CPX $%02X", instr->data.byte);
            break;

        case kADDRMODE_ABSOLUTE:
            TraceLog(LOG_INFO, "CPX $%04X", instr->data.addr);
            break;

        default:
            TraceLog(LOG_ERROR, "invalid addressing mode for instr CPX (%d)", instr->addr_mode);
            break;
    }
}

static void _instr_cpy(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
            TraceLog(LOG_INFO, "CPY #$%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE:
            TraceLog(LOG_INFO, "CPY $%02X", instr->data.byte);
            break;

        case kADDRMODE_ABSOLUTE:
            TraceLog(LOG_INFO, "CPY $%04X", instr->data.addr);
            break;

        default:
            TraceLog(LOG_ERROR, "invalid addressing mode for instr CPY (%d)", instr->addr_mode);
            break;
    }
}

static void _instr_dec(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
            TraceLog(LOG_INFO, "DEC $%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE_X:
            TraceLog(LOG_INFO, "DEC $%02X, X", instr->data.byte);
            break;

        case kADDRMODE_ABSOLUTE:
            TraceLog(LOG_INFO, "DEC $%04X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_X:
            TraceLog(LOG_INFO, "CMP $%04X, X", instr->data.addr);
            break;

        default:
            TraceLog(LOG_ERROR, "invalid addressing mode for instr CPY (%d)", instr->addr_mode);
            break;
    }
}

static void _instr_dex(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_IMPLICIT) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr DEX (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "DEX");
}

static void _instr_dey(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_IMPLICIT) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr DEY (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "DEY");
}

static void _instr_eor(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
            TraceLog(LOG_INFO, "EOR #$%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE:
            TraceLog(LOG_INFO, "EOR $%02X", instr->data.byte);
            break;

        case kADDRMODE_ZEROPAGE_X:
            TraceLog(LOG_INFO, "EOR $%02X, X", instr->data.byte);
            break;

        case kADDRMODE_ABSOLUTE:
            TraceLog(LOG_INFO, "EOR $%04X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_X:
            TraceLog(LOG_INFO, "EOR $%04X, X", instr->data.addr);
            break;

        case kADDRMODE_ABSOLUTE_Y:
            TraceLog(LOG_INFO, "EOR $%04X, Y", instr->data.addr);
            break;

        case kADDRMODE_IDX_INDIRECT:
            TraceLog(LOG_INFO, "EOR ($%02X, X)", instr->data.byte);
            break;

        case kADDRMODE_INDIRECT_IDX:
            TraceLog(LOG_INFO, "EOR ($%02X), Y", instr->data.byte);
            break;

        default:
            TraceLog(LOG_ERROR, "invalid addressing mode for instr CMP (%d)", instr->addr_mode);
            break;
    }
}

static void _instr_nop(const InstrInfo* instr) {
    if (instr->addr_mode != kADDRMODE_IMPLICIT) {
        TraceLog(LOG_ERROR, "invalid addressing mode for instr NOP (%d)", instr->addr_mode);
        return;
    }

    TraceLog(LOG_INFO, "NOP");
}

