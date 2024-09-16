#include "cpu_instr_impl.h"

#include "memory_bus.h"

#include "log.h"

#define BIT(n) (1 << n)
#define IRQ_VECTOR 0xFFFE

static inline void _branch(const InstrInfo* instr);

static inline int _get_data_addr(const InstrInfo* instr, uint16_t* out);
static inline uint8_t _get_data(const InstrInfo* instr);
static inline void _set_data(const InstrInfo* instr, uint8_t data);

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
            break;

        default:
            log_error("invalid addressing mode for instr ADC (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    uint8_t *acc        = cpu_get_acc();
    const uint8_t carry = cpu_get_status_flag(kCPUSTATUSFLAG_CARRY);

    const uint8_t res = *acc + data + carry;
    // unsigned overflow
    const int c = res < *acc;
    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, c);

    // signed overflow
    // this check is kinda strange, but is fundamentally pretty simple. consider
    // the following truth table:
    //
    // | a | d | r | f(a, d, r) |
    // | - | - | - | ---------- |
    // | 0 | 0 | 0 | 0          |
    // | 1 | 0 | 0 | 0          |
    // | 0 | 1 | 0 | 0          |
    // | 0 | 0 | 1 | 1          |
    // | 1 | 1 | 0 | 1          |
    // | 1 | 0 | 1 | 0          |
    // | 0 | 1 | 1 | 0          |
    // | 1 | 1 | 1 | 0          |
    //
    // or in plain english, we want to check if acc and data are positive but the
    // result is negative, or if acc and data are negative but the result is
    // positive. these two states are the only states where an overflow is possible
    // during an add operation, so the below expression checks for that.

    const int v = (*acc & data & ~res) | (~*acc & ~data & res);
    cpu_set_status_flag(kCPUSTATUSFLAG_OVERFLOW, v & BIT(7));

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));

    *acc = res;
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
            break;

        default:
            log_error("invalid addressing mode for instr AND (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    uint8_t* acc        = cpu_get_acc();

    const uint8_t res = *acc & data;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));
}

void cpu_instr_asl(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            break;

        default:
            log_error("invalid addressing mode for instr ASL (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t res   = data << 1;

    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, data & BIT(7));
    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));

    _set_data(instr, res);
}

void cpu_instr_bcc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            break;

        default:
            log_error("invalid addressing mode for instr BCC (%d)", instr->addr_mode);
            break;
    }

    if (! cpu_get_status_flag(kCPUSTATUSFLAG_CARRY))
        _branch(instr);
}

void cpu_instr_bcs(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            break;

        default:
            log_error("invalid addressing mode for instr BCS (%d)", instr->addr_mode);
            break;
    }

    if (cpu_get_status_flag(kCPUSTATUSFLAG_CARRY))
        _branch(instr);
}

void cpu_instr_beq(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            break;

        default:
            log_error("invalid addressing mode for instr BEQ (%d)", instr->addr_mode);
            break;
    }

    if (cpu_get_status_flag(kCPUSTATUSFLAG_ZERO))
        _branch(instr);
}

void cpu_instr_bit(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ABSOLUTE:
            break;

        default:
            log_error("invalid addressing mode for instr BIT (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t* acc  = cpu_get_acc();
    const uint8_t res   = *acc & data;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_OVERFLOW, data & BIT(6));
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, data & BIT(7));
}

void cpu_instr_bmi(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            break;

        default:
            log_error("invalid addressing mode for instr BMI (%d)", instr->addr_mode);
            break;
    }

    if (cpu_get_status_flag(kCPUSTATUSFLAG_NEGATIVE))
        _branch(instr);
}

void cpu_instr_bne(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            break;

        default:
            log_error("invalid addressing mode for instr BNE (%d)", instr->addr_mode);
            break;
    }

    if (! cpu_get_status_flag(kCPUSTATUSFLAG_ZERO))
        _branch(instr);
}

void cpu_instr_bpl(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            break;

        default:
            log_error("invalid addressing mode for instr BPL (%d)", instr->addr_mode);
            break;
    }

    if (! cpu_get_status_flag(kCPUSTATUSFLAG_NEGATIVE))
        _branch(instr);
}

void cpu_instr_brk(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr BRK (%d)", instr->addr_mode);
            break;
    }

    if (cpu_get_status_flag(kCPUSTATUSFLAG_IRQ_DISABLE))
        return;

    cpu_set_status_flag(kCPUSTATUSFLAG_BREAK_CMD, 1);

    uint16_t* pc = cpu_get_pc();
    cpu_stack_push((*pc & 0xFF00) >> 8);
    cpu_stack_push((*pc & 0x00FF));
    cpu_stack_push(*cpu_get_status());

    memory_bus_read(IRQ_VECTOR, pc, sizeof(*pc));
}

void cpu_instr_bvc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            break;

        default:
            log_error("invalid addressing mode for instr BVC (%d)", instr->addr_mode);
            break;
    }

    if (! cpu_get_status_flag(kCPUSTATUSFLAG_OVERFLOW))
        _branch(instr);
}

void cpu_instr_bvs(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            break;

        default:
            log_error("invalid addressing mode for instr BVS (%d)", instr->addr_mode);
            break;
    }

    if (cpu_get_status_flag(kCPUSTATUSFLAG_OVERFLOW))
        _branch(instr);
}

void cpu_instr_clc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr CLC (%d)", instr->addr_mode);
            break;
    }

    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, 0);
}

void cpu_instr_cld(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr CLD (%d)", instr->addr_mode);
            break;
    }

    cpu_set_status_flag(kCPUSTATUSFLAG_DEC_MODE, 0);
}

void cpu_instr_cli(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr CLI (%d)", instr->addr_mode);
            break;
    }

    cpu_set_status_flag(kCPUSTATUSFLAG_IRQ_DISABLE, 0);
}

void cpu_instr_clv(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr CLV (%d)", instr->addr_mode);
            break;
    }

    cpu_set_status_flag(kCPUSTATUSFLAG_OVERFLOW, 0);
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
            break;

        default:
            log_error("invalid addressing mode for instr CMP (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t acc   = *cpu_get_acc();
    const uint8_t res   = acc - data;

    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, acc >= data);
    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));
}

void cpu_instr_cpx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ABSOLUTE:
            break;

        default:
            log_error("invalid addressing mode for instr CPX (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t x     = *cpu_get_x();
    const uint8_t res   = x - data;

    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, x >= data);
    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));
}

void cpu_instr_cpy(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ABSOLUTE:
            break;

        default:
            log_error("invalid addressing mode for instr CPY (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t y     = *cpu_get_y();
    const uint8_t res   = y - data;

    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, y >= data);
    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));
}

void cpu_instr_dec(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            break;

        default:
            log_error("invalid addressing mode for instr CPY (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t res   = data - 1;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));

    _set_data(instr, res);
}

void cpu_instr_dex(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr DEX (%d)", instr->addr_mode);
            break;
    }

    uint8_t* x = cpu_get_x();
    --*x;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, *x == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, *x & BIT(7));
}

void cpu_instr_dey(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr DEY (%d)", instr->addr_mode);
            break;
    }

    uint8_t* y = cpu_get_y();
    --*y;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, *y == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, *y & BIT(7));
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
            break;

        default:
            log_error("invalid addressing mode for instr CMP (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    uint8_t* acc        = cpu_get_acc();
    const uint8_t res   = *acc | data;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));

    *acc = res;
}

void cpu_instr_inc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            break;

        default:
            log_error("invalid addressing mode for instr INC (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t res   = data + 1;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));

    _set_data(instr, res);
}

void cpu_instr_inx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr INX (%d)", instr->addr_mode);
            break;
    }

    uint8_t* x = cpu_get_x();
    ++*x;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, *x == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, *x & BIT(7));
}

void cpu_instr_iny(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr INY (%d)", instr->addr_mode);
            break;
    }

    uint8_t* y = cpu_get_y();
    ++*y;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, *y == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, *y & BIT(7));
}

void cpu_instr_jmp(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_INDIRECT:
            break;

        default:
            log_error("invalid addressing mode for instr JMP (%d)", instr->addr_mode);
            break;
    }

    uint16_t addr;
    _get_data_addr(instr, &addr);
    *cpu_get_pc() = addr;
}

void cpu_instr_jsr(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ABSOLUTE:
            break;

        default:
            log_error("invalid addressing mode for instr JSR (%d)", instr->addr_mode);
            break;
    }

    uint16_t addr;
    _get_data_addr(instr, &addr);

    uint16_t* pc                = cpu_get_pc();
    const uint16_t return_addr  = *pc + 2;

    cpu_stack_push((return_addr & 0xFF00) >> 8);
    cpu_stack_push(return_addr & 0x00FF);
    *pc = addr;
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
            break;

        default:
            log_error("invalid addressing mode for instr LDA (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data = _get_data(instr);

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, data == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, data & BIT(7));

    *cpu_get_acc() = data;
}

void cpu_instr_ldx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_Y:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_Y:
            break;

        default:
            log_error("invalid addressing mode for instr LDX (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data = _get_data(instr);

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, data == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, data & BIT(7));

    *cpu_get_x() = data;
}

void cpu_instr_ldy(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMMEDIATE:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            break;

        default:
            log_error("invalid addressing mode for instr LDY (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data = _get_data(instr);

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, data == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, data & BIT(7));

    *cpu_get_y() = data;
}

void cpu_instr_lsr(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            break;

        default:
            log_error("invalid addressing mode for instr LSR (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t res   = data >> 1;

    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, data & BIT(0));
    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);

    // NOTE: the negative flag _should_ be set if bit 7 is 1, however
    // bit 7 should always be zero after this op anyway so we just always
    // clear the flag.
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, 0);
}

void cpu_instr_nop(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr NOP (%d)", instr->addr_mode);
            break;
    }

    // NOP
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
            break;

        default:
            log_error("invalid addressing mode for instr ORA (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    uint8_t* acc        = cpu_get_acc();
    const uint8_t res   = *acc | data;

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));

    *acc = res;
}

void cpu_instr_pha(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr PHA (%d)", instr->addr_mode);
            break;
    }

    cpu_stack_push(*cpu_get_acc());
}

void cpu_instr_php(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr PHP (%d)", instr->addr_mode);
            break;
    }

    cpu_stack_push(*cpu_get_status());
}

void cpu_instr_pla(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr PLA (%d)", instr->addr_mode);
            break;
    }

    *cpu_get_acc() = cpu_stack_pop();
}

void cpu_instr_plp(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr PLP (%d)", instr->addr_mode);
            break;
    }

    *cpu_get_status() = cpu_stack_pop();
}

void cpu_instr_rol(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            break;

        default:
            log_error("invalid addressing mode for instr ROL (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t res   = (data << 1) | cpu_get_status_flag(kCPUSTATUSFLAG_CARRY);

    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, data & BIT(7));
    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));

    _set_data(instr, res);
}

void cpu_instr_ror(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
        case kADDRMODE_ABSOLUTE_X:
            break;

        default:
            log_error("invalid addressing mode for instr ROR (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    const uint8_t res   = (data >> 1) | (cpu_get_status_flag(kCPUSTATUSFLAG_CARRY) << 7);

    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, data & BIT(0));
    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));

    _set_data(instr, res);
}

void cpu_instr_rti(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr RTI (%d)", instr->addr_mode);
            break;
    }

    const uint8_t status    = cpu_stack_pop();
    const uint16_t addr_lsb = cpu_stack_pop();
    const uint16_t addr_msb = cpu_stack_pop();

    *cpu_get_status()   = status;
    *cpu_get_pc()       = (addr_msb << 8) | addr_lsb;
}

void cpu_instr_rts(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr RTS (%d)", instr->addr_mode);
            break;
    }

    const uint16_t addr_lsb = cpu_stack_pop();
    const uint16_t addr_msb = cpu_stack_pop();

    *cpu_get_pc() = (addr_msb << 8) | addr_lsb;
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
            break;

        default:
            log_error("invalid addressing mode for instr SBC (%d)", instr->addr_mode);
            break;
    }

    const uint8_t data  = _get_data(instr);
    uint8_t *acc        = cpu_get_acc();
    const uint8_t carry = cpu_get_status_flag(kCPUSTATUSFLAG_CARRY);

    const uint8_t res = *acc - data - (1-carry);
    // unsigned underflow
    const int c = res > *acc;
    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, c);

    // similar situation to the overflow check for ADC, the check here seems funky
    // but is fundamentally simple. consider the following truth table:
    //
    // | a | d | r | f(a, d, r) |
    // | - | - | - | ---------- |
    // | 0 | 0 | 0 | 0          |
    // | 1 | 0 | 0 | 1          |
    // | 0 | 1 | 0 | 0          |
    // | 0 | 0 | 1 | 0          |
    // | 1 | 1 | 0 | 0          |
    // | 1 | 0 | 1 | 0          |
    // | 0 | 1 | 1 | 1          |
    // | 1 | 1 | 1 | 0          |
    //
    // again, in plain english what we're looking for is whether acc is negative,
    // data is positive, and the result is positive, or if acc is positive,
    // data is negative, and the result is negative. these are the only situations
    // where over/underflow can occur during subtraction.

    const int v = (*acc & ~data & ~res) | (~*acc & data & res);
    cpu_set_status_flag(kCPUSTATUSFLAG_OVERFLOW, v & BIT(7));

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, res == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, res & BIT(7));

    *acc = res;
}

void cpu_instr_sec(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr SEC (%d)", instr->addr_mode);
            break;
    }

    cpu_set_status_flag(kCPUSTATUSFLAG_CARRY, 1);
}

void cpu_instr_sed(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr SED (%d)", instr->addr_mode);
            break;
    }

    cpu_set_status_flag(kCPUSTATUSFLAG_DEC_MODE, 1);
}

void cpu_instr_sei(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr SEI (%d)", instr->addr_mode);
            break;
    }

    cpu_set_status_flag(kCPUSTATUSFLAG_IRQ_DISABLE, 1);
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
            break;

        default:
            log_error("invalid addressing mode for instr STA (%d)", instr->addr_mode);
            break;
    }

    _set_data(instr, *cpu_get_acc());
}

void cpu_instr_stx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_Y:
        case kADDRMODE_ABSOLUTE:
            break;

        default:
            log_error("invalid addressing mode for instr STX (%d)", instr->addr_mode);
            break;
    }

    _set_data(instr, *cpu_get_x());
}

void cpu_instr_sty(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
        case kADDRMODE_ZEROPAGE_X:
        case kADDRMODE_ABSOLUTE:
            break;

        default:
            log_error("invalid addressing mode for instr STY (%d)", instr->addr_mode);
            break;
    }

    _set_data(instr, *cpu_get_y());
}

void cpu_instr_tax(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr TAX (%d)", instr->addr_mode);
            break;
    }

    uint8_t* x  = cpu_get_x();
    *x          = *cpu_get_acc();

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, *x == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, *x & BIT(7));
}

void cpu_instr_tay(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr TAY (%d)", instr->addr_mode);
            break;
    }

    uint8_t* y  = cpu_get_y();
    *y          = *cpu_get_acc();

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, *y == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, *y & BIT(7));
}

void cpu_instr_tsx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr TSX (%d)", instr->addr_mode);
            break;
    }

    uint8_t* x  = cpu_get_x();
    *x          = *cpu_get_sp();

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, *x == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, *x & BIT(7));
}

void cpu_instr_txa(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr TXA (%d)", instr->addr_mode);
            break;
    }

    uint8_t* acc    = cpu_get_acc();
    *acc            = *cpu_get_x();

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, *acc == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, *acc & BIT(7));
}

void cpu_instr_txs(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr TXS (%d)", instr->addr_mode);
            break;
    }

    *cpu_get_sp() = *cpu_get_x();
}

void cpu_instr_tya(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            break;

        default:
            log_error("invalid addressing mode for instr TYA (%d)", instr->addr_mode);
            break;
    }

    uint8_t* acc    = cpu_get_acc();
    *acc            = *cpu_get_y();

    cpu_set_status_flag(kCPUSTATUSFLAG_ZERO, *acc == 0);
    cpu_set_status_flag(kCPUSTATUSFLAG_NEGATIVE, *acc & BIT(7));
}

static inline void _branch(const InstrInfo* instr) {
    *cpu_get_pc() += instr->data.offset;
}

static inline int _get_data_addr(const InstrInfo* instr, uint16_t* out) {
    switch (instr->addr_mode) {
        case kADDRMODE_ZEROPAGE:
            *out = instr->data.byte;
            return 1;
        case kADDRMODE_ZEROPAGE_X:
            *out = instr->data.byte + *cpu_get_x();
            return 1;
        case kADDRMODE_ZEROPAGE_Y:
            *out = instr->data.byte + *cpu_get_y();
            return 1;
        case kADDRMODE_ABSOLUTE:
            *out = instr->data.addr;
            return 1;
        case kADDRMODE_ABSOLUTE_X:
            *out = instr->data.addr + *cpu_get_x();
            return 1;
        case kADDRMODE_ABSOLUTE_Y:
            *out = instr->data.addr + *cpu_get_y();
            return 1;
        case kADDRMODE_INDIRECT:
        {
            memory_bus_read(instr->data.addr, out, sizeof(*out));
            return 1;
        }
        case kADDRMODE_IDX_INDIRECT:
        {
            uint16_t addr = 0;
            memory_bus_read((instr->data.byte + *cpu_get_x()) & 0x00FF, &addr, sizeof(addr));
            *out = addr;
            return 1;
        }
        case kADDRMODE_INDIRECT_IDX:
        {
            uint16_t addr = 0;
            memory_bus_read(instr->data.byte, &addr, sizeof(addr));
            *out = addr + *cpu_get_y();
            return 1;
        }

        default:
            log_error("cannot get address in addressing mode '%d'", instr->addr_mode);
            return 0;
    }
}

static inline uint8_t _get_data(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
            return *cpu_get_acc();
        case kADDRMODE_IMMEDIATE:
            return instr->data.byte;

        default:
        {
            uint16_t addr;
            if (! _get_data_addr(instr, &addr))
                return 0;

            uint8_t data;
            if (memory_bus_read(addr, &data, sizeof(data))) {
                return data;
            } else {
                log_error("failed to read memory");
                return 0;
            }
        }
    }
}

static inline void _set_data(const InstrInfo* instr, uint8_t data) {
    switch (instr->addr_mode) {
        case kADDRMODE_ACCUMULATOR:
            *cpu_get_acc() = data;
            break;

        default:
        {
            uint16_t addr;
            if (! _get_data_addr(instr, &addr))
                return;

            uint8_t data;
            if (! memory_bus_write(addr, &data, sizeof(data)))
                log_error("failed to write memory");
        }
    }
}

