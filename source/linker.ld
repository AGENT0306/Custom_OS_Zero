ENTRY(_start)

SECTIONS
{
    /* Load the kernel at 1MB */
    . = 1M;

    /* Multiboot headers must be early in the binary */
    .boot :
    {
        KEEP(*(.multiboot))
    }

    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}