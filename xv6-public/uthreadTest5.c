//Thread test priority Inversion donation Flag is set to 1 by default
#include "uthread.h"
extern int donationFlag;
extern thread_p current_thread;
mutex_t myLock;
mutex_p myLock_p;
int count = 0;
void heavyComputationFunction(void){
	int i;
	printf(1, "thread %s running\n", current_thread->name);
	thread_yield();
	for(i = 0; i < 1000000; i++){
		printf(1, "my thread %s iteration %d\n", current_thread->name, i);
	}
	printf(1, "my thread: %s exit\n", current_thread->name);
}

void lowPriorityProcess(void){
	printf(1, "Thread %s starts execution\n", current_thread->name);
	lock_acquire(myLock_p);
	thread_yield();
	int i = 0;
	while(i != 100){
		printf(1, "Thread %s Iteration %d\n", current_thread->name, i);
		i++;
	}
	lockRelease(myLock_p);
	thread_yield();
	printf(1, "Thread %s exits\n", current_thread->name);
}

void lightComputationFunction(void){
	int i = 1;
	printf(1, "my thread %s scheduled\n", current_thread->name);
	lock_acquire(myLock_p);
	i = i + 1;
	printf(1, "my thread %s exits\n", current_thread->name);
	lockRelease(myLock_p);
}

int main(int argc, char *argv[]){
	donationFlag = 0;
	myLock_p = &myLock;
	lock_init(myLock_p);
	thread_init();
	donationFlag = 0;
	thread_create("A", lowPriorityProcess, -100);
	thread_create("B", heavyComputationFunction, 10);
	thread_create("C", lightComputationFunction, 100000);
	thread_join();
	printf(1, "Retuned back to main\n");
	exit();
}