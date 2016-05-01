#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE_OF_MEMORY 200
#define NUM_OF_FILES 20
#define NUM_OF_CONSUMERS 5
#define MAX_LENGTH 10
#define MIN_LENGTH 2

//
memset (str,'-',6);


// prototypes
struct node;
struct producer_parms;
struct fileRecord;

// memory block
typedef struct node *ptrtonode;
typedef ptrtonode memory;
typedef struct node nodetype;

// file record
typedef struct fileRecord* fileRecordPtr; 

memory createLogicalDisk();
void initializeLogicalDisk(memory mem,int s,int e);
void logicalDiskDeallocation(memory men, int s, int size);
int bestFitAllocation(memory mem, int fileSize, int fileId);
char* genRandomString(int minLength, int maxLength);
void printBlocks(memory mem);
void printFiles(char* str);
void printFileTable(fileRecordPtr fileTable, int size);
void* producer(void* parameters);


// global variable declarations
char* diskStartPtr = (char*) malloc(sizeof(char) * SIZE_OF_MEMORY); // starting address of the continus memory, which is used to emaluate the disk
memory logicalDiskStartPt = NULL; // logical disk memory representation
fileRecordPtr fileTable = (fileRecordPtr) malloc(sizeof(struct fileRecord) * MAX_LENGTH); // file table for existent files

mutex buffer_mutex; // similar to "semaphore buffer_mutex = 1", but different (see notes below)
semaphore fillCount = 0;
semaphore emptyCount = 1; // 1 means there is enough space for inserting the file

// definition
struct node
{
	int id;
	int start;
	int hole; // 空余内存块大小
	int end;
	ptrtonode next;
};

struct fileRecord
{
	int index;
	int size;
	int startLocation;
};

struct producer_parms
{
	memory logicalDiskStartPtr;
	char* diskStartPtr;
	fileRecordPtr fileTable;
};

// create a logical disk which is used for memory allocation and deallocation
memory createLogicalDisk()
{
	memory mem;
	mem=(ptrtonode)malloc(sizeof(nodetype));
	mem->next=NULL;
	return mem;
}

// inset file into emulated disk
void initializeLogicalDisk(memory mem,int s,int e)
{
	ptrtonode tmp;
	tmp=(ptrtonode)malloc(sizeof(nodetype));
	tmp->next=mem->next;
	tmp->start=s;
	tmp->end=e;
	tmp->hole=e-s;
	tmp->id=0;
	mem->next=tmp;
}

void logicalDiskDeallocation(memory men, int s, int size)
{
	ptrtonode tmp = men->next;
	while(tmp!= NULL) {
		if((tmp -> start) <= s && (s + size) <= (tmp -> end)) {

			ptrtonode newBlock;
			newBlock = (ptrtonode)malloc(sizeof(nodetype));
			newBlock -> start = s;
			newBlock -> hole = size;
			newBlock -> end = s + size;

			ptrtonode newBlock2;
			newBlock2 = (ptrtonode)malloc(sizeof(nodetype));
			newBlock2 -> start = s + size;
			newBlock2 -> hole = (tmp -> end) - s - size;
			newBlock2 -> end = tmp -> end;

			tmp -> end = s;

			newBlock2 -> next = tmp -> next;
			tmp -> next = newBlock;
			newBlock -> next = newBlock2;
			return;
		}
		tmp = tmp -> next;
	}
}

int bestFitAllocation(memory mem, int fileSize, int fileId)
{
	int diff, count = 0, c = -1, min;
	ptrtonode tmp;
	tmp = mem->next; // temp是当前内存block
	min = 300; // 记录小当前找到的大于process size的最小block大小
	while(tmp!=NULL) // 从头到尾遍历，找到最小的
	{
		if(tmp->hole >= fileSize)
			if(tmp->hole < min) // 更新min
			{
				min = tmp->hole;
				c = count; // 记录小block id
			}
		tmp=tmp->next;
		count++;
	}

	if(c == -1) return -1; // no place

	// 再遍历
	int startLocation = -1;
	tmp=mem->next;
	count=0;
	while(tmp!=NULL)
	{
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
		count++;
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

void printBlocks(memory mem) {
	ptrtonode tmp = mem -> next;
	while(tmp != NULL) {
		printf("the start is %d, the hols is %d, the end is %d\n", tmp->start, tmp->hole, tmp->end);
		tmp = tmp -> next;
	}
}

void printFiles(char* str) {
	int i = 0;
	printf("\nThe result is\n");
	for(i = 0;i < SIZE_OF_MEMORY;i++) {
		if(str[i] != '\0') {
			printf("%c", str[i]);
		}
		else {
			printf("\n");
		}
	}
}

void printFileTable(fileRecordPtr fileTable, int size) {
	int i;
	for(i = 0;i < size;i++) {
		printf("index is %d, size is %d, start location is %d\n", fileTable[i].index, fileTable[i].size, fileTable[i].startLocation);
	}
}

void test(memory startPtr) {
	srand((unsigned) time(NULL));
	int count = 0;
	// while(1) {
	// 	int random, i;
	// 	char* str;
	// 	int length = (rand() % (MAX_LENGTH - MIN_LENGTH + 1)) + MIN_LENGTH;

	// 	if ((str = (char*) malloc(length)) == NULL )
	// 	{
	// 		printf("Malloc failed!\n");
	// 		return;
	// 	}

	// 	for (i = 0; i < length - 1; i++)
	// 	{
	// 		random = rand() % 128;
	// 		str[i] = random;
	// 	}
	// 	str[length - 1] = '\0';

	// 	int startLocation = best_fit(startPtr, length, count);
	// 	if(startLocation == -1) {
	// 		printf("no sufficient space\n");
	// 		printf("the last length is %d\n", length);
	// 		return;
	// 	}
	// 	else {
	// 		printf("the start location is %d, and the length is %d\n", startLocation, length);
	// 		count++;
	// 	}
	// }
	int length = 10;
	int startLocation = bestFitAllocation(startPtr, length, 0);
	if(startLocation == -1) {
		printf("no sufficient space\n");
		printf("the last length is %d\n", length);
		return;
	}
	printf("the start location is %d, and the length is %d\n", startLocation, length);

	length = 20;
	startLocation = bestFitAllocation(startPtr, length, 1);
	if(startLocation == -1) {
		printf("no sufficient space\n");
		printf("the last length is %d\n", length);
		return;
	}
	// printf("the start location is %d, and the length is %d\n", startLocation, length);


	length = 5;
	startLocation = bestFitAllocation(startPtr, length, 2);
	if(startLocation == -1) {
		printf("no sufficient space\n");
		printf("the last length is %d\n", length);
		return;
	}
	// printf("the start location is %d, and the length is %d\n", startLocation, length);

	length = 5;
	startLocation = bestFitAllocation(startPtr, length, 3);
	if(startLocation == -1) {
		printf("no sufficient space\n");
		printf("the last length is %d\n", length);
		return;
	}
	// printf("the start location is %d, and the length is %d\n", startLocation, length);
	printBlocks(startPtr);
	logicalDiskDeallocation(startPtr, 10, 15);
	printf("\nafter deleting\n");
	printBlocks(startPtr);

	logicalDiskDeallocation(startPtr, 32, 3);
	printf("\nafter 2nd deleting\n");
	printBlocks(startPtr);

	bestFitAllocation(startPtr, 2, 4);
	printf("\nafter 2nd inserting\n");
	printBlocks(startPtr);

	bestFitAllocation(startPtr, 1, 5);
	printf("\nafter 3rd inserting\n");
	printBlocks(startPtr);
}

void* producer(void* parameters) {
	srand((unsigned) time(NULL));
	int count = 0;
	while(1) {
		// generate a random file
		int random, i;
		char* str;
		int length = (rand() % (MAX_LENGTH - MIN_LENGTH + 1)) + MIN_LENGTH;

		if ((str = (char*) malloc(length)) == NULL )
		{
			printf("Malloc failed!\n");
			break;;
		}

		for (i = 0; i < length - 1; i++)
		{
			random = rand() % 128;
			str[i] = random;
		}
		str[length - 1] = '\0';
		// 生成新文件结束
		

		int startLocation = bestFitAllocation(startPtr, length, count);
		if(startLocation == -1) {
			printf("no sufficient space\n");
			printf("the last length is %d\n", length);
			break;
		}
		else {
			printf("the start location is %d, and the length is %d, the random string is %s\n", startLocation, length, str);

			// put into disk
			int i;
			for(i = 0;i < length;i++) {
				*(diskStartPtr + startLocation + i) = str[i];
			}

			// update the file table
			int index = count;
			int size = length;
			int realStartLocation = diskStartPtr + startLocation;
			struct fileRecord newFileRecord;
			newFileRecord.index = index;
			newFileRecord.size = size;
			newFileRecord.startLocation = realStartLocation;
			fileTable[count] = newFileRecord;

			count++;
		}
	}
	printBlocks(startPtr);
	printFiles(diskStartPtr);
	printFileTable(fileTable, count);
	return NULL;
}

int main(void) {
	// initialize
	logicalDiskStartPtr = createLogicalDisk();
	initializeLogicalDisk(logicalDiskStartPtr, 0, 0 + SIZE_OF_MEMORY);
	int i;
	for(i = 0;i < SIZE_OF_MEMORY;i++) {
		fileTable[]
	}


	pthread_t producerId;	//存储生产者线程ID   
    pthread_t consumerIds[NUM_OF_CONSUMERS];	//存储消费者者线程ID   
	
	// initialize semaphors



    // create producer thread
	if(pthread_create(&producerId, NULL, &producer, NULL) != 0) {
		printf("producer thread created failed\n");
		exit(0)
	}

	// create consumer thread
	for(i = 0;i < NUM_OF_CONSUMERS;i++) {
		if(pthread_create(&consumerIds[i], NULL, &consumer, NULL) != 0) {
			printf("consumer %d thread created failed\n", i);
			exit(0)
		}
	}

	// wait for threads to terminate
	pthread_join (producerId, NULL);

	for(i = 0;i < NUM_OF_CONSUMERS;i++) {
		pthread_join (consumerIds[i], NULL);
	}
}