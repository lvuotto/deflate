CFLAGS += -Wall -Wextra -Werror -pedantic -std=c99 -O3

.PHONY: all clean

all: bitvec.o

debug: CFLAGS += -g
debug: bitvec.o 

clean:
	$(RM) bitvec.o
