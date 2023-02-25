.SUFFIXES: .c .o
CC = gcc
CCFLAGS = -g
EXEC = regtonfa
OBJS = regtonfa.o stack.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f ${EXEC} ${OBJS}

regtonfa.o: regtonfa.c regtonfa.h
stack.o: stack.c stack.h


