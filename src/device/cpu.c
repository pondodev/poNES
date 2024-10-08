#include "cpu.h"

#include "memory_bus.h"
#include "cpu_instr_impl.h"
#include "helpers.h"

typedef void (*InstrExecFunc)(const InstrInfo* instr);

static InstrExecFunc s_instr_exec_funcs[kINSTRTYPE_COUNT];
#define REG_INSTR(_alias, _func) s_instr_exec_funcs[_alias] = _func

#define STACK_ADDR_MSB 0x0100

static struct {
    uint16_t    pc;
    uint8_t     sp;
    uint8_t     acc;
    uint8_t     x;
    uint8_t     y;
    uint8_t     status;
} s_regs = {
    .pc     = 0xFFFC,
    .sp     = 0xFD,
    .x      = 0x00,
    .y      = 0x00,
    .status = 0x00 & BIT(kCPUSTATUSFLAG_IRQ_DISABLE),
};

static inline void _fetch_bytes(void* buf, size_t size);

void cpu_init(void) {
    REG_INSTR(kINSTRTYPE_UNKNOWN, cpu_instr_unknown);

    REG_INSTR(kINSTRTYPE_ADC, cpu_instr_adc);
    REG_INSTR(kINSTRTYPE_AND, cpu_instr_and);
    REG_INSTR(kINSTRTYPE_ASL, cpu_instr_asl);
    REG_INSTR(kINSTRTYPE_BCC, cpu_instr_bcc);
    REG_INSTR(kINSTRTYPE_BCS, cpu_instr_bcs);
    REG_INSTR(kINSTRTYPE_BEQ, cpu_instr_beq);
    REG_INSTR(kINSTRTYPE_BIT, cpu_instr_bit);
    REG_INSTR(kINSTRTYPE_BMI, cpu_instr_bmi);
    REG_INSTR(kINSTRTYPE_BPL, cpu_instr_bpl);
    REG_INSTR(kINSTRTYPE_BNE, cpu_instr_bne);
    REG_INSTR(kINSTRTYPE_BRK, cpu_instr_brk);
    REG_INSTR(kINSTRTYPE_BVC, cpu_instr_bvc);
    REG_INSTR(kINSTRTYPE_BVS, cpu_instr_bvs);
    REG_INSTR(kINSTRTYPE_CLC, cpu_instr_clc);
    REG_INSTR(kINSTRTYPE_CLD, cpu_instr_cld);
    REG_INSTR(kINSTRTYPE_CLI, cpu_instr_cli);
    REG_INSTR(kINSTRTYPE_CLV, cpu_instr_clv);
    REG_INSTR(kINSTRTYPE_CMP, cpu_instr_cmp);
    REG_INSTR(kINSTRTYPE_CPX, cpu_instr_cpx);
    REG_INSTR(kINSTRTYPE_CPY, cpu_instr_cpy);
    REG_INSTR(kINSTRTYPE_DEC, cpu_instr_dec);
    REG_INSTR(kINSTRTYPE_DEX, cpu_instr_dex);
    REG_INSTR(kINSTRTYPE_DEY, cpu_instr_dey);
    REG_INSTR(kINSTRTYPE_EOR, cpu_instr_eor);
    REG_INSTR(kINSTRTYPE_INC, cpu_instr_inc);
    REG_INSTR(kINSTRTYPE_INX, cpu_instr_inx);
    REG_INSTR(kINSTRTYPE_INY, cpu_instr_iny);
    REG_INSTR(kINSTRTYPE_JMP, cpu_instr_jmp);
    REG_INSTR(kINSTRTYPE_JSR, cpu_instr_jsr);
    REG_INSTR(kINSTRTYPE_LDA, cpu_instr_lda);
    REG_INSTR(kINSTRTYPE_LDX, cpu_instr_ldx);
    REG_INSTR(kINSTRTYPE_LDY, cpu_instr_ldy);
    REG_INSTR(kINSTRTYPE_LSR, cpu_instr_lsr);
    REG_INSTR(kINSTRTYPE_NOP, cpu_instr_nop);
    REG_INSTR(kINSTRTYPE_ORA, cpu_instr_ora);
    REG_INSTR(kINSTRTYPE_PHA, cpu_instr_pha);
    REG_INSTR(kINSTRTYPE_PHP, cpu_instr_php);
    REG_INSTR(kINSTRTYPE_PLA, cpu_instr_pla);
    REG_INSTR(kINSTRTYPE_PLP, cpu_instr_plp);
    REG_INSTR(kINSTRTYPE_ROL, cpu_instr_rol);
    REG_INSTR(kINSTRTYPE_ROR, cpu_instr_ror);
    REG_INSTR(kINSTRTYPE_RTI, cpu_instr_rti);
    REG_INSTR(kINSTRTYPE_RTS, cpu_instr_rts);
    REG_INSTR(kINSTRTYPE_SBC, cpu_instr_sbc);
    REG_INSTR(kINSTRTYPE_SEC, cpu_instr_sec);
    REG_INSTR(kINSTRTYPE_SED, cpu_instr_sed);
    REG_INSTR(kINSTRTYPE_SEI, cpu_instr_sei);
    REG_INSTR(kINSTRTYPE_STA, cpu_instr_sta);
    REG_INSTR(kINSTRTYPE_STX, cpu_instr_stx);
    REG_INSTR(kINSTRTYPE_STY, cpu_instr_sty);
    REG_INSTR(kINSTRTYPE_TAX, cpu_instr_tax);
    REG_INSTR(kINSTRTYPE_TAY, cpu_instr_tay);
    REG_INSTR(kINSTRTYPE_TSX, cpu_instr_tsx);
    REG_INSTR(kINSTRTYPE_TXA, cpu_instr_txa);
    REG_INSTR(kINSTRTYPE_TXS, cpu_instr_txs);
    REG_INSTR(kINSTRTYPE_TYA, cpu_instr_tya);
}

uint16_t* cpu_get_pc(void) {
    return &s_regs.pc;
}

uint8_t* cpu_get_sp(void) {
    return &s_regs.sp;
}

uint8_t* cpu_get_acc(void) {
    return &s_regs.acc;
}

uint8_t* cpu_get_x(void) {
    return &s_regs.x;
}

uint8_t* cpu_get_y(void) {
    return &s_regs.y;
}

uint8_t* cpu_get_status(void) {
    return &s_regs.status;
}

uint8_t cpu_get_status_flag(CPUStatusFlag flag) {
    return read_bit(s_regs.status, flag);
}

void cpu_set_status_flag(CPUStatusFlag flag, int value) {
    write_bit(&s_regs.status, flag, value);
}

void cpu_stack_push(uint8_t data) {
    const uint16_t addr = STACK_ADDR_MSB | s_regs.sp;

    memory_bus_write(addr, &data, sizeof(data));
    --s_regs.sp;
}

uint8_t cpu_stack_pop(void) {
    const uint16_t addr = STACK_ADDR_MSB | s_regs.sp;

    uint8_t data;
    memory_bus_read(addr, &data, sizeof(data));
    ++s_regs.sp;

    return data;
}

InstrInfo cpu_decode(void) {
    InstrInfo instr = {
        .type       = kINSTRTYPE_UNKNOWN,
        .addr_mode  = kADDRMODE_UNKNOWN,
        .stride     = 1,
    };
    memory_bus_read(s_regs.pc, &instr.opcode, sizeof(instr.opcode));

    switch (instr.opcode) {
        // ADC instructions
        case 0x69:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0x65:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x75:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0x6D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x7D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }
        case 0x79:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.stride    = 3;
            break;
        }
        case 0x61:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.stride    = 2;
            break;
        }
        case 0x71:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ADC;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.stride    = 2;
            break;
        }

        // AND instructions
        case 0x29:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0x25:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x35:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0x2D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x3D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }
        case 0x39:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.stride    = 3;
            break;
        }
        case 0x21:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.stride    = 2;
            break;
        }
        case 0x31:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_AND;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
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
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ASL;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x16:
        {
            uint8_t byte = 0;
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ASL;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.data.byte = byte;
            instr.stride    = 2;
            break;
        }
        case 0x0E:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ASL;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x1E:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ASL;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }

        // BCC instructions
        case 0x90:
        {
            _fetch_bytes(&instr.data.offset, sizeof(instr.data.offset));
            instr.type      = kINSTRTYPE_BCC;
            instr.addr_mode = kADDRMODE_RELATIVE;
            instr.stride    = 2;
            break;
        }

        // BCS instructions
        case 0xB0:
        {
            _fetch_bytes(&instr.data.offset, sizeof(instr.data.offset));
            instr.type      = kINSTRTYPE_BCS;
            instr.addr_mode = kADDRMODE_RELATIVE;
            instr.stride    = 2;
            break;
        }

        // BEQ instructions
        case 0xF0:
        {
            _fetch_bytes(&instr.data.offset, sizeof(instr.data.offset));
            instr.type      = kINSTRTYPE_BEQ;
            instr.addr_mode = kADDRMODE_RELATIVE;
            instr.stride    = 2;
            break;
        }

        // BIT instructions
        case 0x24:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_BIT;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x2C:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_BIT;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }

        // BMI instructions
        case 0x30:
        {
            _fetch_bytes(&instr.data.offset, sizeof(instr.data.offset));
            instr.type      = kINSTRTYPE_BMI;
            instr.addr_mode = kADDRMODE_RELATIVE;
            instr.stride    = 2;
            break;
        }

        // BNE instructions
        case 0xD0:
        {
            _fetch_bytes(&instr.data.offset, sizeof(instr.data.offset));
            instr.type      = kINSTRTYPE_BNE;
            instr.addr_mode = kADDRMODE_RELATIVE;
            instr.stride    = 2;
            break;
        }

        // BPL instructions
        case 0x10:
        {
            _fetch_bytes(&instr.data.offset, sizeof(instr.data.offset));
            instr.type      = kINSTRTYPE_BPL;
            instr.addr_mode = kADDRMODE_RELATIVE;
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

        // BVC instructions
        case 0x50:
        {
            _fetch_bytes(&instr.data.offset, sizeof(instr.data.offset));
            instr.type      = kINSTRTYPE_BVC;
            instr.addr_mode = kADDRMODE_RELATIVE;
            instr.stride    = 2;
            break;
        }

        // BVS instructions
        case 0x70:
        {
            _fetch_bytes(&instr.data.offset, sizeof(instr.data.offset));
            instr.type      = kINSTRTYPE_BVS;
            instr.addr_mode = kADDRMODE_RELATIVE;
            instr.stride    = 2;
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
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0xC5:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0xD5:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0xCD:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0xDD:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }
        case 0xD9:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.stride    = 3;
            break;
        }
        case 0xC1:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.stride    = 2;
            break;
        }
        case 0xD1:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_CMP;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.stride    = 2;
            break;
        }

        // CPX instructions
        case 0xE0:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_CPX;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0xE4:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_CPX;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0xEC:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_CPX;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }

        // CPY instructions
        case 0xC0:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_CPY;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0xC4:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_CPY;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0xCC:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_CPY;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }

        // DEC instructions
        case 0xC6:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_DEC;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0xD6:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_DEC;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0xCE:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_DEC;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0xDE:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_DEC;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
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
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0x45:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x55:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0x4D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x5D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }
        case 0x59:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.stride    = 3;
            break;
        }
        case 0x41:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.stride    = 2;
            break;
        }
        case 0x51:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_EOR;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.stride    = 2;
            break;
        }

        // INC instructions
        case 0xE6:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_INC;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0xF6:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_INC;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0xEE:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_INC;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0xFE:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_INC;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }

        // INX instructions
        case 0xE8:
        {
            instr.type      = kINSTRTYPE_INX;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // INY instructions
        case 0xC8:
        {
            instr.type      = kINSTRTYPE_INY;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // JMP instructions
        case 0x4C:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_JMP;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x6C:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_JMP;
            instr.addr_mode = kADDRMODE_INDIRECT;
            instr.stride    = 3;
            break;
        }

        // JSR instructions
        case 0x20:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_JSR;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }

        // LDA instructions
        case 0xA9:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDA;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0xA5:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDA;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0xB5:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDA;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0xAD:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_LDA;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0xBD:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_LDA;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }
        case 0xB9:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_LDA;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.stride    = 3;
            break;
        }
        case 0xA1:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDA;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.stride    = 2;
            break;
        }
        case 0xB1:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDA;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.stride    = 2;
            break;
        }

        // LDX instructions
        case 0xA2:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDX;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0xA6:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDX;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0xB6:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDX;
            instr.addr_mode = kADDRMODE_ZEROPAGE_Y;
            instr.stride    = 2;
            break;
        }
        case 0xAE:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_LDX;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0xBE:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_LDX;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.stride    = 3;
            break;
        }

        // LDY instructions
        case 0xA0:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDY;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0xA4:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDY;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0xB4:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LDY;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0xAC:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_LDY;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0xBC:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_LDY;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }

        // LSR instructions
        case 0x4A:
        {
            instr.type      = kINSTRTYPE_LSR;
            instr.addr_mode = kADDRMODE_ACCUMULATOR;
            instr.stride    = 1;
            break;
        }
        case 0x46:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LSR;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x56:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_LSR;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0x4E:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_LSR;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x5E:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_LSR;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }

        // NOP instructions
        case 0xEA:
        {
            instr.type      = kINSTRTYPE_NOP;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            break;
        }

        // ORA instructions
        case 0x09:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ORA;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0x05:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ORA;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x15:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ORA;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0x0D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ORA;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x1D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ORA;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }
        case 0x19:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ORA;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.stride    = 3;
            break;
        }
        case 0x01:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ORA;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.stride    = 2;
            break;
        }
        case 0x11:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ORA;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.stride    = 2;
            break;
        }

        // PHA instructions
        case 0x48:
        {
            instr.type      = kINSTRTYPE_PHA;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // PHP instructions
        case 0x08:
        {
            instr.type      = kINSTRTYPE_PHP;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // PLA instructions
        case 0x68:
        {
            instr.type      = kINSTRTYPE_PLA;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // PLP instructions
        case 0x28:
        {
            instr.type      = kINSTRTYPE_PLP;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // ROL instructions
        case 0x2A:
        {
            instr.type      = kINSTRTYPE_ROL;
            instr.addr_mode = kADDRMODE_ACCUMULATOR;
            instr.stride    = 1;
            break;
        }
        case 0x26:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ROL;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x36:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ROL;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0x2E:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ROL;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x3E:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ROL;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }

        // ROR instructions
        case 0x6A:
        {
            instr.type      = kINSTRTYPE_ROR;
            instr.addr_mode = kADDRMODE_ACCUMULATOR;
            instr.stride    = 1;
            break;
        }
        case 0x66:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ROR;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x76:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_ROR;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0x6E:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ROR;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x7E:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_ROR;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }

        // RTI instructions
        case 0x40:
        {
            instr.type      = kINSTRTYPE_RTI;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // RTS instructions
        case 0x60:
        {
            instr.type      = kINSTRTYPE_RTS;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // SBC instructions
        case 0xE9:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_SBC;
            instr.addr_mode = kADDRMODE_IMMEDIATE;
            instr.stride    = 2;
            break;
        }
        case 0xE5:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_SBC;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0xF5:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_SBC;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0xED:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_SBC;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0xFD:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_SBC;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }
        case 0xF9:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_SBC;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.stride    = 3;
            break;
        }
        case 0xE1:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_SBC;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.stride    = 2;
            break;
        }
        case 0xF1:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_SBC;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.stride    = 2;
            break;
        }

        // SEC instructions
        case 0x38:
        {
            instr.type      = kINSTRTYPE_SEC;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // SED instructions
        case 0xF8:
        {
            instr.type      = kINSTRTYPE_SED;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // SEI instructions
        case 0x78:
        {
            instr.type      = kINSTRTYPE_SEI;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // STA instructions
        case 0x85:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_STA;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x95:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_STA;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0x8D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_STA;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }
        case 0x9D:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_STA;
            instr.addr_mode = kADDRMODE_ABSOLUTE_X;
            instr.stride    = 3;
            break;
        }
        case 0x99:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_STA;
            instr.addr_mode = kADDRMODE_ABSOLUTE_Y;
            instr.stride    = 3;
            break;
        }
        case 0x81:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_STA;
            instr.addr_mode = kADDRMODE_IDX_INDIRECT;
            instr.stride    = 2;
            break;
        }
        case 0x91:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_STA;
            instr.addr_mode = kADDRMODE_INDIRECT_IDX;
            instr.stride    = 2;
            break;
        }

        // STX instructions
        case 0x86:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_STX;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x96:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_STX;
            instr.addr_mode = kADDRMODE_ZEROPAGE_Y;
            instr.stride    = 2;
            break;
        }
        case 0x8E:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_STX;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }

        // STY instructions
        case 0x84:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_STY;
            instr.addr_mode = kADDRMODE_ZEROPAGE;
            instr.stride    = 2;
            break;
        }
        case 0x94:
        {
            _fetch_bytes(&instr.data.byte, sizeof(instr.data.byte));
            instr.type      = kINSTRTYPE_STY;
            instr.addr_mode = kADDRMODE_ZEROPAGE_X;
            instr.stride    = 2;
            break;
        }
        case 0x8C:
        {
            _fetch_bytes(&instr.data.addr, sizeof(instr.data.addr));
            instr.type      = kINSTRTYPE_STY;
            instr.addr_mode = kADDRMODE_ABSOLUTE;
            instr.stride    = 3;
            break;
        }

        // TAX instructions
        case 0xAA:
        {
            instr.type      = kINSTRTYPE_TAX;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // TAY instructions
        case 0xA8:
        {
            instr.type      = kINSTRTYPE_TAY;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // TSX instructions
        case 0xBA:
        {
            instr.type      = kINSTRTYPE_TSX;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // TXA instructions
        case 0x8A:
        {
            instr.type      = kINSTRTYPE_TXA;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // TXS instructions
        case 0x9A:
        {
            instr.type      = kINSTRTYPE_TXS;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        // TYA instructions
        case 0x98:
        {
            instr.type      = kINSTRTYPE_TYA;
            instr.addr_mode = kADDRMODE_IMPLICIT;
            instr.stride    = 1;
            break;
        }

        default: break;
    }

    return instr;
}

void cpu_exec(const InstrInfo* instr) {
    s_instr_exec_funcs[instr->type](instr);
}

int cpu_apu_io_reg_read8(uint16_t addr, uint8_t* out) {
    (void)addr;
    (void)out;

    // TODO
    return 0;
}

int cpu_apu_io_reg_write8(uint16_t addr, const uint8_t* in) {
    (void)addr;
    (void)in;

    // TODO
    return 0;
}

static inline void _fetch_bytes(void* buf, size_t size) {
    // +1 offset since pc should point at current instruction opcode
    memory_bus_read(s_regs.pc+1, buf, size);
}
