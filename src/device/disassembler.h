#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#define DISASM_INSTRUCTIONS

#include "instructions.h"

const char* disasm_get_asm(const InstrInfo* instr);

#endif

