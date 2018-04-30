//Basic Thread Implementation Test
#include "uthread.h"
extern thread_p current_thread;
void mythread(void){
	int i;
	printf(1, "thread %s running\n", current_thread->name);
	for (i = 0; i < 10; i++) {
		printf(1, "my thread %s 0x%x iteration %d\n", (int) current_thread->name, current_thread, i);
		thread_yield();
	}
	printf(1, "my thread: %s exit\n", current_thread->name);
}

int main(int argc, char *argv[]){
	thread_init();
	thread_create("C", mythread, 1);
	thread_create("D", mythread, 2);
	thread_join();
	printf(1, "Retuned to main\n");
	exit();
}