ASM=nasm
ASMFLAGS=-f elf64
LD=ld

all: program

main.o: main.asm lib.inc colon.inc words.inc
	$(ASM) $(ASMFLAGS) -o $@ $<

%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<
	
program: lib.o dict.o main.o
	$(LD) -o $@ $^

clean:
	rm -f *.o program

.PHONY: clean
