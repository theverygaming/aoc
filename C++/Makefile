CC = g++
CFLAGS = -O3

all: days

days: day5.o

%.o: %.cpp
	@echo "-> compiling $<"
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f *.o
