#include "disassembler.h"

#include <string.h>
#include <stdio.h>

static inline void _get_mnemonic(InstrType type);
static inline void _get_args(const InstrInfo* instr);

#define MNEMONIC_BUF_SIZE 4
#define ARGS_BUF_SIZE 20
#define MAIN_BUF_SIZE MNEMONIC_BUF_SIZE + ARGS_BUF_SIZE
static char s_mnemonic_buf[MNEMONIC_BUF_SIZE];
static char s_args_buf[ARGS_BUF_SIZE];
static char s_main_buf[MAIN_BUF_SIZE];

const char* disasm_get_asm(const InstrInfo* instr) {
    // clear out buffers first
    memset(s_mnemonic_buf, '\0', sizeof(s_mnemonic_buf[0])*MNEMONIC_BUF_SIZE);
    memset(s_args_buf, '\0', sizeof(s_args_buf[0])*ARGS_BUF_SIZE);
    memset(s_main_buf, '\0', sizeof(s_main_buf[0])*MAIN_BUF_SIZE);

    _get_mnemonic(instr->type);
    _get_args(instr);
    snprintf(s_main_buf, MAIN_BUF_SIZE-1, "%s %s", s_mnemonic_buf, s_args_buf);

    return s_main_buf;
}

#define LOAD_MNEMONIC(_str) \
    strncpy(s_mnemonic_buf, _str, MNEMONIC_BUF_SIZE-1)
static inline void _get_mnemonic(InstrType type) {
    switch (type) {
        case kINSTRTYPE_ADC:
            LOAD_MNEMONIC("ADC");
            break;
        case kINSTRTYPE_AND:
            LOAD_MNEMONIC("AND");
            break;
        case kINSTRTYPE_ASL:
            LOAD_MNEMONIC("ASL");
            break;
        case kINSTRTYPE_BCC:
            LOAD_MNEMONIC("BCC");
            break;
        case kINSTRTYPE_BCS:
            LOAD_MNEMONIC("BCS");
            break;
        case kINSTRTYPE_BEQ:
            LOAD_MNEMONIC("BEQ");
            break;
        case kINSTRTYPE_BIT:
            LOAD_MNEMONIC("BIT");
            break;
        case kINSTRTYPE_BMI:
            LOAD_MNEMONIC("BMI");
            break;
        case kINSTRTYPE_BNE:
            LOAD_MNEMONIC("BNE");
            break;
        case kINSTRTYPE_BPL:
            LOAD_MNEMONIC("BPL");
            break;
        case kINSTRTYPE_BRK:
            LOAD_MNEMONIC("BRK");
            break;
        case kINSTRTYPE_BVC:
            LOAD_MNEMONIC("BVC");
            break;
        case kINSTRTYPE_BVS:
            LOAD_MNEMONIC("BVS");
            break;
        case kINSTRTYPE_CLC:
            LOAD_MNEMONIC("CLC");
            break;
        case kINSTRTYPE_CLD:
            LOAD_MNEMONIC("CLD");
            break;
        case kINSTRTYPE_CLI:
            LOAD_MNEMONIC("CLI");
            break;
        case kINSTRTYPE_CLV:
            LOAD_MNEMONIC("CLV");
            break;
        case kINSTRTYPE_CMP:
            LOAD_MNEMONIC("CMP");
            break;
        case kINSTRTYPE_CPX:
            LOAD_MNEMONIC("CPX");
            break;
        case kINSTRTYPE_CPY:
            LOAD_MNEMONIC("CPY");
            break;
        case kINSTRTYPE_DEC:
            LOAD_MNEMONIC("DEC");
            break;
        case kINSTRTYPE_DEX:
            LOAD_MNEMONIC("DEX");
            break;
        case kINSTRTYPE_DEY:
            LOAD_MNEMONIC("DEY");
            break;
        case kINSTRTYPE_EOR:
            LOAD_MNEMONIC("EOR");
            break;
        case kINSTRTYPE_INC:
            LOAD_MNEMONIC("INC");
            break;
        case kINSTRTYPE_INX:
            LOAD_MNEMONIC("INX");
            break;
        case kINSTRTYPE_INY:
            LOAD_MNEMONIC("INY");
            break;
        case kINSTRTYPE_JMP:
            LOAD_MNEMONIC("JMP");
            break;
        case kINSTRTYPE_JSR:
            LOAD_MNEMONIC("JSR");
            break;
        case kINSTRTYPE_LDA:
            LOAD_MNEMONIC("LDA");
            break;
        case kINSTRTYPE_LDX:
            LOAD_MNEMONIC("LDX");
            break;
        case kINSTRTYPE_LDY:
            LOAD_MNEMONIC("LDY");
            break;
        case kINSTRTYPE_LSR:
            LOAD_MNEMONIC("LSR");
            break;
        case kINSTRTYPE_NOP:
            LOAD_MNEMONIC("NOP");
            break;
        case kINSTRTYPE_ORA:
            LOAD_MNEMONIC("ORA");
            break;
        case kINSTRTYPE_PHA:
            LOAD_MNEMONIC("PHA");
            break;
        case kINSTRTYPE_PHP:
            LOAD_MNEMONIC("PHP");
            break;
        case kINSTRTYPE_PLA:
            LOAD_MNEMONIC("PLA");
            break;
        case kINSTRTYPE_PLP:
            LOAD_MNEMONIC("PLP");
            break;
        case kINSTRTYPE_ROL:
            LOAD_MNEMONIC("ROL");
            break;
        case kINSTRTYPE_ROR:
            LOAD_MNEMONIC("ROR");
            break;
        case kINSTRTYPE_RTI:
            LOAD_MNEMONIC("RTI");
            break;
        case kINSTRTYPE_RTS:
            LOAD_MNEMONIC("RTS");
            break;
        case kINSTRTYPE_SBC:
            LOAD_MNEMONIC("SBC");
            break;
        case kINSTRTYPE_SEC:
            LOAD_MNEMONIC("SEC");
            break;
        case kINSTRTYPE_SED:
            LOAD_MNEMONIC("SED");
            break;
        case kINSTRTYPE_SEI:
            LOAD_MNEMONIC("SEI");
            break;
        case kINSTRTYPE_STA:
            LOAD_MNEMONIC("STA");
            break;
        case kINSTRTYPE_STX:
            LOAD_MNEMONIC("STX");
            break;
        case kINSTRTYPE_STY:
            LOAD_MNEMONIC("STY");
            break;
        case kINSTRTYPE_TAX:
            LOAD_MNEMONIC("TAX");
            break;
        case kINSTRTYPE_TAY:
            LOAD_MNEMONIC("TAY");
            break;
        case kINSTRTYPE_TSX:
            LOAD_MNEMONIC("TSX");
            break;
        case kINSTRTYPE_TXA:
            LOAD_MNEMONIC("TXA");
            break;
        case kINSTRTYPE_TXS:
            LOAD_MNEMONIC("TXS");
            break;
        case kINSTRTYPE_TYA:
            LOAD_MNEMONIC("TYA");
            break;

        case kINSTRTYPE_UNKNOWN:
        default:
            LOAD_MNEMONIC("???");
            break;
    }
}

#define LOAD_ARGS(_fmt, ...) \
    snprintf(s_args_buf, ARGS_BUF_SIZE-1, _fmt, ##__VA_ARGS__)
static inline void _get_args(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;
        case kADDRMODE_ACCUMULATOR:
            LOAD_ARGS("A");
            break;
        case kADDRMODE_IMMEDIATE:
            LOAD_ARGS("#$%02X", instr->data.byte);
            break;
        case kADDRMODE_ZEROPAGE:
            LOAD_ARGS("$%02X", instr->data.byte);
            break;
        case kADDRMODE_ZEROPAGE_X:
            LOAD_ARGS("$%02X, X", instr->data.byte);
            break;
        case kADDRMODE_ZEROPAGE_Y:
            LOAD_ARGS("$%02X, Y", instr->data.byte);
            break;
        case kADDRMODE_RELATIVE:
            LOAD_ARGS("*%s%d", instr->data.offset < 0 ? "" : "+", instr->data.offset);
            break;
        case kADDRMODE_ABSOLUTE:
            LOAD_ARGS("$%04X", instr->data.addr);
            break;
        case kADDRMODE_ABSOLUTE_X:
            LOAD_ARGS("$%04X, X", instr->data.addr);
            break;
        case kADDRMODE_ABSOLUTE_Y:
            LOAD_ARGS("$%04X, Y", instr->data.addr);
            break;
        case kADDRMODE_INDIRECT:
            LOAD_ARGS("($%04X)", instr->data.addr);
            break;
        case kADDRMODE_IDX_INDIRECT:
            LOAD_ARGS("($%02X, X)", instr->data.byte);
            break;
        case kADDRMODE_INDIRECT_IDX:
            LOAD_ARGS("($%02X), Y", instr->data.byte);
            break;

        case kADDRMODE_UNKNOWN:
        default:
            LOAD_ARGS("???");
            break;
    }
}

