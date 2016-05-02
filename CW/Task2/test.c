#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char* ptr = (char*) malloc(sizeof(char) * 2);
	char a1 = ptr[0];
	char a2 = ptr[1];
	printf("address is %d\n", ptr);
	printf("address of a1 is %p\n", ptr + 0);
	printf("address of a2 is %p\n", ptr + 1);
}