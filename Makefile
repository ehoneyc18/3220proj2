
CC=gcc
CFLAGS=-Wall -c

BINS=mythreads.o myqueue.o libmythreads.a


all: $(BINS)

mythreads.o: mythreads.c
	$(CC) $(CFLAGS) mythreads.c

myqueue.o: myqueue.c
	$(CC) $(CFLAGS) myqueue.c

libmythreads.a: mythreads.o
	ar -cvr libmythreads.a mythreads.o myqueue.o

clean:
	rm $(BINS)
