#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define NUM_OF_ELEMENTS 21
#define SHARED_MEMORY_NAME "SHAREDMEMORY"
#define SIZE_OF_MEMORY NUM_OF_ELEMENTS * sizeof(int)
#define NUM_OF_ITERATIONS 10
#define START_INDEX 11

int main(int argc,char *argv[]) {
	int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666); // get the file description of the shared memory
	if(shm_fd == -1)
	{
		printf("failed to open shared memory\n");
		exit(1); 
	}
	int *i_ptr = mmap(NULL, SIZE_OF_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // i_ptr指针所指向的内存即为共享内存
	int i;
	int result = *(i_ptr+START_INDEX-1);
	for(i = START_INDEX;i < START_INDEX + NUM_OF_ITERATIONS;i++) {
		result = result - 10;
		*(i_ptr+i) = result;
		// printf("result is %d\n", result);
	}
}
	

