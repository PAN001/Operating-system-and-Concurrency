#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h>


int main(int argc,char *argv[]) {
	// int *i_ptr;
	// int x = 3;
	// i_ptr = &x;
	// *(i_ptr+1) = 4;
	// printf("x is %d\n", x);
	// // printf("i_ptr + 1 is %d\n", *(i_ptr + 1));

	// int x2[2] = {1,2};
	// int *i_ptr2 = x2;
	// printf("i_ptr2 + 1 is %d\n", *(i_ptr2 + 1));
	execlp("./ChildP1", "ChildP1", NULL);
	printf("error here\n");

}