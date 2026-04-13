#
# This makefile doesn't build a program to run.
# Its default target is `list` that shows the disassembled
# output of compiler.
#

CFLAGS = -march=rv64im -mabi=lp64 -Og -Wall -ffreestanding
LDFLAGS = -nostdlib -e0
OBJDUMP_FLAGS = --no-show-raw-insn --no-addresses --disassembler-color=terminal --visualize-jumps=extended-color

all : list

%.elf: %.o
	riscv64-unknown-elf-ld $(LDFLAGS) $< -o $@

%.o: %.c
	riscv64-unknown-elf-gcc $(CFLAGS) -c $< -o $@

list: codegen.elf
	riscv64-unknown-elf-objdump $(OBJDUMP_FLAGS) -d $<

clean:
	rm -f *.o *.bin *.elf *.list

.PHONY: all clean
