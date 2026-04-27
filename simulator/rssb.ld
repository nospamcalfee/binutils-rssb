MEMORY {
    /* PC (rwx) : ORIGIN = 0, LENGTH = 4 */
    ROM (rx) : ORIGIN = 0x0, LENGTH = 0x1000
    ROM2 (r) : ORIGIN = 0x1000 LENGTH = 0x1000
    RAM (rwx) : ORIGIN = 0x2000, LENGTH = 0x1000
}
SECTIONS {
    .text : { *(.text) } > ROM
    .rodata : { *(.rodata) } > ROM2
    .data : { *(.data) } > RAM
    .bss : { *(.bss) *(COMMON) } > RAM
}
