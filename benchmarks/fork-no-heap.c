#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


// The purpose of this file is to count the time that
// takes a process (and its children) to resume
// after being forked 


long get_time() {
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * 1e6 + currentTime.tv_usec;
}

int main() {
	//char heap_alloc[255*12];
	printf("benchmark: fork-no-heap\n");

	char heap_alloc[255*12]; // i show fill this memory
	printf("#INFO: allocated %ld in the stack\n", sizeof(char)*255*12);

	long pre_fork_timestamp = get_time();
	printf("pre_fork_timestamp = %ld;\n", pre_fork_timestamp);
	
	fflush(stdout);
	pid_t pid = fork();
	
	long post_fork_timestamp = get_time();
	long diff = post_fork_timestamp - pre_fork_timestamp;
	printf("pid = %d; post_fork_timestamp = %ld;\n", pid, post_fork_timestamp);
	printf("pid = %d; diff = %ld\n", pid, diff);
}
