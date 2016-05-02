#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>  

#define NUM_OF_ELEMENTS 21
#define SIZE_OF_MEMORY NUM_OF_ELEMENTS * sizeof(int)
#define SHARED_MEMORY_NAME "SHAREDMEMORY"

int main(int argc,char *argv[]) {
	int i, status;
	pid_t pid2;
	int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0666); // creates and opens a new, or opens an existing, POSIX shared memory object
	if(shm_fd == -1)
	{
		printf("failed to open shared memory\n");
		exit(1); 
	}
	if(ftruncate(shm_fd, SIZE_OF_MEMORY) == -1) { // truncate a file to a specified length
		printf("failed to set size of memory\n");
		exit(1);
	}

	// Map the shared memory object in to the processes’ logical address space using mmap, specifying the
	// address location at which to attach the memory, the size, the read/write protection, optional flags, 
	// the file descriptor for the shared object, and the offset (usually 0). 
	// Best practice is to specify NULL for the address location, thereby allowing Linux to decide itself 
	// where to attach the object into the logical address space, and returning the memory.
	int *i_ptr = mmap(NULL, SIZE_OF_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // map the file into memory

	// generate RandInt
	srand((unsigned)time(NULL)); 
	int RandInt = rand() % 20 + 1; 
	*i_ptr = RandInt;
	printf("The RandInt = %d, created by the parent process\n", *i_ptr);
	pid_t pid = fork();

	if(pid < 0) {
		printf("fork error\n");
	} 
	else if(pid == 0) { // first child process
		execlp("./ChildP1", "ChildP1", NULL);
		printf("failed to run ChildP1\n");
		exit(1);
	}
	else { // parent process
		waitpid(pid, &status, WUNTRACED);
		pid2 = fork();
		if(pid2 < 0) {
			printf("fork error\n");
			exit(1);
		} 
		else if(pid2 == 0) { // second child process
			execlp("./ChildP2", "ChildP2", NULL);
		}
		else { // parent process
			waitpid(pid, &status, WUNTRACED);
			printf("failed to run ChildP1\n");
			exit(1);
		}
	}


	waitpid(pid, &status, WUNTRACED);
	// printf("Child1 process has finished\n");
	waitpid(pid2, &status, WUNTRACED);
	// printf("Child2 process has finished\n");

	int j;
	int indexOfChildProcess = 1;
	for(j = 1;j < 21;j++) {
		if(j == 11) {
			indexOfChildProcess = 2;
		}
		printf("The RandInt = %d, created by the child process %d\n", *(i_ptr + j), indexOfChildProcess);
	}

	if (munmap(i_ptr, SIZE_OF_MEMORY) == -1) // 取消参数start所指的映射内存起始地址
		perror("Error un-mmapping the file");

	shm_unlink( SHARED_MEMORY_NAME ); // unlink POSIX shared memory objects
	shmctl(shm_fd, IPC_RMID, 0); // mark the segment to be destroyed
	return 0;
}