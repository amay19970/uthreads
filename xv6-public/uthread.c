#include "uthread.h"

int numThreads = 0;
int entryIndex = 0;
extern int donationFlag;
extern thread_p current_thread;
extern thread_p  next_thread;
thread_t all_thread[MAX_THREAD];
static void thread_schedule(void);

static void nameCpy(const char* name, thread_p t){
	int i = 0;
	char *dest = t->name;
	while((*dest++ = *name++) != 0 && i++ != NAME_SIZE);
	if(i == NAME_SIZE)
		t->name[NAME_SIZE - 1] = '\0';
}

void lock_init(mutex_p lockP){
	lockP->val = 0;
	lockP->thread = NULL;
}

void priorityDonate(mutex_p lockP){
	printf(1, "priority donated\n");
	lockP->thread->priority = current_thread->priority;
}

void acquired(mutex_p lockP){
	printf(1, "Thread %s acquired lock\n", current_thread->name);
	if(donationFlag)
		lockP->thread = current_thread;
	current_thread->lock = NULL;
}

void lock_acquire(mutex_p lockP){
	while(xchg(&lockP->val, 1) != 0){
		if(donationFlag)
			priorityDonate(lockP);
		current_thread->lock = lockP;
		current_thread->state = WAITING;
		thread_schedule();
	}
	acquired(lockP);
}

void lock_busy_wait_acquire(mutex_p lockP){
	while(xchg(&lockP->val, 1) != 0){
		printf(1, "Thread %s lock acquire failed\n", current_thread->name);
		if(donationFlag)
			priorityDonate(lockP);
		thread_yield();
	}
	printf(1, "Thread %s lock acquired\n", current_thread->name);
	acquired(lockP);
}

void lockRelease(mutex_p lockP){
	printf(1, "Thread %s released lock\n", current_thread->name);
	for(thread_p t = all_thread; t < all_thread + MAX_THREAD; t++)
		if(t->state == WAITING && t->lock == lockP)
			t->state = RUNNABLE;
	xchg(&lockP->val, 0);
	current_thread->priority = current_thread->originalPriority;
}

void thread_init(void){
	donationFlag = 1;
	numThreads += 1;
	entryIndex += 1;
	current_thread = all_thread;
	current_thread->priority = 1;
	current_thread->state = RUNNING;
	nameCpy("main", current_thread);
	current_thread->lock = NULL;
	for(thread_p t = all_thread + 1; t < all_thread + MAX_THREAD; t++)
		t->state = FREE;
}

static void thread_schedule(void){
	thread_p t;

	/* Find another runnable thread. */
	int maxPriority = - MAX_PRIORITY, entryIn = MAX_PRIORITY;
	int j = 0;
	t = all_thread;
	while(j != numThreads){
		if(t->state != FREE){
			if (t != current_thread && t->state == RUNNABLE && (t->priority > maxPriority || (t->priority == maxPriority && t->entryIndex < entryIn))){
				maxPriority = t->priority;
				entryIn = t->entryIndex;
				next_thread = t;
			}
			j++;
		}
		t++;
	}

	j = 0;
	t = all_thread;
	if(maxPriority == -MAX_PRIORITY)
		if (current_thread->state == FREE)
			next_thread = all_thread;
		else
			next_thread = current_thread;
	else
		while(j != numThreads){
			if(t->state != FREE){
				if (t->state == RUNNABLE && t != next_thread && t->priority < maxPriority){
					t->age++;
					if(t->age == CUT_AGE){
						t->priority++;
						t->age = 0;
					}
				}
				j++;
			}
			t++;
		}

	j = 0;
	t = all_thread;
	int i = 0;
	while(j != numThreads){
		i++;
		if(t->state != FREE){
			j++;
			printf(1, "%d %d\n", i, t->priority);
		}
		t++;
	}
	if (current_thread != next_thread) {         /* switch threads?  */
		next_thread->state = RUNNING;
		thread_switch();
	}
	else
		next_thread = 0;
}

static void thread_function(){
	current_thread->fName();
	current_thread->state = FREE;
	numThreads--;
	thread_schedule();
}

void thread_create(char* name, void (*func)(), int priority){
	thread_p t;
	int found = 0;

	for (t = all_thread; t < all_thread + MAX_THREAD && !found; t++)
		if (t->state == FREE)
			found = 1;
	t--;
	if (!found){
		printf(2, "Too many threads forked, max limit is %d\n", MAX_THREAD);
		exit();
	}

	numThreads++;
	entryIndex++;
	nameCpy(name, t);
	printf(1, "Thread %s forked\n", name);
	t->priority = priority;
	t->originalPriority = priority;
	t->lock = NULL;
	t->age = 0;
	t->entryIndex = entryIndex;
	t->fName = func;
	t->sp = (int) (t->stack + STACK_SIZE);// set sp to the top of the stack
	t->sp -= 4;                       // space for return address
	* (int *) (t->sp) = (int) thread_function;    // push return address on stack
	t->sp -= 32;                      // space for registers that thread_switch will push
	t->state = RUNNABLE;
	thread_yield();
}

void thread_yield(void){
	printf(1, "Thread %s Yielding...\n", current_thread->name);
	current_thread->state = RUNNABLE;
	thread_schedule();
}

void thread_join(void){
	if (current_thread != all_thread){
		printf(2, "Join must be called from main thread only\n");		
		exit();
	}
	current_thread->state = WAITJOIN;
	thread_schedule();
}
