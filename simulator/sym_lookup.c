#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "sym_lookup.h"

//to build this program if main is enabled
//gcc -g3 -O0  sym_lookup.c -o sl.o

//internal lookup structure used to fill in var_info
struct sec_info {
    const char *name;
    uint32_t flags;
    uint32_t baseaddress;
    uint32_t sh_size;   //sector size
};


// get symtab and strtab return 1 when gotit
int get_syms(void *ptr, Elf32_Shdr  **symtab, Elf32_Shdr  **strtab) {
    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)ptr;
    Elf32_Shdr* shdr = (Elf32_Shdr*)(ptr + ehdr->e_shoff);
    *symtab = NULL;
    *strtab = NULL;
    // 1. Find .symtab and its linked .strtab
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_SYMTAB) {
            *symtab = &shdr[i];
            *strtab = &shdr[shdr[i].sh_link]; // Link field points to string table
            break;
        }
    }
    if (!*symtab || !*strtab) {
        printf("Symbol table not found. (Binary might be stripped)\n");
        return 0;   //failure
    }
    return 1;   //success
}
//find sector info
void get_sector_info(void *map, struct sec_info *sects, int sects_size) {
    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)map;
    Elf32_Shdr* shdr = (Elf32_Shdr*)(map + ehdr->e_shoff);
    Elf32_Shdr * secthdr = (Elf32_Shdr *) ((char *) map + ehdr->e_shoff + ehdr->e_shstrndx * sizeof(*secthdr));
    char *sectnames = (char *)(map + secthdr->sh_offset);
    for (int i = 0; i < ehdr->e_shnum; i++) {
        const char *name;
        secthdr =  (Elf32_Shdr *) ((char *) map + ehdr->e_shoff + i * sizeof(*secthdr));
        // printf("%p sh_offset=0x%x sh_type=%d sh_flags=%d sh_addr=0x%x name=%s\n",
        //         secthdr, secthdr->sh_offset, secthdr->sh_type,secthdr->sh_flags, secthdr->sh_addr, sectnames + secthdr->sh_name);
        if (secthdr->sh_type == SHT_PROGBITS && secthdr->sh_name && i < sects_size) {
            name = sectnames + secthdr->sh_name;
            // printf("%u sh_flags=%d %s \n", i, secthdr->sh_flags, name);
            sects[i].name = name;    //save sect name
            sects[i].flags = secthdr->sh_flags; //save r/w and other flags
            sects[i].sh_size = secthdr->sh_size;
            sects[i].baseaddress = secthdr->sh_addr;
        }
    }
}

//return filled out var_info if found
struct var_info find_sector_by_name(void *map, const char *name) {
    struct var_info vari = {0};
    struct sec_info sects[256] = {};  //temporary section storage
    get_sector_info(map, sects, sizeof(sects)/sizeof(sects[0]));
    for (int i = 0; i < sizeof(sects)/sizeof(sects[0]); i++) {
        if (sects[i].name) {
            if (!strcmp(sects[i].name, name)) {
                //found the match, prep the return values in the struct
                //secname and value are not relevent
                strncpy(vari.name, sects[i].name, sizeof(vari.name));
                vari.name[sizeof(vari.name) - 1] = '\0';
                vari.st_value = 0;
                vari.sh_flags = sects[i].flags; //this variable section r/w flags
                vari.sh_size = sects[i].sh_size;
                vari.baseaddress = sects[i].baseaddress;
                vari.sectname[0] = '\0';   //this is a sector so no parent
            }
        }
    }
    return vari;
}

//return null pointers if error
struct var_info find_symbol_by_name(void *map, const char *name) {
    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)map;
    Elf32_Shdr* shdr = (Elf32_Shdr*)(map + ehdr->e_shoff);

    Elf32_Shdr *symtab = NULL, *strtab = NULL;
    struct var_info vari = {0};
    struct sec_info sects[256];  //temporary section storage

    get_sector_info(map, sects, sizeof(sects)/sizeof(sects[0]));

    if (!get_syms(map, &symtab, &strtab)) {
        return vari; //failure
    }
    // 2. Iterate through symbols
    Elf32_Sym* symbols = (Elf32_Sym*)(map + symtab->sh_offset);
    char* names = (char*)(map + strtab->sh_offset);
    int num_symbols = symtab->sh_size / symtab->sh_entsize;

    for (int i = 0; i < num_symbols; i++) {
        // printf("sym attempt %d: %s\n", i, &names[symbols[i].st_name]);
        // printf("%p st_shndx=0x%x st_value=0x%x st_size=0x%x\n", &symbols[i], symbols[i].st_shndx, symbols[i].st_value, symbols[i].st_size );
        if (symbols[i].st_name != 0 ) {
            if (!strcmp(&names[symbols[i].st_name], name)) {
                //found the match, prep the return values in the struct
                strncpy(vari.name, &names[symbols[i].st_name], sizeof(vari.name));
                vari.name[sizeof(vari.name) - 1] = '\0';
                vari.st_value = symbols[i].st_value;
                if (symbols[i].st_shndx == SHN_ABS) {
                    strncpy(vari.sectname, "ABS", sizeof(vari.sectname));
                    vari.sh_flags = 0;
                    vari.sh_size = 0;
                    vari.baseaddress = 0;
                } else if (symbols[i].st_shndx != SHN_UNDEF) {
                    strncpy(vari.sectname, sects[symbols[i].st_shndx].name, sizeof(vari.sectname));
                    vari.sh_flags = sects[symbols[i].st_shndx].flags; //this variable section r/w flags
                    vari.sh_size = sects[i].sh_size;
                    vari.baseaddress = sects[symbols[i].st_shndx].baseaddress;
                }
                return vari; //success
            }
        }
    }
    return vari;    //not found exit
}

//return null pointers if error
struct var_info find_symbol_by_address(void *map, uint32_t target_addr) {
    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)map;
    Elf32_Shdr* shdr = (Elf32_Shdr*)(map + ehdr->e_shoff);

    Elf32_Shdr *symtab = NULL, *strtab = NULL;
    struct var_info vari = {0};
    struct sec_info sects[256];  //temporary section storage

    if (!get_syms(map, &symtab, &strtab)) {
        return vari; //failure
    }
    //find sector info
    get_sector_info(map, sects, sizeof(sects)/sizeof(sects[0]));

    // 2. Iterate through symbols
    Elf32_Sym* symbols = (Elf32_Sym*)(map + symtab->sh_offset);
    char* names = (char*)(map + strtab->sh_offset);
    int num_symbols = symtab->sh_size / symtab->sh_entsize;

    for (int i = 0; i < num_symbols; i++) {
        // printf("sym attempt %d: %s\n", i, &names[symbols[i].st_name]);
        // printf("%p st_shndx=0x%x st_value=0x%x st_size=0x%x\n", &symbols[i], symbols[i].st_shndx, symbols[i].st_value, symbols[i].st_size );
        if (symbols[i].st_name != 0 &&
                *(uint32_t *)&symbols[i].st_value == target_addr) {
            // printf("Match found: 0x%x %s\n", *(uint32_t *)&symbols[i].st_value, &names[symbols[i].st_name]);
            strncpy(vari.name, &names[symbols[i].st_name], sizeof(vari.name));
            vari.name[sizeof(vari.name) - 1] = '\0';
            vari.st_value = symbols[i].st_value;
            if (symbols[i].st_shndx == SHN_ABS) {
                strncpy(vari.sectname, "ABS", sizeof(vari.sectname));
                vari.baseaddress = 0;
                vari.sh_flags = 0;
                vari.sh_size = 0;
            } else if (symbols[i].st_shndx != SHN_UNDEF) {
                strncpy(vari.sectname, sects[symbols[i].st_shndx].name, sizeof(vari.sectname));
                vari.sh_flags = sects[symbols[i].st_shndx].flags; //this variable section r/w flags
                vari.sh_size = sects[i].sh_size;
                vari.baseaddress = sects[symbols[i].st_shndx].baseaddress;
            }
            // return vari; //success
        }
    }
    // printf("No matching symbol found for address 0x%x\n", target_addr);
    return vari; //no error, but not match ptrs will be null
 }
