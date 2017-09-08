#include "mythreads.h"
#include "myqueue.h"
#include <assert.h>

int numCreatedThreads;
threadPool *pool;
threadPool *deadPool;
ucontext_t main_context;
int interruptsAreDisabled = 0;
int locks[NUM_LOCKS];
int conditionV[NUM_LOCKS][CONDITIONS_PER_LOCK];

//to make sure stacks get free'd properly
void cleanup (void) __attribute__((destructor));

void cleanup()
{
  destroyPool(deadPool);
  destroyPool(pool);
}

static void interruptDisable()
{
  assert(!interruptsAreDisabled);
  interruptsAreDisabled = 1;
}

static void interruptEnable()
{
  assert(interruptsAreDisabled);
  interruptsAreDisabled = 0;
}

//thread handler to get returns
void thread_exec(Thread *thread)
{
  void *retval = thread->funcPtr(thread->argPtr);

  if(retval != NULL)
    threadExit(retval);
  else
  {
    threadExit(NULL);
  }
}

void threadInit()
{
  //initialize data structs (namely the thread pool)
  int i, j;
  for (i = 0; i < NUM_LOCKS; i++)
    locks[i] = 0;

  for (i = 0; i < NUM_LOCKS; i++)
    for (j = 0; j < CONDITIONS_PER_LOCK; j++)
      conditionV[i][j] = 0;

  numCreatedThreads = 0;
  pool = buildPool();
  deadPool = buildPool();
  getcontext(&main_context);
  Thread *main_thread = buildThread(NULL, NULL, 0, &main_context);
  addToFront(pool, main_thread);

  return;
}

//each created thread needs unique ID (at least for the thread's life)
int threadCreate(thFuncPtr funcPtr, void *argPtr)
{
  ucontext_t *newcontext = (ucontext_t *)malloc(sizeof(ucontext_t));

  //save current context
  getcontext(newcontext);

  //allocate and initialize a new call stack
  newcontext->uc_stack.ss_sp = malloc(STACK_SIZE);
  newcontext->uc_stack.ss_size = STACK_SIZE;
  newcontext->uc_stack.ss_flags = 0;

  numCreatedThreads++;
  Thread *add = buildThread(funcPtr, argPtr, numCreatedThreads, newcontext);
  makecontext(add->context, (void (*) (void))thread_exec, 1, add);
  addToFront(pool, add);

  swapcontext(pool->front->next->context, add->context);

  return numCreatedThreads;
}

void threadYield()
{
  //get the front thread and move it to the back
  //update contexts of new threads with swapcontext
  Thread *yield = (Thread*) malloc(sizeof(Thread));

  yield = getFromPool(pool);

  //if only main, go to main
  if (yield == NULL)
    setcontext(&main_context);

  addToBack(pool, yield);
  swapcontext(pool->back->context, pool->front->context);

  return;
}

void threadJoin(int thread_id, void **result)
{
  //force calling function to wait until ID'd function is finished

  Thread *found = (Thread*) malloc(sizeof(Thread));

  found = getByID(pool, thread_id);

  //if the threadID is completely invalid
  if ((!isMember(deadPool, thread_id)) && (!isMember(pool, thread_id)))
    return;

  //if the threadID is dead but existed and had results, get them
  if ((isMember(deadPool, thread_id)))
  {
    found = getByID(deadPool, thread_id);
    if (result != NULL)
      *result = found->resultPtr;
    return;
  }

  found->join = pool->front->threadID;

  //yield until thread is finished
  while (!isMember(deadPool, thread_id))
  {
    threadYield();
  }

  found = getByID(deadPool, thread_id);

  if (result != NULL)
    *result = found->resultPtr;

  return;
}

void threadExit(void *result)
{
  Thread *del = (Thread*) malloc(sizeof(Thread));

  del = getFromPool(pool);

  //if the main thread, exit()
  if (del->threadID == 0)
  {
    exit(0);
  }
  //if joined thread, handle and return
  else if (del->join != -1)
  {
    del->resultPtr = result;
    addToBack(deadPool, del);

    Thread *temp = (Thread*) malloc(sizeof(Thread));
    Thread *temp2 = (Thread*) malloc(sizeof(Thread));
    temp = getByID(pool, del->join);

    while (pool->front->threadID != temp->threadID)
    {
      temp2 = getFromPool(pool);
      addToBack(pool, temp2);
    }

    setcontext(pool->front->context);
  }
  else
  {
    del->resultPtr = result;
    addToBack(deadPool, del);

    setcontext(pool->front->context);
  }
}

void threadLock(int lockNum)
{
  interruptDisable();

  while (locks[lockNum] == 1)
  {
    interruptEnable();
    threadYield();
    interruptDisable();
  }

  locks[lockNum] = 1;
  interruptEnable();

  return;
}

void threadUnlock(int lockNum)
{
  interruptDisable();

  locks[lockNum] = 0;

  interruptEnable();

  return;
}

void threadWait(int lockNum, int conditionNum)
{
  if (locks[lockNum] == 0)
  {
    printf("ERR: Attempting to wait on unlocked lock...\n");
    exit(0);
  }

  threadUnlock(lockNum);

  while (conditionV[lockNum][conditionNum] != 1)
    threadYield();

  conditionV[lockNum][conditionNum] = 0;
  threadLock(lockNum);

  return;
}

void threadSignal(int lockNum, int conditionNum)
{
  conditionV[lockNum][conditionNum] = 1;

  return;
}
