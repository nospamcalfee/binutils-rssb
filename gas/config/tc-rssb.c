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

const char md_shortopts[] = {};
const struct option md_longopts[] = {};
const size_t md_longopts_size = sizeof(md_longopts);

//comments
const char comment_chars[] = "";
const char line_comment_chars[] = "//";
const char line_separator_chars[] = ";";

//floating point
const char EXP_CHARS[] = "";
const char FLT_CHARS[] = "";

/* This table describes all the machine specific pseudo-ops
   the assembler has to support.  The fields are:
   *** Pseudo-op name without dot.
   *** Function to call to execute this pseudo-op.
   *** Integer arg to pass to the function.  */

const pseudo_typeS md_pseudo_table[] =
{
  /* In bssd machine, align is in bytes (not a ptwo boundary).  */
  {
    (char *) 0,
    (void(*)(int))0, 0
  }
};
/* Process machine-dependent command line options.  Called once for
   each option on the command line that the machine-independent part of
   GAS does not understand.  */

int
md_parse_option (int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

/* Machine-dependent usage-output.  */

void
md_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
  return;
}

void md_begin(void) {
  return;
}

/* This is the guts of the machine-dependent assembler.  OP points to a
   machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.  */

void
md_assemble (char *op)
{
    return;
}

symbolS *
md_undefined_symbol (char *name)
{
  // if (*name == '_' && *(name + 1) == 'G'
  //     && strcmp (name, "_GLOBAL_OFFSET_TABLE_") == 0)
  //   {
  //     if (!GOT_symbol)
  // {
  //   if (symbol_find (name))
  //     as_bad (_("GOT already in symbol table"));
  //   GOT_symbol = symbol_new (name, undefined_section,
  //          &zero_address_frag, 0);
  // }
  //     return GOT_symbol;
  //   }
  return 0;
}
const char *
md_atof (int type, char *litP, int *sizeP)
{
  return 0; //ieee_md_atof (type, litP, sizeP, target_big_endian);
}

/* Round up a section size to the appropriate boundary.  */

valueT
md_section_align (segT seg, valueT val)
{
  // /* Round .text section to a multiple of 2.  */
  // if (seg == text_section)
  //   return (val + 1) & ~1;
  // return val;
  return 0;
}

void
md_convert_frag (bfd *abfd ATTRIBUTE_UNUSED,
     asection *sec ATTRIBUTE_UNUSED,
     fragS *fragP)
{
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
  if (generic_force_reloc (fix) || 0 /*SWITCH_TABLE (fix)*/)
    return 1;

  return 0;
}

void
md_apply_fix (fixS *fixP, valueT *valP, segT seg)
{
  return;
}

/* Generate a relocation entry for a fixup.  */

arelent *
tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS * fixP)
{
  return 0;
}

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from (fixS *fixp)
{
  as_fatal(_("unexpected call"));
  return 0; //fixp->fx_frag->fr_address + fixp->fx_where;
}

int md_estimate_size_before_relax(fragS *fragp, asection *seg) {
  as_fatal(_("unexpected call"));
  return 0;
}
long
md_pcrel_from_section (fixS *fixP, segT sec)
{
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
