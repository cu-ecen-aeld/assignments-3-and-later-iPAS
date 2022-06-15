# https://opensource.com/article/18/8/what-how-makefile

BINS := writer
# CC := gcc
CC := ${CROSS_COMPILE}gcc

LFLAGS = -lm

.PHONY = all clean run

all: ${BINS}

%: %.o
	${CC} ${LFLAGS} $< -o $@

%.o: %.c
	${CC} -c $<

clean:
	rm -rvf *.o ${BINS} test.txt

run: all
	./writer hello test.txt
	tail -n10000 /var/log/syslog | grep writer
