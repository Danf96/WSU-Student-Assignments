.SUFFIXES: .c .o
CC = gcc
CCFLAGS = -g -pedantic -Wall
EXEC = lineNum
OBJS = main.o lineNum.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}

clean:
	rm -f ${EXEC} ${OBJS}

main.o: main.c lineNum.h

lineNum.o: lineNum.c lineNum.h


