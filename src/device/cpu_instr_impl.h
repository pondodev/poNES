#ifndef CPU_INSTR_IMPL_H
#define CPU_INSTR_IMPL_H

#include "cpu.h"

void cpu_instr_unknown(const InstrInfo* instr);
void cpu_instr_adc(const InstrInfo* instr);
void cpu_instr_and(const InstrInfo* instr);
void cpu_instr_asl(const InstrInfo* instr);
void cpu_instr_bcc(const InstrInfo* instr);
void cpu_instr_bcs(const InstrInfo* instr);
void cpu_instr_beq(const InstrInfo* instr);
void cpu_instr_bit(const InstrInfo* instr);
void cpu_instr_bmi(const InstrInfo* instr);
void cpu_instr_bne(const InstrInfo* instr);
void cpu_instr_bpl(const InstrInfo* instr);
void cpu_instr_brk(const InstrInfo* instr);
void cpu_instr_bvc(const InstrInfo* instr);
void cpu_instr_bvs(const InstrInfo* instr);
void cpu_instr_clc(const InstrInfo* instr);
void cpu_instr_cld(const InstrInfo* instr);
void cpu_instr_cli(const InstrInfo* instr);
void cpu_instr_clv(const InstrInfo* instr);
void cpu_instr_cmp(const InstrInfo* instr);
void cpu_instr_cpx(const InstrInfo* instr);
void cpu_instr_cpy(const InstrInfo* instr);
void cpu_instr_dec(const InstrInfo* instr);
void cpu_instr_dex(const InstrInfo* instr);
void cpu_instr_dey(const InstrInfo* instr);
void cpu_instr_eor(const InstrInfo* instr);
void cpu_instr_inc(const InstrInfo* instr);
void cpu_instr_inx(const InstrInfo* instr);
void cpu_instr_iny(const InstrInfo* instr);
void cpu_instr_jmp(const InstrInfo* instr);
void cpu_instr_jsr(const InstrInfo* instr);
void cpu_instr_lda(const InstrInfo* instr);
void cpu_instr_ldx(const InstrInfo* instr);
void cpu_instr_ldy(const InstrInfo* instr);
void cpu_instr_lsr(const InstrInfo* instr);
void cpu_instr_nop(const InstrInfo* instr);
void cpu_instr_ora(const InstrInfo* instr);
void cpu_instr_pha(const InstrInfo* instr);
void cpu_instr_php(const InstrInfo* instr);
void cpu_instr_pla(const InstrInfo* instr);
void cpu_instr_plp(const InstrInfo* instr);
void cpu_instr_rol(const InstrInfo* instr);
void cpu_instr_ror(const InstrInfo* instr);
void cpu_instr_rti(const InstrInfo* instr);
void cpu_instr_rts(const InstrInfo* instr);
void cpu_instr_sbc(const InstrInfo* instr);
void cpu_instr_sec(const InstrInfo* instr);
void cpu_instr_sed(const InstrInfo* instr);
void cpu_instr_sei(const InstrInfo* instr);
void cpu_instr_sta(const InstrInfo* instr);
void cpu_instr_stx(const InstrInfo* instr);
void cpu_instr_sty(const InstrInfo* instr);
void cpu_instr_tax(const InstrInfo* instr);
void cpu_instr_tay(const InstrInfo* instr);
void cpu_instr_tsx(const InstrInfo* instr);
void cpu_instr_txa(const InstrInfo* instr);
void cpu_instr_txs(const InstrInfo* instr);
void cpu_instr_tya(const InstrInfo* instr);

#endif

