/* rssb for 32-bit ELF.
   Copyright (C) 2005-2025 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"

#define ELF_ARCH bfd_arch_rssb
#define ELF_MAXPAGESIZE 0x4000
#define ELF_MACHINE_CODE EM_RSSB

#undef TARGET_LITTLE_SYM
#define TARGET_LITTLE_SYM rssb_elf32_vec

#undef TARGET_LITTLE_NAME
#define TARGET_LITTLE_NAME "elf32-rssb"

#define bfd_elf32_bfd_reloc_type_lookup rssb_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup rssb_reloc_name_lookup

/* Retrieve a howto ptr using a BFD reloc_code.  */

static reloc_howto_type *
rssb_reloc_type_lookup (bfd *abfd,
          bfd_reloc_code_real_type code)
{
  // unsigned int i;

  // for (i = 0; i < R_CR16_MAX; i++)
  //   if (code == rssb_reloc_map[i].bfd_reloc_enum)
  //     return &rssb_elf_howto_table[rssb_reloc_map[i].rssb_reloc_type];

  // _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
  //         abfd, code);
  // return NULL;
  return 0;
}

static reloc_howto_type *
rssb_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
          const char *r_name)
{
  // unsigned int i;

  // for (i = 0; ARRAY_SIZE (cr16_elf_howto_table); i++)
  //   if (cr16_elf_howto_table[i].name != NULL
  // && strcasecmp (cr16_elf_howto_table[i].name, r_name) == 0)
  //     return cr16_elf_howto_table + i;

  // return NULL;
  return 0;
}
//last include
#include "elf32-target.h"
