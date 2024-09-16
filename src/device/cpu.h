#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>

typedef enum {
    kINSTRTYPE_UNKNOWN = 0,
    kINSTRTYPE_ADC, kINSTRTYPE_AND, kINSTRTYPE_ASL, kINSTRTYPE_BCC,
    kINSTRTYPE_BCS, kINSTRTYPE_BEQ, kINSTRTYPE_BIT, kINSTRTYPE_BMI,
    kINSTRTYPE_BNE, kINSTRTYPE_BPL, kINSTRTYPE_BRK, kINSTRTYPE_BVC,
    kINSTRTYPE_BVS, kINSTRTYPE_CLC, kINSTRTYPE_CLD, kINSTRTYPE_CLI,
    kINSTRTYPE_CLV, kINSTRTYPE_CMP, kINSTRTYPE_CPX, kINSTRTYPE_CPY,
    kINSTRTYPE_DEC, kINSTRTYPE_DEX, kINSTRTYPE_DEY, kINSTRTYPE_EOR,
    kINSTRTYPE_INC, kINSTRTYPE_INX, kINSTRTYPE_INY, kINSTRTYPE_JMP,
    kINSTRTYPE_JSR, kINSTRTYPE_LDA, kINSTRTYPE_LDX, kINSTRTYPE_LDY,
    kINSTRTYPE_LSR, kINSTRTYPE_NOP, kINSTRTYPE_ORA, kINSTRTYPE_PHA,
    kINSTRTYPE_PHP, kINSTRTYPE_PLA, kINSTRTYPE_PLP, kINSTRTYPE_ROL,
    kINSTRTYPE_ROR, kINSTRTYPE_RTI, kINSTRTYPE_RTS, kINSTRTYPE_SBC,
    kINSTRTYPE_SEC, kINSTRTYPE_SED, kINSTRTYPE_SEI, kINSTRTYPE_STA,
    kINSTRTYPE_STX, kINSTRTYPE_STY, kINSTRTYPE_TAX, kINSTRTYPE_TAY,
    kINSTRTYPE_TSX, kINSTRTYPE_TXA, kINSTRTYPE_TXS, kINSTRTYPE_TYA,

    kINSTRTYPE_COUNT,
} InstrType;

typedef enum {
    kADDRMODE_UNKNOWN = 0,
    kADDRMODE_IMPLICIT,
    kADDRMODE_ACCUMULATOR,
    kADDRMODE_IMMEDIATE,
    kADDRMODE_ZEROPAGE,
    kADDRMODE_ZEROPAGE_X,
    kADDRMODE_ZEROPAGE_Y,
    kADDRMODE_RELATIVE,
    kADDRMODE_ABSOLUTE,
    kADDRMODE_ABSOLUTE_X,
    kADDRMODE_ABSOLUTE_Y,
    kADDRMODE_INDIRECT,
    kADDRMODE_IDX_INDIRECT,
    kADDRMODE_INDIRECT_IDX,
} AddrMode;

typedef struct {
    InstrType       type;
    AddrMode        addr_mode;
    uint8_t         opcode;
    union {
        uint8_t     byte;
        uint16_t    addr;
        int8_t      offset;
    }               data;
    uint16_t        stride;
} InstrInfo;

typedef enum {
    kCPUSTATUSFLAG_CARRY        = 0,
    kCPUSTATUSFLAG_ZERO         = 1,
    kCPUSTATUSFLAG_IRQ_DISABLE  = 2,
    kCPUSTATUSFLAG_DEC_MODE     = 3,
    kCPUSTATUSFLAG_BREAK_CMD    = 4,
    kCPUSTATUSFLAG_OVERFLOW     = 5,
    kCPUSTATUSFLAG_NEGATIVE     = 6,
} CPUStatusFlag;

void cpu_init(void);

uint16_t* cpu_get_pc(void);
uint8_t* cpu_get_sp(void);
uint8_t* cpu_get_acc(void);
uint8_t* cpu_get_x(void);
uint8_t* cpu_get_y(void);
uint8_t* cpu_get_status(void);
uint8_t cpu_get_status_flag(CPUStatusFlag flag);
void cpu_set_status_flag(CPUStatusFlag flag, int value);
void cpu_stack_push(uint8_t data);
uint8_t cpu_stack_pop(void);

InstrInfo cpu_decode(void);
void cpu_exec(const InstrInfo* instr);

int cpu_apu_io_reg_read(uint16_t addr, void* out, size_t n);
int cpu_apu_io_reg_write(uint16_t addr, const void* in, size_t n);

#endif

