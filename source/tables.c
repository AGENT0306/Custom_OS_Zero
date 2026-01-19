#include "tables.h"

// Define tables
struct GDTEntry gdt[5]; // Null, Kernel Code, Kernel Data, User Code, User Data, TSS
struct GDTPtr gdtr;
struct TSS tss;

struct IDTEntry idt;
struct IDTPtr idtr;

// Assembly helper to load the GDT/IDT
extern void gdt_flush(uint64_t gdtr_addr);
extern void idt_flush(uint64_t idtr_addr);
extern void tss_flush(void);

void gdt_set_gate(int num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran){
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access = access;
}

void init_gdt(){
    gdt_set_gate(0,0,0,0,0);
    
    // 1: Kernel Code Segment (64-bit, Ring 0)
    // Access: 0x9A (Present, Ring 0, Code, Readable)
    // Flags: 0xA0 (Long Mode, 4KB Granularity)
    gdt_set_gate(1, 0, 0, 0x9A, 0xA0);

    // 2: Kernel Data Segment
    // Access: 0x92 (Present, Ring 0, Data, Writable)
    gdt_set_gate(2, 0, 0, 0x92, 0xA0);

    // 3: User Code and 4: User Data 
    gdt_set_gate(3, 0, 0, 0xFA, 0xA0);
    gdt_set_gate(4, 0, 0, 0xF2, 0xA0);

    // 5: TSS Descriptor (Special 16-Byte Descriptor in x64)
    uint64_t tss_base = (uint64_t)&tss;
    uint64_t tss_limit = sizeof(tss);

    gdt_set_gate(5, tss_base, tss_limit, 0x89, 0x00); // Low 8 Bytes

    // High 8 Bytes of TSS Descriptor
    gdt[6].limit_low = (uint16_t)(tss_base >> 32);
    gdt[6].base_low = (uint16_t)(tss_base >> 48);
    gdt[6].base_middle = 0;
    gdt[6].access = 0;
    gdt[6].granularity = 0;
    gdt[6].base_high = 0;

    // Clear TSS and set RSP0

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt;
    
    gdt_flush((uint64_t)&gdtr);
    tss_flush();
}

void init_idt() {
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)&idt;

    // We will populate gates later

    idt_flush((uint64_t)&idtr);
}