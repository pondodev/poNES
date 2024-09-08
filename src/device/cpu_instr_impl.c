#include "cpu_instr_impl.h"

#include "memory_bus.h"
#include "disassembler.h"

#include "log.h"

static inline uint16_t _get_data_addr_zeropage(const InstrInfo* instr);
static inline uint16_t _get_data_addr_zeropage_x(const InstrInfo* instr);
static inline uint16_t _get_data_addr_zeropage_y(const InstrInfo* instr);
static inline uint16_t _get_data_addr_relative(const InstrInfo* instr);
static inline uint16_t _get_data_addr_absolute(const InstrInfo* instr);
static inline uint16_t _get_data_addr_absolute_x(const InstrInfo* instr);
static inline uint16_t _get_data_addr_absolute_y(const InstrInfo* instr);
static inline uint16_t _get_data_addr_indirect(const InstrInfo* instr);
static inline uint16_t _get_data_addr_idx_indirect(const InstrInfo* instr);
static inline uint16_t _get_data_addr_indirect_idx(const InstrInfo* instr);

void cpu_instr_unknown(const InstrInfo* instr) {
    log_warn("unhandled instruction '0x%02X'", instr->opcode);
}

void cpu_instr_adc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
        case kADDRMODE_ABSOLUTE_Y:
        case kADDRMODE_IDX_INDIRECT:
        case kADDRMODE_INDIRECT_IDX:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr ADC (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_and(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
        case kADDRMODE_ABSOLUTE_Y:
        case kADDRMODE_IDX_INDIRECT:
        case kADDRMODE_INDIRECT_IDX:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr AND (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_asl(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr ASL (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_bcc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BCC (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_bcs(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BCS (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_beq(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BEQ (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_bit(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ABSOLUTE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BIT (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_bmi(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BMI (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_bne(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BNE (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_bpl(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BPL (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_brk(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BRK (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_bvc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BVC (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_bvs(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BVS (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_clc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CLC (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_cld(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CLD (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_cli(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CLI (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_clv(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CLV (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_cmp(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
        case kADDRMODE_ABSOLUTE_Y:
        case kADDRMODE_IDX_INDIRECT:
        case kADDRMODE_INDIRECT_IDX:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CMP (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_cpx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ABSOLUTE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CPX (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_cpy(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ABSOLUTE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CPY (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_dec(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CPY (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_dex(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr DEX (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_dey(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr DEY (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_eor(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
        case kADDRMODE_ABSOLUTE_Y:
        case kADDRMODE_IDX_INDIRECT:
        case kADDRMODE_INDIRECT_IDX:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CMP (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_inc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr INC (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_inx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr INX (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_iny(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr INY (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_jmp(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_INDIRECT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr JMP (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_jsr(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ABSOLUTE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr JSR (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_lda(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
        case kADDRMODE_ABSOLUTE_Y:
        case kADDRMODE_IDX_INDIRECT:
        case kADDRMODE_INDIRECT_IDX:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr LDA (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_ldx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_Y:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_Y:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr LDX (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_ldy(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr LDY (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_lsr(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr LSR (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_nop(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr NOP (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_ora(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
        case kADDRMODE_ABSOLUTE_Y:
        case kADDRMODE_IDX_INDIRECT:
        case kADDRMODE_INDIRECT_IDX:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr ORA (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_pha(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr PHA (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_php(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr PHP (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_pla(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr PLA (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_plp(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr PLP (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_rol(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr ROL (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_ror(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr ROR (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_rti(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr RTI (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_rts(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr RTS (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_sbc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
        case kADDRMODE_ABSOLUTE_Y:
        case kADDRMODE_IDX_INDIRECT:
        case kADDRMODE_INDIRECT_IDX:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr SBC (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_sec(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr SEC (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_sed(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr SED (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_sei(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr SEI (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_sta(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
        case kADDRMODE_ABSOLUTE_Y:
        case kADDRMODE_IDX_INDIRECT:
        case kADDRMODE_INDIRECT_IDX:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr STA (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_stx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_Y:
        case kADDRMODE_ABSOLUTE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr STX (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_sty(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr STY (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_tax(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TAX (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_tay(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TAY (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_tsx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TSX (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_txa(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TXA (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_txs(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TXS (%d)", instr->addr_mode);
            break;
    }
}

void cpu_instr_tya(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TYA (%d)", instr->addr_mode);
            break;
    }
}

static inline uint16_t _get_data_addr_zeropage(const InstrInfo* instr) {
    return instr->data.byte;
}

static inline uint16_t _get_data_addr_zeropage_x(const InstrInfo* instr) {
    return instr->data.byte + *cpu_get_x();
}

static inline uint16_t _get_data_addr_zeropage_y(const InstrInfo* instr) {
    return instr->data.byte + *cpu_get_y();
}

static inline uint16_t _get_data_addr_relative(const InstrInfo* instr) {
    return *cpu_get_pc() + instr->data.offset;
}

static inline uint16_t _get_data_addr_absolute(const InstrInfo* instr) {
    return instr->data.addr;
}

static inline uint16_t _get_data_addr_absolute_x(const InstrInfo* instr) {
    return instr->data.addr + *cpu_get_x();
}

static inline uint16_t _get_data_addr_absolute_y(const InstrInfo* instr) {
    return instr->data.addr + *cpu_get_y();
}

static inline uint16_t _get_data_addr_indirect(const InstrInfo* instr) {
    uint16_t addr = 0;
    memory_bus_read(instr->data.addr, &addr, sizeof(addr));
    return addr;
}

static inline uint16_t _get_data_addr_idx_indirect(const InstrInfo* instr) {
    uint16_t addr = 0;
    memory_bus_read((instr->data.byte + *cpu_get_x()) & 0x00FF, &addr, sizeof(addr));
    return addr;
}

static inline uint16_t _get_data_addr_indirect_idx(const InstrInfo* instr) {
    uint16_t addr = 0;
    memory_bus_read(instr->data.byte, &addr, sizeof(addr));
    return addr + *cpu_get_y();
}

