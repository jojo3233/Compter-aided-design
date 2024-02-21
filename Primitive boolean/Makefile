TARGET	= boolean
CC	= g++

SRCS	= main.cpp
OBJS	= ${SRCS:.c=.o}
CFLAGS  = -g -Iinclude

${TARGET}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${TARGET}

.SUFFIXES: .c .o

.c.o:
	${CC} -o $@ -c $<

clean:
	rm -f core *~ $(TARGET); \
	rm *.o