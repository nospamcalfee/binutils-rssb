/* Disassemble rssb.
   Copyright (C) 2021 Free Software Foundation, Inc.
   Contributed by Joe Legg.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "disassemble.h"
#include "bfd.h"
#include "opcode/rssb.h"

int
print_insn_rssb (bfd_vma addr, struct disassemble_info *info)
{
  /* Buffer to read the instruction into.  */
  bfd_byte insn_buf[RSSB_INSN_SIZE];

  int err = (*info->read_memory_func) (addr, insn_buf, RSSB_INSN_SIZE, info);

  if (err)
    {
      (*info->memory_error_func) (err, addr, info);
      return -1;
    }

  unsigned long a;

  bfd_vma (*get32) (const void *)
      = info->endian == BFD_ENDIAN_BIG ? bfd_getb32 : bfd_getl32;

  a = get32 (insn_buf);

  (*info->fprintf_func) (info->stream, "rssb %#0lx", a);

  return RSSB_INSN_SIZE;
}