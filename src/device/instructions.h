#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>


void instr_init(void);
InstrInfo instr_decode(void);
void instr_exec(const InstrInfo* instr);

#endif

