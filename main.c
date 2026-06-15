#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#include <time.h>


// It seems like linux can't read more than 2147479552 bytes, 
// so I need to do it in multiple calls
// https://stackoverflow.com/questions/70368651/why-cant-linux-write-more-than-2147479552-bytes
void fill_pointer_w_file(int file_descriptor, void* target, size_t bytes_to_read) {
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
	fill_pointer_w_file(urandom_fd, a_lot_of_mem, amount_of_mem);
	

	// For the process
	pid_t pid = fork();
	if (pid == 0) {
		printf("I'm the children!\n");
	} else {
		printf("I'm the child process with pid %i\n", pid);
	}
	
	int seconds = 10;
	printf("Sleeping for %dseconds\n", seconds);
	sleep(seconds);
	puts("hello gang");
	free(a_lot_of_mem);
}

