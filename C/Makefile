CC = gcc
CFLAGS = -static -I shitcstd/include -nostdlib -Wall -m32 -march=i386 -fno-pie -fno-stack-protector -ffreestanding
CLIBS = -Lshitcstd/build -lshitcstd

all: bindir shitcstd/build/libshitcstd.a days

bindir:
	mkdir -p bin

shitcstd/build/libshitcstd.a:
	@mkdir -p shitcstd/build
	@cd shitcstd/build && cmake .. && make

days: day3.o

%.o: %.c
	@echo "-> compiling $<"
	@$(CC) $(CFLAGS) $< -o $@ $(CLIBS)

clean:
	@rm -f *.o
	@rm -rf shitcstd/build/
