// for elf symbol lookup
#ifndef SYM_LOOKUP_H
#define SYM_LOOKUP_H
// struct defn
struct var_info {
    char name[50];      //variable name
    char sectname[50];  //section name for variable
    uint32_t sh_flags;  //from elf, rw/e etc
    uint32_t sh_size;   //sector size
    uint32_t st_value;  //origin variable address
    uint32_t baseaddress; //section base address
};

// Function prototypes (declarations)
struct var_info find_symbol_by_address(void *map, uint32_t target_addr);
struct var_info find_symbol_by_name(void *map, const char *name);
struct var_info find_sector_by_name(void *map, const char *name);
#endif
