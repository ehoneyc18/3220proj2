#include "myqueue.h"

//init threadPool
threadPool *buildPool()
{
  threadPool *pool = (threadPool*) malloc(sizeof(threadPool));

  pool->numThreads = 0;
  pool->front = NULL;
  pool->back = NULL;

  return pool;
}

//init Thread
Thread *buildThread(thFuncPtr newFuncPtr, void *newArgPtr, int newThreadID, ucontext_t *currContext)
{
  Thread *new = (Thread*) malloc(sizeof(Thread));

  new->funcPtr = newFuncPtr;
  new->argPtr = newArgPtr;
  new->threadID = newThreadID;
  new->context = currContext;
  new->next = NULL;
  new->resultPtr = NULL;
  new->join = -1;

  return new;
}

//properly free a whole pool
void destroyPool(threadPool *pool) {
  Thread *destroy;
  while (!isEmpty(pool))
  {
    destroy = getFromPool(pool);
    free(destroy->context->uc_stack.ss_sp);
    free(destroy);
  }
  free(pool);
}

//check if pool is empty
int isEmpty(threadPool *pool)
{
  if (pool == NULL)
    return 0;

  if (pool->numThreads == 0)
    return 1;
  else
    return 0;
}

//check for membership based on ID
int isMember(threadPool *pool, int ID)
{
  Thread *toFind = pool->front;

  int i;

  for (i = 0; i < pool->numThreads; i++)
  {
    if (toFind == NULL)
      return 0;
    else if (toFind->threadID == ID)
      return 1;

    toFind = toFind->next;
  }
  return 0;
}

//add thread to back of pool
int addToBack(threadPool *pool, Thread *new)
{
  if ((pool == NULL) || (new == NULL))
    return 0;
  new->next = NULL;

  //if queue is empty
  if (pool->numThreads == 0)
  {
    pool->front = new;
    pool->back = new;
  }
  else
  {
    pool->back->next = new;
    pool->back = new;
  }

  pool->numThreads++;
  return 1;
}

int addToFront(threadPool *pool, Thread *new)
{
  if ((pool == NULL) || (new == NULL))
    return 0;
  new->next = NULL;

  //if queue is empty
  if (pool->numThreads == 0)
  {
    pool->front = new;
    pool->back = new;
  }
  else
  {
    Thread *temp = (Thread*) malloc(sizeof(Thread));
    temp = pool->front;
    pool->front = new;
    pool->front->next = temp;
  }

  pool->numThreads++;
  return 1;
}

Thread *getFromPool(threadPool *pool)
{
  if (isEmpty(pool))
  {
    printf("Empty threadPool....\n");
    return NULL;
  }

  Thread *toRemove;

  toRemove = pool->front;
  pool->front = pool->front->next;
  pool->numThreads--;
  return toRemove;
}

Thread *getByID(threadPool *pool, int ID)
{
  if (isEmpty(pool))
  {
    printf("Empty threadPool....\n");
    return NULL;
  }

  Thread *toFind = pool->front;

  int i;

  for (i = 0; i < pool->numThreads; i++)
  {
    if (toFind == NULL)
      return NULL;
    else if (toFind->threadID == ID)
      return toFind;

    toFind = toFind->next;
  }

  return NULL;
}
