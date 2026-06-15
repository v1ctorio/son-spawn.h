#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

int main() {
	// Allocate a lot of memory and fill it with garbage 
	size_t amount_of_mem = sizeof(char)*1024*1024*1024*10;
	char* a_lot_of_mem = malloc(amount_of_mem);

	printf("SSIZE_MAX: %ld\namount_of_mem: %ld\n",  SSIZE_MAX, amount_of_mem);
	assert(amount_of_mem<SSIZE_MAX);
	
	int urandom_fd = open("/dev/urandom", O_RDONLY, NULL);
	assert(urandom_fd>=0);


	printf("The /dev/urandom file descriptor is %d\n", urandom_fd);



	ssize_t success = read(urandom_fd, a_lot_of_mem, amount_of_mem);
	assert(success>=0);

	if (success < amount_of_mem) {
		printf("Only read %ld out of %ld bytes from /dev/urandom", success, amount_of_mem);
	} else {
		printf("Successfully (%ld) filled %ld bytes of memory with garbage",success,amount_of_mem);
	}

	// For the process
	pid_t pid = fork();
	if (pid == 0) {
		printf("I'm the children!\n");
	} else {
		printf("I'm the child process with pid %i\n", pid);
	}
	
	int seconds = 10;
	printf("Sleeping for %dseconds", seconds);
	sleep(seconds);
	puts("hello gang");
	free(a_lot_of_mem);
}
