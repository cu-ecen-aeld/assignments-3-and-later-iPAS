# https://opensource.com/article/18/8/what-how-makefile

BINS := writer
CC := gcc

LFLAGS = -lm

.PHONY = all clean run

all: ${BINS}

%: %.o
	${CC} ${LFLAGS} $< -o $@

%.o: %.c
	${CC} -c $<

clean:
	rm -rvf *.o ${BINS}

run: all
	./writer
	tail -n10000 /var/log/syslog | grep writer
