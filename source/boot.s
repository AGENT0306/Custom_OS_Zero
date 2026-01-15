/* boot_x64.s - 64-bit Trampoline */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic' number lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 4096
.global stack_bottom
.global stack_top
/* Reserve 16KiB for stack */
stack_bottom:
.skip 16384
stack_top:

/* Reserve space for Page Tables */
.align 4096
p4_table:
.skip 4096
p3_table:
.skip 4096
p2_table:
.skip 4096

.section .text
.global _start
.type _start, @function
.code32
_start:
    /* 1. Setup Stack */
    mov $stack_top, %esp

    /* 2. Check for CPUID/Long Mode support (omitted for brevity, we assume x64 CPU) */

    /* 3. Setup Page Tables (Identity Map first 1GB) */
    /* Map P4 entry 0 to P3 table */
    mov $p3_table, %eax
    or $0b11, %eax          /* Present + Writable */
    mov %eax, (p4_table)

    /* Map P3 entry 0 to P2 table */
    mov $p2_table, %eax
    or $0b11, %eax
    mov %eax, (p3_table)

    /* Map P2 entries (Huge Pages - 2MB each) */
    mov $0, %ecx            /* Counter */
.map_p2_table:
    mov $0x200000, %eax     /* 2MB size */
    mul %ecx
    or $0b10000011, %eax    /* Present + Writable + Huge */
    mov %eax, p2_table(,%ecx,8)
    inc %ecx
    cmp $512, %ecx          /* Map 512 entries (1GB total) */
    jne .map_p2_table

    /* 4. Enable PAE-paging */
    mov %cr4, %eax
    or $(1 << 5), %eax      /* Set PAE bit */
    mov %eax, %cr4

    /* 5. Load P4 to CR3 */
    mov $p4_table, %eax
    mov %eax, %cr3

    /* 6. Enable Long Mode in EFER MSR */
    mov $0xC0000080, %ecx
    rdmsr
    or $(1 << 8), %eax      /* Set LM bit */
    wrmsr

    /* 7. Enable Paging */
    mov %cr0, %eax
    or $(1 << 31), %eax     /* Set PG bit */
    mov %eax, %cr0

    /* 8. Load 64-bit GDT */
    lgdt gdt64_pointer
    ljmp $8, $long_mode_start

.code64
long_mode_start:
    /* Zero out data segment registers */
    mov $0, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    /* Call the C kernel */
    call kernel_main
    hlt

/* Global Descriptor Table (64-bit) */
.section .rodata
gdt64:
    .quad 0                 /* Zero entry */
    .quad (1<<43) | (1<<44) | (1<<47) | (1<<53) /* Code segment */
gdt64_pointer:
    .word . - gdt64 - 1
    .quad gdt64