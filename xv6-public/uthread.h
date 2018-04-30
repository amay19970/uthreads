#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

/* Possible states of a thread; */
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2
#define WAITING		0x3
#define WAITJOIN	0x4

#define NULL 0
#define CUT_AGE 10
#define MAX_PRIORITY 1000000000
#define	NAME_SIZE	50
#define STACK_SIZE  8192
#define MAX_THREAD  100

typedef struct thread thread_t, *thread_p;
typedef struct mutex mutex_t, *mutex_p;
int donationFlag;
struct thread {
	int        sp;                /* curent stack pointer */
	void (*fName)();
	char stack[STACK_SIZE];       /* the thread's stack */
	int        state;             /* running, runnable, waiting */
	char name[NAME_SIZE];
	int priority;
	int age;
	int originalPriority;
	mutex_p lock;
	int entryIndex;
};

struct mutex{
	uint val;
	thread_p thread;
};

extern void thread_switch(void);
void thread_yield(void);
void lock_init(mutex_p);
void priorityDonate(mutex_p);
void acquired(mutex_p);
void lock_acquire(mutex_p);
void lock_busy_wait_acquire(mutex_p);
void lockRelease(mutex_p);
void thread_init(void);
void thread_create(char*, void (*func)(), int);
void thread_yield(void);
void thread_join(void);
thread_p  current_thread, next_thread;