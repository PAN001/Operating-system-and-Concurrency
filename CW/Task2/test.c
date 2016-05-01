#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char* ptr = (char*) malloc(sizeof(char) * 2);
	printf("address is %p\n", ptr);
}