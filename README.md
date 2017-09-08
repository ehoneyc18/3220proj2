Earl Honeycutt (ehoneyc@g.clemson.edu)

This project is divided across two .c files and two .h files:

1) myqueue.c (and myqueue.h)

myqueue.c contains a fairly loose queue implementation that allows for enqueueing at both the 
front and the back of the queue. Also included are some search functions (mainly for finding
threads by a certain ID number) and constructors and destructors.

2) mythreads.c (and mythreads.h)

mythreads.c contains an implementation of a basic thread management API. Additionally
implemented are some basic locking and unlocking functionalities as well as a wait/signal condition variable.

KNOWN PROBLEMS:

weirdly wasn't able to free() contexts themselves in thread destructors

some memory leaks (largely out of fear of freeing things that were still being used)

DESIGN (References):

https://computing.llnl.gov/tutorials/pthreads/
http://www.cplusplus.com/reference/thread/thread/join/
https://phoxis.org/2011/04/27/c-language-constructors-and-destructors-with-gcc/
