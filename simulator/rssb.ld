MEMORY {
    /* PC (rwx) : ORIGIN = 0, LENGTH = 4 */
    ROM (rx) : ORIGIN = 0x0, LENGTH = 0x1000
    ROM2 (r) : ORIGIN = 0x1000 LENGTH = 0x1000
    RAM (rwx) : ORIGIN = 0x2000, LENGTH = 0x1000
}
SECTIONS {
    .text : { *(.text) } > ROM
    . = ALIGN(16);
    .rodata : {
                    *(.rodata) } > ROM
    .data : { . = ALIGN(32);
            *(.data)
            *(.bss) *(COMMON) } > ROM
    /* .bss : { *(.bss) *(COMMON) } > RAM */
}
