#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "sym_lookup.h"

//to build this program if main is enabled
// gcc -g3 -O0 sym_test.c sym_lookup.c -o sl.o
//to run this program symname
// not no argv checking!
// ./sl.o ../build/a.bin 1008

int main(int argc, char **argv) {
    if (argc < 3) return printf("Usage: %s <elf_file> <symbol_name>\n", argv[0]);
// now get the entire elf file
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) return EXIT_FAILURE;

    // Map the file into memory for easier access
    off_t size = lseek(fd, 0, SEEK_END);
    void* map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

    struct var_info vari = find_symbol_by_address(map, strtol(argv[2], NULL, 16));

    if (vari.name && *vari.name) {
        // char name[50];      //variable name
        // char sectname[50];  //section name for variable
        // uint32_t sh_flags;  //from elf, rw/e etc
        // uint32_t st_value;  //origin variable address
        printf("%s found in section %s (0x%x) sh_flags=0x%x address=0x%x\n", vari.name, vari.sectname, vari.baseaddress, vari.sh_flags, vari.st_value);
    } else {
        printf("match not found for 0x%lx\n", strtol(argv[2], NULL, 16));
    }
    vari = find_symbol_by_name(map, argv[3]);

    if (vari.name && *vari.name) {
        printf("%s found in section %s (0x%x) sh_flags=0x%x address=0x%x\n", vari.name, vari.sectname, vari.baseaddress, vari.sh_flags, vari.st_value);
    } else {
        printf("match not found for %s\n", argv[3]);
    }

    vari = find_sector_by_name(map, argv[4]);

    if (vari.name && *vari.name) {
        printf("%s found in section %s (0x%x) sh_flags=0x%x address=0x%x\n", vari.name, vari.sectname, vari.baseaddress, vari.sh_flags, vari.st_value);
    } else {
        printf("match not found for %s\n", argv[4]);
    }

    // stay tidy
    munmap(map, size);
    close(fd);

    return EXIT_SUCCESS;
}
