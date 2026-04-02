/* rssb.h.  RSSB-V opcode list for GDB, the GNU debugger.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.
   Contributed by Andrew Waterman

   This file is part of GDB, GAS, and the GNU binutils.

   GDB, GAS, and the GNU binutils are free software; you can redistribute
   them and/or modify them under the terms of the GNU General Public
   License as published by the Free Software Foundation; either version
   3, or (at your option) any later version.

   GDB, GAS, and the GNU binutils are distributed in the hope that they
   will be useful, but WITHOUT ANY WARRANTY; without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3. If not,
   see <http://www.gnu.org/licenses/>.  */

#ifndef _RSSB_OPCODES_H_
#define _RSSB_OPCODES_H_

// #include "rssb-opc.h"
#include <stdlib.h>
#include <stdint.h>
#if 0
typedef uint64_t insn_t;

/* These fake label defines are use by both the assembler, and
   libopcodes.  The assembler uses this when it needs to generate a fake
   label, and libopcodes uses it to hide the fake labels in its output.  */
#define RSSB_FAKE_LABEL_NAME ".L0 "
#define RSSB_FAKE_LABEL_CHAR ' '

/* Replace bits MASK << SHIFT of STRUCT with the equivalent bits in
   VALUE << SHIFT.  VALUE is evaluated exactly once.  */
#define INSERT_BITS(STRUCT, VALUE, MASK, SHIFT) \
  (STRUCT) = (((STRUCT) & ~((insn_t)(MASK) << (SHIFT))) \
	      | ((insn_t)((VALUE) & (MASK)) << (SHIFT)))

/* Extract bits MASK << SHIFT from STRUCT and shift them right
   SHIFT places.  */
#define EXTRACT_BITS(STRUCT, MASK, SHIFT) \
  (((STRUCT) >> (SHIFT)) & (MASK))

/* Extract the operand given by FIELD from integer INSN.  */
#define EXTRACT_OPERAND(FIELD, INSN) \
  ((unsigned int) EXTRACT_BITS ((INSN), OP_MASK_##FIELD, OP_SH_##FIELD))

/* Extract an unsigned immediate operand on position s with n bits.  */
#define EXTRACT_U_IMM(n, s, l) \
  RV_X (l, s, n)

/* Extract an signed immediate operand on position s with n bits.  */
#define EXTRACT_S_IMM(n, s, l) \
  RV_X_SIGNED (l, s, n)

/* Validate that unsigned n-bit immediate is within bounds.  */
#define VALIDATE_U_IMM(v, n) \
  ((unsigned long) v < (1UL << n))

/* Validate that signed n-bit immediate is within bounds.  */
#define VALIDATE_S_IMM(v, n) \
  (v < (long) (1UL << (n-1)) && v >= -(offsetT) (1UL << (n-1)))
#endif
#define MAX_OPERANDS 1
#define RSSB_OPERAND_SIZE 4
#define RSSB_INSN_SIZE 4
#if 0
static inline unsigned int rssb_insn_length (insn_t insn ATTRIBUTE_UNUSED)
{
  return RSSB_OPERAND_SIZE;
}

#define GLOBAL_OFFSET_TABLE_NAME "_GLOBAL_OFFSET_TABLE_"

/* This structure holds information for a particular instruction.  */
struct rssb_opcode
{
  /* The name of the instruction.  */
  const char *name;

  /* The requirement of xlen for the instruction, 0 if no requirement.  */
  unsigned xlen_requirement;

  /* A string describing the arguments for this instruction.  */
  const char *args;

  /* The basic opcode for the instruction.  When assembling, this
     opcode is modified by the arguments to produce the actual opcode
     that is used.  If pinfo is INSN_MACRO, then this is 0.  */
  insn_t match;

  /* If pinfo is not INSN_MACRO, then this is a bit mask for the
     relevant portions of the opcode when disassembling.  If the
     actual opcode anded with the match field equals the opcode field,
     then we have found the correct instruction.  If pinfo is
     INSN_MACRO, then this field is the macro identifier.  */
  insn_t mask;

  /* A function to determine if a word corresponds to this instruction.
     Usually, this computes ((word & mask) == match).  */
  int (*match_func) (const struct rssb_opcode *op, insn_t word);

  /* For a macro, this is INSN_MACRO.  Otherwise, it is a collection
     of bits describing the instruction, notably any relevant hazard
     information.  */
  unsigned long pinfo;
};

/* Instruction is actually a macro.  It should be ignored by the
   disassembler, and requires special treatment by the assembler.  */
#define INSN_MACRO		0xffffffff

/* The mapping symbol states.  */
enum rssb_seg_mstate
{
  MAP_NONE = 0,		/* Must be zero, for seginfo in new sections.  */
  MAP_DATA,		/* Data.  */
  MAP_INSN,		/* Instructions.  */
};

extern const struct rssb_opcode rssb_opcodes[];
/* Data structure for a single instruction's arguments (Operands).  */

typedef struct
{
  /* Constant/immediate/absolute value.  */
  long constant;
  /* CC code.  */
  unsigned int cc;
  // /* Argument type.  only constants/labels*/
  // argtype type;
  /* Size of the argument (in bits) required to represent.  */
  int size;
  /* The type of the expression.  */
  unsigned char X_op;
}
argument;

typedef uint32_t rssb_instruction;
/* Internal structure to hold the various entities
   corresponding to the current assembling instruction.  */

typedef struct
{
  /* Number of arguments.  */
  int nargs;
  /* The argument data structure for storing args (operands).  */
  argument arg[MAX_OPERANDS];
  /* Instruction size (in bytes).  */
  rssb_instruction inst;
  int size;
    /* Expression used for setting the fixups (if any).  */
  expressionS exp;
  bfd_reloc_code_real_type rtype;
}
ins;
typedef struct
{
  /* Operand type.  */
  // operand_type op_type;
  /* Operand location within the opcode.  */
  unsigned int shift;
}
operand_desc;

/* Instruction data structure used in instruction table.  */

typedef struct
{
  /* Name.  */
  const char *mnemonic;
  /* Size (in words).  */
  unsigned int size;
  /* Constant prefix (matched by the disassembler).  */
  unsigned long match;  /* ie opcode */
  /* Match size (in bits).  */
  /* MASK: if( (i & match_bits) == match ) then match */
  int match_bits;
  /* Attributes.  */
  unsigned int flags;
  /* Operands (always last, so unreferenced operands are initialized).  */
  operand_desc operands[MAX_OPERANDS];
}
inst;
#endif

#endif /* _RSSB_OPCODES_H_ */
