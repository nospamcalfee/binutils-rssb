MEMORY {
    /* PC (rwx) : ORIGIN = 0, LENGTH = 4 */
    ROM (rx) : ORIGIN = 0x10, LENGTH = 0xff8
    RAM (rwx) : ORIGIN = 0x000, LENGTH = 0x10000
}
SECTIONS {
    .text : { *(.text) } > RAM
    .data : { *(.data) } > RAM
    .bss : { *(.bss) *(COMMON) } > RAM
}
