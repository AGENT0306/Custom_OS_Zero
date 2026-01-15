#ifndef TABLES_H
#define TABLES_H

#include <stdint.h>

// 1. IDT Entry (16 Bytes)
struct IDTEntry{
    uint16_t offset_low; // Lower 16 bits of handler address
    uint16_t selector; // Kernal Code Segment Selector
    uint8_t ist; // Interrupt Stack Table offset (0 = disabled)
    uint8_t type_attributes; // Gate Type, DPL, P fields
    uint16_t offset_mid; // Middle 16 bits of handler address
    uint32_t offset_high; // Upper 32 bits of handler address
    uint32_t zero; // Reserved (must be zero)
} __attribute__((packed));

struct IDTPtr{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// 2. GDT Entry (8 Bytes)
struct GDTEntry{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct GDTPtr{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// 3. TSS (104 Bytes)
struct TSS{
    uint32_t reserved1;
    uint64_t rsp0; // Stack pointer for Ring 0 (Kernal) interrupts
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved2;
    uint64_t ist1; // Interrupt Stack Table Pointers
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t iomap_base;
} __attribute__((packed));

// Functions we will implement in tables.c
void init_gdt(void);
void init_idt(void);

#endif 