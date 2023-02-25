.SUFFIXES: .c .o
CC = gcc
CCFLAGS = -g
EXEC = assignment7
OBJS = wordpairs.o hashtable.o getWord.o crc64.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}

clean:
	rm -f ${EXEC} ${OBJS}

wordpairs.o: assignment7.c wordpairs.h

hashtable.o: hashtable.c hashtable.h 

getWord.o: getWord.c getWord.h 

crc64.o: crc64.c crc64.h


