//Thread test lock_acquire
#include "uthread.h"
extern thread_p current_thread;
extern int donationFlag;
mutex_t myLock;
mutex_p myLock_p;
int count = 0;
void mythread(void){
	int i;
	for (i = 0; i < 5; i++) {
		lock_acquire(myLock_p);
		//Critical section
		int temp = count;
		printf(1, "Thread %s count read = %d\n", current_thread->name, count);
		thread_yield();
		temp = temp + 1;
		printf(1, "Incrementing count %d\n", temp);
		thread_yield();
		count = temp;
		thread_yield();
		printf(1, "Releasing Lock new Count = %d\n", count);
		lockRelease(myLock_p);
		thread_yield();
	}
	printf(1, "Thread: %s exit\n", current_thread->name);
}

int main(int argc, char *argv[]){
	myLock_p = &myLock;
	lock_init(myLock_p);
	thread_init();
	donationFlag = 0;
	thread_create("A", mythread, 1);
	thread_create("B", mythread, 1);
	thread_join();
	printf(1, "Retuned back to main\n");
	printf(1, "count = %d\n", count);
	exit();
}