CC = g++
CFLAGS = -O3

all: days

days: day5.o day6.o day7.o day8.o

%.o: %.cpp
	@echo "-> compiling $<"
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f *.o
