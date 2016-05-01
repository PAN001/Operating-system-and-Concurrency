//产生长度为length的随机字符串
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

char* genRandomString(int minLength, int maxLength)
{
	int random, i;
	char* str;
	srand((unsigned) time(NULL));
	// for(int i = 0;i< 1000000;i++) {
	// 	int length = (rand() % (maxLength - minLength + 1)) + minLength;
	// 	if(length < minLength || length > maxLength) {
	// 		printf("error\n");
	// 		return NULL;
	// 	}
	// 	printf("%d\n", i);
	// }
	int length = (rand() % (maxLength - minLength + 1)) + minLength;

	if ((str = (char*) malloc(length)) == NULL )
	{
		printf("Malloc failed!\n");
		return NULL ;
	}

	for (i = 0; i < length - 1; i++)
	{
		random = rand() % 128;
		str[i] = random;
	}
	str[length - 1] = '\0';
	return str;
}

int main(void) {
	printf("%s\n", genRandomString(2, 10));
}