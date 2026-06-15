#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#include <sys/time.h>


// It seems like linux can't read more than 2147479552 bytes, 
// so I need to do it in multiple calls
// https://stackoverflow.com/questions/70368651/why-cant-linux-write-more-than-2147479552-bytes
void extended_read(int file_descriptor, void* target, size_t bytes_to_read) {
	size_t MAX_RW_COUNT = INT_MAX & sysconf(_SC_PAGESIZE);
	size_t count = 0;

	while (count < bytes_to_read) {
		size_t now_read = MAX_RW_COUNT;
		if (now_read > bytes_to_read-count) now_read = bytes_to_read - count;
		
		size_t bytes_read = read(file_descriptor, target, now_read);
		count = count + bytes_read;
	}
	printf("Successfully read %ldbytes into %p\n", count, target);
}

// From https://gist.github.com/w1k1n9cc/012be60361e73de86bee0bce51652aa7
// I'm concerned this way of getting precise time is inefficient but I'll
// use it for now
long get_time() {
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * 1e6 + currentTime.tv_usec;
} 

int main() {
	// Allocate a lot of memory and fill it with garbage 
	size_t amount_of_mem = sizeof(char)*1024*1024*1024*10;
	char* a_lot_of_mem = malloc(amount_of_mem);

	printf("SSIZE_MAX: %ld\namount_of_mem: %ld\n",  SSIZE_MAX, amount_of_mem);
	assert(amount_of_mem<SSIZE_MAX);
	
	int urandom_fd = open("/dev/urandom", O_RDONLY, NULL);
	assert(urandom_fd>=0);


	printf("The /dev/urandom file descriptor is %d\n", urandom_fd);
	printf("Trying to fill memory\n");
	extended_read(urandom_fd, a_lot_of_mem, amount_of_mem);
	

	long pre_fork_timestamp = get_time(); 

	// For the process
	pid_t pid = fork();

	long post_fork_timestamp = get_time();
	long timestamps_diff = post_fork_timestamp - pre_fork_timestamp;

	if (pid == 0) {
		printf("I'm the parent! I resumed after %ldusec\n", timestamps_diff);
	} else {
		printf("I'm the child process with pid %i. I resumed after%ldusec\n", pid, timestamps_diff);
	}
	
	int seconds = 10;
	printf("Sleeping for %dseconds\n", seconds);
	sleep(seconds);
	puts("hello gang");
	free(a_lot_of_mem);
}

