#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE_OF_MEMORY 200
#define MAX_LENGTH 10
#define MIN_LENGTH 2

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


// definition
struct node
{
	int id;
	int start;
	int hole;
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
			printf("Delete file at %d with size %d\n", s, size);
			if( ((tmp -> start) == s) && ((tmp -> end) == s + size) ) { // exactly match
				tmp -> hole = size;
				if(((tmp -> next) != NULL) && (((tmp -> next) -> hole) > 0) && (((tmp -> next) -> start) == (tmp -> end))) { // if the following block has hole
					(tmp -> hole) += (tmp -> next) -> hole;
					tmp -> next = (tmp -> next) -> next;
					tmp -> end = (tmp -> start) + (tmp -> hole);
				}
			}
			else {
				ptrtonode newBlock;
				newBlock = (ptrtonode)malloc(sizeof(nodetype));
				newBlock -> start = s;
				newBlock -> hole = size;
				newBlock -> end = s + size;

				if((tmp -> end) - s - size != 0) { // if the current block could be splitted and the firs half is full, the split into two
					ptrtonode newBlock2;
					newBlock2 = (ptrtonode)malloc(sizeof(nodetype));
					newBlock2 -> start = s + size;
					newBlock2 -> hole = (tmp -> end) - s - size;
					newBlock2 -> end = tmp -> end;

					tmp -> end = s;

					newBlock2 -> next = tmp -> next;
					tmp -> next = newBlock;
					newBlock -> next = newBlock2;
				}
				else {
					tmp -> end = s;
					newBlock -> next = tmp -> next;
					tmp -> next = newBlock;
				}
			}
			return;
		}
		tmp = tmp -> next;
	}
}

int bestFitAllocation(memory mem, int fileSize, int fileId)
{
	int diff, count = 0, c = -1, min;
	ptrtonode tmp;
	tmp = mem->next; // temp is the current memory block
	min = SIZE_OF_MEMORY + 1; // the minimum size of the block even found that could contain the file
	while(tmp!=NULL) // find the minimum block
	{
		if(tmp->hole >= fileSize)
			if(tmp->hole < min) // update min
			{
				min = tmp->hole;
				c = count; // record the block id
			}
		tmp=tmp->next;
		count++;
	}

	if(c == -1) return -1; // no place

	// update
	int startLocation = -1;
	tmp=mem->next;
	count=0;
	while(tmp!=NULL)
	{
		if(c==count)
		{
			tmp->id = fileId;
			diff = tmp->hole - fileSize;
			tmp->hole = 0;
			tmp->end = tmp->start + fileSize;
			startLocation = tmp -> start;
			printf("Insert file %d at %d with size %d\n", fileId, startLocation, fileSize);
			if(tmp->next!=NULL && tmp->next->hole==0) // if there is no following empty memory right after this block
			{
				// update the block following
				tmp = tmp->next;
				tmp->hole += diff;
				tmp->start -= diff;
			}
			else // if the following block is an empty block, create a new block and put it into the middle of the two
			{
				ptrtonode t;
				t=(ptrtonode)malloc(sizeof(nodetype));
				t->next=tmp->next;
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
	printf("###########################################\n");
	printf("The logical disk is: \n");
	ptrtonode tmp = mem -> next;
	while(tmp != NULL) {
		printf("the start is %d, the hols is %d, the end is %d\n", tmp->start, tmp->hole, tmp->end);
		tmp = tmp -> next;
	}
	printf("###########################################\n");
}

void printFile(char* str, int size) {
	int i = 0;

	for(i = 0;i < size;i++) {
			printf("%c", str[i]);
	}
	printf("\n");
}

void printFileTable(fileRecordPtr fileTable, int size) {
	printf("###########################################\n");
	printf("The file table is: \n");
	int i;
	for(i = 0;i < size;i++) {
		printf("%d: index is %d, size is %d, start location is %p, logical location (offset) is %d\n", i, fileTable[i].index, fileTable[i].size, fileTable[i].startLocation, fileTable[i].startLocation - diskStartPtr);
		if(fileTable[i].index >= 0) {
			printf("The file content is: ");
			printFile(fileTable[i].startLocation, fileTable[i].size);
		}
		else {
			printf("The file content before overriden is: ");
			int j = 0;

			for(j = 0;j < fileTable[i].size;j++) {
					printf("%c", '_');
			}
			printf("\n");
		}
	}
	printf("###########################################\n");
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
	memory startPtr = ((struct producer_parms *) parameters) -> logicalDiskStartPtr;
	char* diskStartPtr = ((struct producer_parms *) parameters) -> diskStartPtr;
	fileRecordPtr fileTable = ((struct producer_parms *) parameters) -> fileTable;

	srand((unsigned) time(NULL));
	int count = 0;
	while(1) {
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
	return count;
}

int main(void) {
	char* diskStartPtr = (char*) malloc(sizeof(char) * SIZE_OF_MEMORY); // starting address of the continus memory
	memory logicalDiskStartPtr = createLogicalDisk();
	initializeLogicalDisk(logicalDiskStartPtr, 0, 0 + SIZE_OF_MEMORY);
	fileRecordPtr fileTable = (fileRecordPtr)malloc(sizeof(struct fileRecord) * MAX_LENGTH);

	struct producer_parms producer_args;
	producer_args.logicalDiskStartPtr = logicalDiskStartPtr;
	producer_args.diskStartPtr = diskStartPtr;
	producer_args.fileTable = fileTable;

	pthread_t newThread;
	int* numOfFiles;
	pthread_create(&newThread, numOfFiles, &producer, &producer_args);
	pthread_join (newThread, NULL);

	printFileTable(fileTable, *numOfFiles);

	// free the momery
	free(diskStartPtr);
	free(logicalDiskStartPtr);
	free(fileTable);
}