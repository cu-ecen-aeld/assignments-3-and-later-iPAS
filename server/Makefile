# https://opensource.com/article/18/8/what-how-makefile

BINS := aesdsocket
CC := ${CROSS_COMPILE}gcc

CFLAGS = -Wall -Werror
LFLAGS = -lm

.PHONY = all default clean run

all: default
default: ${BINS}

%: %.o
	${CC} ${CFLAGS} ${LFLAGS} $< -o $@

%.o: %.c
	${CC} -c $<

clean:
	rm -rvf *.o ${BINS}

run: all
	./aesdsocket
