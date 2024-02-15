#include "instructions.h"

#include "device.h"

#include "raylib.h"

#include <string.h>

static inline void _instr_fetch_bytes(void* buf, size_t size);
static inline void _instr_adc(const InstrInfo* instr);

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
            uint16_t byte = 0;
            _instr_fetch_bytes(&byte, sizeof(byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.data.addr = byte;
            instr.stride    = 2;
            break;
        }

        // BRK insructions
        case 0x00:
        {
            instr.type      = kINSTRTYPE_BRK;
            instr.addr_mode = kADDRMODE_IMPLICIT;
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
    switch (instr->type) {
        case kINSTRTYPE_ADC:
            _instr_adc(instr);
            break;

        case kINSTRTYPE_BRK:
            TraceLog(LOG_INFO, "BRK");
            break;

        case kINSTRTYPE_NOP:
            TraceLog(LOG_INFO, "NOP");
            break;

        case kINSTRTYPE_UNKNOWN:
        default:
            TraceLog(LOG_WARNING, "unhandled instruction '0x%02X'", instr->opcode);
            break;
    }
}

static inline void _instr_fetch_bytes(void* buf, size_t size) {
    // +1 offset since pc should point at current instruction opcode
    uint8_t* bytes_start = g_device.cart->buffer + g_device.pc + 1;
    memcpy(buf, bytes_start , size);
}

static inline void _instr_adc(const InstrInfo* instr) {
    switch (instr->addr_mode)
    {
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

