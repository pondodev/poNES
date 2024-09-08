#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#define DISASM_INSTRUCTIONS

#include "cpu.h"

const char* disasm_get_asm(const InstrInfo* instr);

#endif

