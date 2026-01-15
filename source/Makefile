# Toolchain setup
CC = x86_64-elf-gcc
AS = x86_64-elf-as
LD = x86_64-elf-ld

# Compiler flags: standard C99, no standard library, optimizations, warnings, and no PIE
CFLAGS = -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2

# Linker flags: script location, no stdlib, no PIE, and link libgcc
LDFLAGS = -ffreestanding -O2 -nostdlib -z max-page-size=0x1000 -lgcc

# Object files to build
OBJS = boot.o kernel.o 

# Default target
all: myos.bin

# Link the final kernel
myos.bin: $(OBJS)
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $(OBJS)
	x86_64-elf-objcopy --output-target=elf32-i386 $@ $@

# Compile C files
kernel.o: kernel.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Assemble assembly files
boot.o: boot.s
	$(AS) $< -o $@

# Helper to run QEMU
run: myos.bin
	qemu-system-x86_64 -kernel myos.bin

# Clean up build artifacts
clean:
	rm -f *.o myos.bin