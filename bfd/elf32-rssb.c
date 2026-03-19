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
#include "elf/rssb.h"

#undef TARGET_LITTLE_NAME
#define TARGET_LITTLE_NAME "elf32-rssb"

static reloc_howto_type rssb_elf_howto_table[] =
{
  /* Do nothing.  */
  HOWTO (R_RSSB_NONE,          /* type */
         0,                      /* rightshift */
         3,                      /* size (0 = byte, 1 = short, 2 = long) */
         0,                      /* bitsize */
         false,                  /* pc_relative */
         0,                      /* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc,  /* special_function */
         "R_RSSB_NONE",        /* name */
         false,                  /* partial_inplace */
         0,                      /* src_mask */
         0,                      /* dst_mask */
         false),                 /* pcrel_offset */

  HOWTO (R_RSSB_32,            /* type */
         0,                      /* rightshift by 2 to divide the address by 4 */
         2,                      /* size (0 = byte, 1 = short, 2 = long) */
         32,                     /* bitsize */
         false,                  /* pc_relative */
         0,                      /* bitpos */
         complain_overflow_bitfield, /* complain_on_overflow */
         bfd_elf_generic_reloc,  /* special_function */
         "R_RSSB_32",          /* name */
         false,                  /* partial_inplace */
         0x00000000,             /* src_mask */
         0xffffffff,             /* dst_mask */
         false),                 /* pcrel_offset */
};

struct rssb_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned int elf_reloc_type;
};

static const struct rssb_reloc_map rssb_reloc_map [] =
{
  { BFD_RELOC_NONE, R_RSSB_NONE },
  { BFD_RELOC_32,   R_RSSB_32   }
};

static reloc_howto_type *
rssb_reloc_type_lookup (bfd *abdf ATTRIBUTE_UNUSED, bfd_reloc_code_real_type code)
{
  unsigned int i;
  for (i = 0; i < sizeof (rssb_reloc_map) / sizeof (rssb_reloc_map[0]);
       i++)
    if (rssb_reloc_map[i].bfd_reloc_val == code)
      return &rssb_elf_howto_table[rssb_reloc_map[i].elf_reloc_type];

  return NULL;
}

static reloc_howto_type *
rssb_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED, const char *name)
{
  unsigned int i;
  for (i = 0;
       i < sizeof (rssb_elf_howto_table) / sizeof (rssb_elf_howto_table[0]);
       i++)
      if (rssb_elf_howto_table[i].name != NULL && strcasecmp (rssb_elf_howto_table[i].name, name) == 0)
        return &rssb_elf_howto_table[i];

  return NULL;
}
static bool
rssb_info_to_howto_rela (bfd *abfd,
                           arelent *cache_ptr,
                           Elf_Internal_Rela *dst)
{
  unsigned int r_type = ELF32_R_TYPE (dst->r_info);
  if (r_type >= (unsigned int) R_RSSB_max)
    {
      _bfd_error_handler (_("%p: unsupported relocation type %#x"), abfd, r_type);
      bfd_set_error (bfd_error_bad_value);
      return false;
    }
  cache_ptr->howto = &rssb_elf_howto_table[r_type];
  return true;
}

#define ELF_ARCH bfd_arch_rssb
#define ELF_MACHINE_CODE EM_RSSB
#define ELF_MAXPAGESIZE 1

#define TARGET_LITTLE_SYM rssb_elf32_vec

#define elf_info_to_howto rssb_info_to_howto_rela
#define elf_info_to_howto_rel NULL

#define bfd_elf32_bfd_reloc_type_lookup rssb_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup rssb_reloc_name_lookup

//last include
#include "elf32-target.h"
