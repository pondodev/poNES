#include "instructions.h"

#include "device.h"
#include "disassembler.h"
#include "memory.h"

#include "log.h"

typedef void (*InstrExecFunc)(const InstrInfo* instr);

static inline void _fetch_bytes(void* buf, size_t size);

static inline uint8_t _fetch_data_immediate(const InstrInfo* instr);
static inline uint8_t _fetch_data_zeropage(const InstrInfo* instr);
static inline uint8_t _fetch_data_zeropage_x(const InstrInfo* instr);
static inline uint8_t _fetch_data_zeropage_y(const InstrInfo* instr);
static inline uint16_t _fetch_addr_relative(const InstrInfo* instr);
static inline uint8_t _fetch_data_absolute(const InstrInfo* instr);
static inline uint8_t _fetch_data_absolute_x(const InstrInfo* instr);
static inline uint8_t _fetch_data_absolute_y(const InstrInfo* instr);
static inline uint16_t _fetch_addr_indirect(const InstrInfo* instr);
static inline uint8_t _fetch_data_idx_indirect(const InstrInfo* instr);
static inline uint8_t _fetch_data_indirect_idx(const InstrInfo* instr);

static inline void _instr_unknown(const InstrInfo* instr);
static inline void _instr_adc(const InstrInfo* instr);
static inline void _instr_and(const InstrInfo* instr);
static inline void _instr_asl(const InstrInfo* instr);
static inline void _instr_bcc(const InstrInfo* instr);
static inline void _instr_bcs(const InstrInfo* instr);
static inline void _instr_beq(const InstrInfo* instr);
static inline void _instr_bit(const InstrInfo* instr);
static inline void _instr_bmi(const InstrInfo* instr);
static inline void _instr_bne(const InstrInfo* instr);
static inline void _instr_bpl(const InstrInfo* instr);
static inline void _instr_brk(const InstrInfo* instr);
static inline void _instr_bvc(const InstrInfo* instr);
static inline void _instr_bvs(const InstrInfo* instr);
static inline void _instr_clc(const InstrInfo* instr);
static inline void _instr_cld(const InstrInfo* instr);
static inline void _instr_cli(const InstrInfo* instr);
static inline void _instr_clv(const InstrInfo* instr);
static inline void _instr_cmp(const InstrInfo* instr);
static inline void _instr_cpx(const InstrInfo* instr);
static inline void _instr_cpy(const InstrInfo* instr);
static inline void _instr_dec(const InstrInfo* instr);
static inline void _instr_dex(const InstrInfo* instr);
static inline void _instr_dey(const InstrInfo* instr);
static inline void _instr_eor(const InstrInfo* instr);
static inline void _instr_inc(const InstrInfo* instr);
static inline void _instr_inx(const InstrInfo* instr);
static inline void _instr_iny(const InstrInfo* instr);
static inline void _instr_jmp(const InstrInfo* instr);
static inline void _instr_jsr(const InstrInfo* instr);
static inline void _instr_lda(const InstrInfo* instr);
static inline void _instr_ldx(const InstrInfo* instr);
static inline void _instr_ldy(const InstrInfo* instr);
static inline void _instr_lsr(const InstrInfo* instr);
static inline void _instr_nop(const InstrInfo* instr);
static inline void _instr_ora(const InstrInfo* instr);
static inline void _instr_pha(const InstrInfo* instr);
static inline void _instr_php(const InstrInfo* instr);
static inline void _instr_pla(const InstrInfo* instr);
static inline void _instr_plp(const InstrInfo* instr);
static inline void _instr_rol(const InstrInfo* instr);
static inline void _instr_ror(const InstrInfo* instr);
static inline void _instr_rti(const InstrInfo* instr);
static inline void _instr_rts(const InstrInfo* instr);
static inline void _instr_sbc(const InstrInfo* instr);
static inline void _instr_sec(const InstrInfo* instr);
static inline void _instr_sed(const InstrInfo* instr);
static inline void _instr_sei(const InstrInfo* instr);
static inline void _instr_sta(const InstrInfo* instr);
static inline void _instr_stx(const InstrInfo* instr);
static inline void _instr_sty(const InstrInfo* instr);
static inline void _instr_tax(const InstrInfo* instr);
static inline void _instr_tay(const InstrInfo* instr);
static inline void _instr_tsx(const InstrInfo* instr);
static inline void _instr_txa(const InstrInfo* instr);
static inline void _instr_txs(const InstrInfo* instr);
static inline void _instr_tya(const InstrInfo* instr);

static InstrExecFunc s_instr_exec_funcs[kINSTRTYPE_COUNT];
#define REG_INSTR(_alias, _func) s_instr_exec_funcs[_alias] = _func

void instr_init(void) {
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
    REG_INSTR(kINSTRTYPE_INC, _instr_inc);
    REG_INSTR(kINSTRTYPE_INX, _instr_inx);
    REG_INSTR(kINSTRTYPE_INY, _instr_iny);
    REG_INSTR(kINSTRTYPE_JMP, _instr_jmp);
    REG_INSTR(kINSTRTYPE_JSR, _instr_jsr);
    REG_INSTR(kINSTRTYPE_LDA, _instr_lda);
    REG_INSTR(kINSTRTYPE_LDX, _instr_ldx);
    REG_INSTR(kINSTRTYPE_LDY, _instr_ldy);
    REG_INSTR(kINSTRTYPE_LSR, _instr_lsr);
    REG_INSTR(kINSTRTYPE_NOP, _instr_nop);
    REG_INSTR(kINSTRTYPE_ORA, _instr_ora);
    REG_INSTR(kINSTRTYPE_PHA, _instr_pha);
    REG_INSTR(kINSTRTYPE_PHP, _instr_php);
    REG_INSTR(kINSTRTYPE_PLA, _instr_pla);
    REG_INSTR(kINSTRTYPE_PLP, _instr_plp);
    REG_INSTR(kINSTRTYPE_ROL, _instr_rol);
    REG_INSTR(kINSTRTYPE_ROR, _instr_ror);
    REG_INSTR(kINSTRTYPE_RTI, _instr_rti);
    REG_INSTR(kINSTRTYPE_RTS, _instr_rts);
    REG_INSTR(kINSTRTYPE_SBC, _instr_sbc);
    REG_INSTR(kINSTRTYPE_SEC, _instr_sec);
    REG_INSTR(kINSTRTYPE_SED, _instr_sed);
    REG_INSTR(kINSTRTYPE_SEI, _instr_sei);
    REG_INSTR(kINSTRTYPE_STA, _instr_sta);
    REG_INSTR(kINSTRTYPE_STX, _instr_stx);
    REG_INSTR(kINSTRTYPE_STY, _instr_sty);
    REG_INSTR(kINSTRTYPE_TAX, _instr_tax);
    REG_INSTR(kINSTRTYPE_TAY, _instr_tay);
    REG_INSTR(kINSTRTYPE_TSX, _instr_tsx);
    REG_INSTR(kINSTRTYPE_TXA, _instr_txa);
    REG_INSTR(kINSTRTYPE_TXS, _instr_txs);
    REG_INSTR(kINSTRTYPE_TYA, _instr_tya);
}

InstrInfo instr_decode(void) {
    const uint8_t opcode = memory_read8(g_device.pc);
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

void instr_exec(const InstrInfo* instr) {
    s_instr_exec_funcs[instr->type](instr);
}

static inline void _fetch_bytes(void* buf, size_t size) {
    // +1 offset since pc should point at current instruction opcode
    memory_read(g_device.pc+1, buf, size);
}

static inline uint8_t _fetch_data_immediate(const InstrInfo* instr) {
    return instr->data.byte;
}

static inline uint8_t _fetch_data_zeropage(const InstrInfo* instr) {
    return memory_read8(instr->data.byte);
}

static inline uint8_t _fetch_data_zeropage_x(const InstrInfo* instr) {
    return memory_read8(instr->data.byte + g_device.x);
}

static inline uint8_t _fetch_data_zeropage_y(const InstrInfo* instr) {
    return memory_read8(instr->data.byte + g_device.y);
}

static inline uint16_t _fetch_addr_relative(const InstrInfo* instr) {
    return g_device.pc + instr->data.offset;
}

static inline uint8_t _fetch_data_absolute(const InstrInfo* instr) {
    return memory_read8(instr->data.addr);
}

static inline uint8_t _fetch_data_absolute_x(const InstrInfo* instr) {
    return memory_read8(instr->data.addr + g_device.x);
}

static inline uint8_t _fetch_data_absolute_y(const InstrInfo* instr) {
    return memory_read8(instr->data.addr + g_device.y);
}

static inline uint16_t _fetch_addr_indirect(const InstrInfo* instr) {
    uint16_t addr;
    memory_read(instr->data.addr, &addr, sizeof(addr));
    return addr;
}

static inline uint8_t _fetch_data_idx_indirect(const InstrInfo* instr) {
    const uint16_t addr = instr->data.byte + g_device.x;
    return memory_read8(addr & 0x00FF);
}

static inline uint8_t _fetch_data_indirect_idx(const InstrInfo* instr) {
    const uint16_t addr = memory_read8(instr->data.byte) + g_device.y;
    return memory_read8(addr);
}

static inline void _instr_unknown(const InstrInfo* instr) {
    log_warn("unhandled instruction '0x%02X'", instr->opcode);
}

static inline void _instr_adc(const InstrInfo* instr) {
    uint8_t data;
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

static inline void _instr_and(const InstrInfo* instr) {
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

static inline void _instr_asl(const InstrInfo* instr) {
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

static inline void _instr_bcc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BCC (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_bcs(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BCS (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_beq(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BEQ (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_bit(const InstrInfo* instr) {
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

static inline void _instr_bmi(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BMI (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_bne(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BNE (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_bpl(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BPL (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_brk(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BRK (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_bvc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BVC (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_bvs(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_RELATIVE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr BVS (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_clc(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CLC (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_cld(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CLD (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_cli(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CLI (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_clv(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr CLV (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_cmp(const InstrInfo* instr) {
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

static inline void _instr_cpx(const InstrInfo* instr) {
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

static inline void _instr_cpy(const InstrInfo* instr) {
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

static inline void _instr_dec(const InstrInfo* instr) {
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

static inline void _instr_dex(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr DEX (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_dey(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr DEY (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_eor(const InstrInfo* instr) {
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

static inline void _instr_inc(const InstrInfo* instr) {
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

static inline void _instr_inx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr INX (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_iny(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr INY (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_jmp(const InstrInfo* instr) {
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

static inline void _instr_jsr(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_ABSOLUTE:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr JSR (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_lda(const InstrInfo* instr) {
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

static inline void _instr_ldx(const InstrInfo* instr) {
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

static inline void _instr_ldy(const InstrInfo* instr) {
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

static inline void _instr_lsr(const InstrInfo* instr) {
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

static inline void _instr_nop(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr NOP (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_ora(const InstrInfo* instr) {
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

static inline void _instr_pha(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr PHA (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_php(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr PHP (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_pla(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr PLA (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_plp(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr PLP (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_rol(const InstrInfo* instr) {
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

static inline void _instr_ror(const InstrInfo* instr) {
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

static inline void _instr_rti(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr RTI (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_rts(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr RTS (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_sbc(const InstrInfo* instr) {
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

static inline void _instr_sec(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr SEC (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_sed(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr SED (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_sei(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr SEI (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_sta(const InstrInfo* instr) {
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

static inline void _instr_stx(const InstrInfo* instr) {
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

static inline void _instr_sty(const InstrInfo* instr) {
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

static inline void _instr_tax(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TAX (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_tay(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TAY (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_tsx(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TSX (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_txa(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TXA (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_txs(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TXS (%d)", instr->addr_mode);
            break;
    }
}

static inline void _instr_tya(const InstrInfo* instr) {
    switch (instr->addr_mode) {
        case kADDRMODE_IMPLICIT:
            log_info("%s", disasm_get_asm(instr));
            break;

        default:
            log_error("invalid addressing mode for instr TYA (%d)", instr->addr_mode);
            break;
    }
}

