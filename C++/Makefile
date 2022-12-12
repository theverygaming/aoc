CC = g++
CFLAGS = -O3

all: days

days: day5.o day6.o day7.o day8.o day9.o day10.o day10_generator.o day11.o

%.o: %.cpp
	@echo "-> compiling $<"
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f *.o
