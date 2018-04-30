//Thread test starvation
#include "uthread.h"
extern int donationFlag;
extern thread_p current_thread;
void heavyComputationFunction(void){
	int i;
	printf(1, "thread %s running\n", current_thread->name);
	for(i = 0; i < 100000; i++){
		printf(1, "my thread %s iteration %d\n", current_thread->name, i);
		thread_yield();
	}
	printf(1, "my thread: %s exit\n", current_thread->name);
}

void lightComputationFunction(void){
	int i = 1;
	thread_yield();
	printf(1, "my thread %s scheduled\n", current_thread->name);
	thread_yield();
	i = i + 1;
	thread_yield();
	printf(1, "my thread %s exits\n", current_thread->name);
	exit();
}

int main(int argc, char *argv[]){
	thread_init();
	thread_create("A", lightComputationFunction, 1);
	thread_create("B", heavyComputationFunction, 1000);
	thread_create("C", heavyComputationFunction, 1000);
	thread_join();
	printf(1, "Retuned back to main\n");
	exit();
}