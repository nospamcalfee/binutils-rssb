MEMORY {
    PC (rwx) : ORIGIN = 0, LENGTH = 4
    ROM (rx) : ORIGIN = 4, LENGTH = 0xff8
    RAM (rwx) : ORIGIN = 0x1000, LENGTH = 0x100
}
SECTIONS {
    .text : { *(.text) } > ROM
    .data : { *(.data) } > RAM
    .bss : { *(.bss) *(COMMON) } > RAM
}
