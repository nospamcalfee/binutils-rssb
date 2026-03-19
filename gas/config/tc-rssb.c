/* tc-cr16.c -- Assembler code for the CR16 CPU core.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

   Contributed by M R Swami Reddy <MR.Swami.Reddy@nsc.com>

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "as.h"
#include "bfd.h"
#include "opcode/rssb.h"

//comments
const char comment_chars[] = "#";
const char line_comment_chars[] = "//";
const char line_separator_chars[] = ";";

//floating point
const char EXP_CHARS[] = "";
const char FLT_CHARS[] = "";

/* continue the horrible tradition of Global variables.  */

/* Array to hold an instruction encoding.  */
long output_opcode[2];

/* Nonzero means a relocatable symbol.  */
int relocatable;

#ifdef OBJ_ELF
/* Pre-defined "_GLOBAL_OFFSET_TABLE_"  */
symbolS * GOT_symbol;
#endif

/* Current instruction we're assembling.  */
const inst *instruction;

/* Endianness. */
extern int target_big_endian;

//end of horrible globals
/* This table describes all the machine specific pseudo-ops
   the assembler has to support.  The fields are:
   *** Pseudo-op name without dot.
   *** Function to call to execute this pseudo-op.
   *** Integer arg to pass to the function.  */

const pseudo_typeS md_pseudo_table[] =
{
  /* In rssd machine, align is in bytes (not a ptwo boundary).  */
  {
    (char *) 0,
    (void(*)(int))0, 0
  }
};

/**
 * @brief Prints a hexadecimal and character dump of a memory block.
 *
 * @param data A pointer to the memory block.
 * @param size The number of bytes to print.
 */
static void dump_bytes(const void* data, size_t size) {
    const unsigned char* byte = (const unsigned char*)data; // Treat memory as unsigned bytes
    size_t i, j;

    for (i = 0; i < size; i += 16) {
        // Print the memory address offset
        printf("%08lX | ", (unsigned long)i);

        // Print the hex values for 16 bytes (or fewer, if at the end)
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                printf("%02X ", byte[i + j]);
            } else {
                printf("   "); // Print spaces for alignment if less than 16 bytes
            }
            if (j == 7) {
                printf("- "); // Add a separator in the middle
            }
        }

        // // Print the ASCII representation
        // printf("| ");
        // for (j = 0; j < 16; j++) {
        //     if (i + j < size) {
        //         // Use isprint() to check if the character is printable, otherwise print a dot
        //         printf("%c", isprint(byte[i + j]) ? byte[i + j] : '.');
        //     }
        // }
        printf("\n");
    }
}

/* Machine-dependent usage-output.  */

// void
// md_operand (void *) {
// }
void
md_operand (struct expressionS *op ATTRIBUTE_UNUSED)
{
    printf("%s\n",__func__);
}

void md_begin(void) {
      printf("%s\n",__func__);
  return;
}
void
md_number_to_chars (char *ptr, valueT val, int nbytes)
{
  if (target_big_endian)
    number_to_chars_bigendian (ptr, val, nbytes);
  else
    number_to_chars_littleendian (ptr, val, nbytes);
}

static int
emit_rssb_operand (expressionS *expr)
{
  char *frag = frag_more (RSSB_OPERAND_SIZE);

    printf("%s\n",__func__);

  if (expr->X_op == O_symbol) {
      fix_new_exp (frag_now, frag - frag_now->fr_literal, 4, expr, 0,
                   BFD_RELOC_32);
  } else if (expr->X_op != O_constant) {
      as_bad ("%s argument must be a symbol or constant", __func__);
      return -1;
    }

  md_number_to_chars (frag, expr->X_add_number, RSSB_OPERAND_SIZE);
    dump_bytes(frag, RSSB_OPERAND_SIZE);

  return 0; /* Return 0 on success.  */
}


/* This is the guts of the machine-dependent assembler.  OP points to a
   machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.  */

void
md_assemble (char *op)
{
    // ins rssb_ins;
    expressionS addr_field;

    printf("%s Inst string: %s\n",__func__ , op);
    char *s = strcasestr(op, "rssb");
    if (!s) {
        //opcode is optional!
        s = op;
    } else {
        s += 4; //skip the op-code
    }

    input_line_pointer = s;

    printf("%s symb string: %s\n",__func__ , s);
    expression(&addr_field);
    emit_rssb_operand( &addr_field);
    return;
}

symbolS *
md_undefined_symbol (char *name)
{
   printf("%s\n",__func__);
  if (*name == '_' && *(name + 1) == 'G'
      && strcmp (name, "_GLOBAL_OFFSET_TABLE_") == 0)
    {
      if (!GOT_symbol)
  {
    if (symbol_find (name))
      as_bad (_("GOT already in symbol table"));
    GOT_symbol = symbol_new (name, undefined_section,
           &zero_address_frag, 0);
  }
      return GOT_symbol;
    }
  return 0;
}
const char *
md_atof (int type ATTRIBUTE_UNUSED, char *litP ATTRIBUTE_UNUSED, int *sizeP ATTRIBUTE_UNUSED)
{
  printf("%s\n",__func__);
  return 0; //ieee_md_atof (type, litP, sizeP, target_big_endian);
}

/* Round up a section size to the appropriate boundary.  */

valueT
md_section_align (segT seg, valueT val)
{
  printf("%s\n",__func__);
  /* Round .text section to a multiple of 2.  */
  if (seg == text_section)
    return (val + 1) & ~1;
  return val;
}

void
md_convert_frag (bfd *abfd ATTRIBUTE_UNUSED,
     asection *sec ATTRIBUTE_UNUSED,
     fragS *fragP ATTRIBUTE_UNUSED)
{
  printf("%s\n",__func__);
  as_fatal(_("unexpected call"));
    return;
}

/* Apply a fixS (fixup of an instruction or data that we didnt have
   enough info to complete immediately) to the data in a frag.
   Since linkrelax is nonzero and TC_LINKRELAX_FIXUP is defined to disable
   relaxation of debug sections, this function is called only when
   fixuping relocations of debug sections.  */

/* See whether we need to force a relocation into the output file.
   This is used to force out switch and PC relative relocations when
   relaxing.  */

int
rssb_force_relocation (fixS *fix)
{
  printf("%s\n",__func__);
  if (generic_force_reloc (fix) || 0 /*SWITCH_TABLE (fix)*/)
    return 1;

  return 0;
}

void
md_apply_fix (fixS *fixP ATTRIBUTE_UNUSED, valueT *valP ATTRIBUTE_UNUSED, segT seg ATTRIBUTE_UNUSED)
{
  printf("%s\n",__func__);
  return;
}

/* Generate a relocation entry for a fixup.  */

arelent *
tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS * fixp)
{
  printf("%s\n",__func__);
  arelent *reloc = XNEW (arelent);
  reloc->sym_ptr_ptr = XNEW (asymbol *);
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);

  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  reloc->addend = fixp->fx_offset;

  return reloc;
}

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from (fixS *fixp ATTRIBUTE_UNUSED)
{
  printf("%s\n",__func__);
  as_fatal(_("unexpected call"));
  return 0; //fixp->fx_frag->fr_address + fixp->fx_where;
}

int md_estimate_size_before_relax(fragS *fragp ATTRIBUTE_UNUSED, asection *seg ATTRIBUTE_UNUSED) {
  printf("%s\n",__func__);
  as_fatal(_("unexpected call"));
  return 0;
}
long
md_pcrel_from_section (fixS *fixP, segT sec)
{
  printf("%s\n",__func__);
  if (fixP->fx_addsy != NULL
      && (!S_IS_DEFINED (fixP->fx_addsy)
      || S_GET_SEGMENT (fixP->fx_addsy) != sec))
    {
      /* The symbol is undefined (or is defined but not in this section).
         Let the linker figure it out.  */
      return 0;
    }
  return fixP->fx_frag->fr_address + fixP->fx_where;
}

bool
rssb_eol_in_insn (char *line)
{
   /* Allow a new-line to appear in the middle of a multi-issue instruction.  */

   char *temp = line;

  if (*line != '\n')
    return false;

  /* A semi-colon followed by a newline is always the end of a line.  */
  if (line[-1] == ';')
    return false;

  if (line[-1] == '|')
    return true;

  /* If the || is on the next line, there might be leading whitespace.  */
  temp++;
  while (is_whitespace (*temp))
    temp++;

  if (*temp == '|')
    return true;

  return false;
}

bool
rssb_start_label (char *s)
{
  while (*s != 0)
    {
      if (*s == '(' || *s == '[')
  return false;
      s++;
    }

  return true;
}
/* Return true if the fix can be handled by GAS, false if it must
   be passed through to the linker.  */

bool
rssb_fix_adjustable (fixS *fixP)
{
  switch (fixP->fx_r_type)
    {
  // /* Adjust_reloc_syms doesn't know about the GOT.  */
  //   case BFD_RELOC_RSSB_GOT:
  //   case BFD_RELOC_RSSB_PLTPC:
  // /* We need the symbol name for the VTABLE entries.  */
  //   case BFD_RELOC_VTABLE_INHERIT:
  //   case BFD_RELOC_VTABLE_ENTRY:
  //     return 0;

    default:
      return 1;
    }
}
/* Options */

const char md_shortopts[] = "";

enum options
{
  OPTION_EB = OPTION_MD_BASE,
  OPTION_EL
};

const struct option md_longopts[] = { { "EB", no_argument, NULL, OPTION_EB },
                                { "EL", no_argument, NULL, OPTION_EL },
                                { NULL, no_argument, NULL, 0 } };

const size_t md_longopts_size = sizeof (md_longopts);
// const struct option md_longopts[] =
// {
//   {NULL, no_argument, NULL, 0}
// };
// const size_t md_longopts_size = sizeof (md_longopts);

int
md_parse_option (int c, const char *arg ATTRIBUTE_UNUSED)
{
    printf("%s\n",__func__);
  switch (c)
    {
    case OPTION_EB:
      target_big_endian = 1;
      break;
    case OPTION_EL:
      target_big_endian = 0;
      break;
    default:
      return 0;
    }
  return 0; //default le
}

void
md_show_usage (FILE *stream)
{
  fprintf (stream, _ ("\
  -EB                     assemble for a big endian system\n\
  -EL                     assemble for a little endian system (default)\n"));
}