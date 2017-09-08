#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include "mythreads.h"

typedef struct Thread
{
  thFuncPtr funcPtr;
  void *argPtr;
  int threadID;
  struct Thread *next;
  ucontext_t *context;
  void *resultPtr;
  int join;
} Thread;

typedef struct threadPool
{
  Thread *front;
  Thread *back;
  int numThreads;
} threadPool;

threadPool *buildPool();
Thread *buildThread(thFuncPtr newFuncPtr, void *newArgPtr, int newThreadID, ucontext_t *currContext);
void destroyPool(threadPool *pool);
int isEmpty(threadPool *pool);
int isMember(threadPool *pool, int ID);
int addToFront(threadPool *pool, Thread *new);
int addToBack(threadPool *pool, Thread *new);
Thread *getFromPool(threadPool *pool);
Thread *getByID(threadPool *pool, int ID);

#endif
