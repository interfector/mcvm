# MCVM

VERSION = 0.0.1

# includes and libs
LIBS = -ldl

# flags
CFLAGS = -Wall -O3 ${LIBS} -DVERSION=\"${VERSION}\" -I./include

SRC = src/main.c src/sim.c
BIN = mcvm

all:
	gcc -o $(BIN) $(SRC) $(CFLAGS)

clean:
	rm -f $(BIN)

install: all
	cp -f $(BIN) /usr/local/bin/
