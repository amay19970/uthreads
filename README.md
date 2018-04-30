# uthreads
User Level Threads implemented in xv6
Project User Level threads library along with slightly smart scheduler.
Scheduler has features of priority inversion and priority donation.
Mutex thread locks are also implemented.
Test programs uthreadTest 1 through 6 are created to testify correctness of implementation.
To test simulation build kernel, make sure that simulation run on single CPU.
Run on emulator qemu using command: make CPUS=1 qemu
