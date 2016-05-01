#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SIZE_OF_MEMORY 40
#define MAX_LENGTH 10
#define MIN_LENGTH 2


int best_fit(int* bitMap, int fileSize)
{
	int diff, count=0, c = -1, min;
	ptrtonode tmp;
	tmp = mem->next; // temp是当前内存block
	min = 900; // 记录小当前找到的大于process size的最小block大小
	while(tmp!=NULL) // 从头到尾遍历，找到最小的
	{
		count++;
		if(tmp->hole >= fileSize)
			if(tmp->hole < min) // 更新min
			{
				min = tmp->hole;
				c = count; // 记录小block id
			}
		tmp=tmp->next;
	}

	if(c == -1) return -1; // no place

	// 再遍历
	int startLocation = -1;
	tmp=mem->next;
	count=0;
	while(tmp!=NULL)
	{
		count++;
		// 找到了目标block
		if(c==count)
		{
			tmp->id = fileId;
			diff = tmp->hole - fileSize;
			tmp->hole = 0;
			tmp->end = tmp->start + fileSize; // 更新找到的block。把前部分设为0。
			startLocation = tmp -> start;
			if(tmp->next!=NULL && tmp->next->hole==0) // 如果当前block不是最后一个且当前block后面的block已满，代表当前block前后都是满的（正好符合塞进的）
			{
				tmp = tmp->next;
				tmp->hole += diff;
				tmp->start -= diff;
			}
			else // 如果后面是空的，那么就新建一个。插入到当前block和后面一个的中间
			{
				ptrtonode t;
				t=(ptrtonode)malloc(sizeof(nodetype));
				t->next=tmp->next; // 将新建的加在当前的后面
				tmp->next=t;
				t->id=0;
				t->hole=diff; 
				t->start=tmp->end;
				t->end=t->start+t->hole;
			}
			break;
		}
		tmp=tmp->next;
	}

	return startLocation;
}

char* genRandomString(int minLength, int maxLength)
{
	int random, i;
	char* str;
	srand((unsigned) time(NULL));

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

void producer() {
	srand((unsigned) time(NULL));
	int bitMap[SIZE_OF_MEMORY] = {0};
	int count = 0;
	while(1) {
		int random, i;
		char* str;
		int length = (rand() % (MAX_LENGTH - MIN_LENGTH + 1)) + MIN_LENGTH;

		if ((str = (char*) malloc(length)) == NULL )
		{
			printf("Malloc failed!\n");
			return;
		}

		for (i = 0; i < length - 1; i++)
		{
			random = rand() % 128;
			str[i] = random;
		}
		str[length - 1] = '\0';

		int startLocation = best_fit(startPtr, length, count);
		if(startLocation == -1) {
			printf("no sufficient space\n");
			printf("the last length is %d\n", length);
			return;
		}
		else {
			printf("the start location is %d, and the length is %d\n", startLocation, length);
			count++;
		}
	}
}

int main(void) {
	char* startPtr = (char*) malloc(sizeof(char) * SIZE_OF_MEMORY); // starting address of the continus memory
	// char* bitMapStart = (char*) malloc(sizeof(char) * SIZE_OF_MEMORY);
	insert(mStartPtr, 0, 0 + SIZE_OF_MEMORY);
	producer();
}