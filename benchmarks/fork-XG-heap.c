#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#include <sys/time.h>

#ifndef SSIZE_MAX
//this is so clangd stops complaining
#define SSIZE_MAX 0 
#endif

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
	printf("#INFO(extended_read): successfully read %ldbytes into %p\n", count, target);
}

// From https://gist.github.com/w1k1n9cc/012be60361e73de86bee0bce51652aa7
// I'm concerned this way of getting precise time is inefficient but I'll
// use it for now
long get_time() {
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * 1e6 + currentTime.tv_usec;
} 

int main(int _argc, char **argv) {
	int n_of_gb = atoi(argv[0]);
	if (n_of_gb == 0) {
		n_of_gb = 1;
		printf("#INFO: invalid or missing number of GB. Defaulted to 1\n");
	}

    printf("benchmark: fork-%dG-heap\n", n_of_gb);
	// Allocate a lot of memory and fill it with garbage 
	size_t amount_of_mem = sizeof(char)* 1e9 * n_of_gb;
	char* a_lot_of_mem = malloc(amount_of_mem);

	assert(amount_of_mem<SSIZE_MAX);
	
	int urandom_fd = open("/dev/urandom", O_RDONLY, NULL);
	assert(urandom_fd>=0);


	printf("#INFO: trying to fill memory\n");
	extended_read(urandom_fd, a_lot_of_mem, amount_of_mem);
	

	long pre_fork_timestamp = get_time(); 
	printf("pre_fork_timestamp = %ld;\n", pre_fork_timestamp);
	// For the process
	pid_t pid = fork();

	long post_fork_timestamp = get_time();
	printf("pid = %d; post_fork_timestamp = %ld;\n", pid, post_fork_timestamp);
	long timestamps_diff = post_fork_timestamp - pre_fork_timestamp;

	printf("pid = %d; post_fork_timestamp = %ld;\n", pid, post_fork_timestamp);
	printf("pid = %d; diff = %ld\n", pid, timestamps_diff);
	
	#define SECONDS 10
	printf("#info: sleeping for %dseconds\n", SECONDS);
	sleep(SECONDS);
	free(a_lot_of_mem);
}

