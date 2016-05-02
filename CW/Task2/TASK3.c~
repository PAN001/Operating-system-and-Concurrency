#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define SIZE_OF_MEMORY 200
#define NUM_OF_FILES 8
#define NUM_OF_CONSUMERS 3
#define MAX_LENGTH 10
#define MIN_LENGTH 2

// prototypes
// struct node;
// struct producer_parms;
// struct fileRecord;

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
void printFile(char* str, int size);
void printFileTable(fileRecordPtr fileTable, int size);
void* producer(void* parameters);
void* consumer(void* parameters);



// global variable declarations
char* diskStartPtr = NULL; // starting address of the continus memory, which is used to emaluate the disk
memory logicalDiskStartPtr = NULL; // logical disk memory representation
fileRecordPtr fileTable = NULL;

// mutex diskMutex; // similar to "semaphore buffer_mutex = 1", but different (see notes below)
sem_t numOfFilesLeft;
pthread_mutex_t mutex;  
int numOfFilesEver = 0;
// sem_t ; // 1 means there is enough space for inserting the file



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
	char* startLocation;
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

void* producer(void* parameters) {
	int tick = 0; // mark the number of loops
	while(numOfFilesEver < NUM_OF_FILES) {
    	// generate a random file
		int random, i;
		char* str;
		int length = (rand() % (MAX_LENGTH - MIN_LENGTH + 1)) + MIN_LENGTH;

		if ((str = (char*) malloc(length)) == NULL ) {
			printf("Malloc failed!\n");
			exit(0);
		}

		for (i = 0; i < length - 1; i++) {
			random = rand() % 128;
			str[i] = random;
		}
		str[length - 1] = '\0';


    	pthread_mutex_lock(&mutex); // wait until no other thread uses the disk
    	// Here, no other threads could access the disk

    	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    	printf("The %d producer starts\n", tick);
		int startLocation = bestFitAllocation(logicalDiskStartPtr, length, numOfFilesEver);
		if(startLocation == -1) {
			printf("no sufficient space\n");
		}
		else {
			// put into disk
			int i;
			for(i = 0;i < length;i++) {
				*(diskStartPtr + startLocation + i) = str[i];
			}

			// update the file table
			int index = numOfFilesEver;
			int size = length;
			int realStartLocation = diskStartPtr + startLocation;
			struct fileRecord newFileRecord;
			newFileRecord.index = index;
			newFileRecord.size = size;
			newFileRecord.startLocation = realStartLocation;
			fileTable[numOfFilesEver] = newFileRecord;

			numOfFilesEver++;
			sem_post(&numOfFilesLeft);
			// numOfFilesLeft++;
		}

		printBlocks(logicalDiskStartPtr);
		// printFiles(diskStartPtr);
		printFileTable(fileTable, numOfFilesEver);
		printf("The %d producer ends\n", tick);
		printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		printf("\n\n\n");
		tick++;
		/* Release the buffer */

		// sleep(3);
		pthread_mutex_unlock(&mutex);

		if (tick % 2 == 1) sleep(1); // interleave
	}
	printf("producer has already generated %d files\n", NUM_OF_FILES);
	exit(1);
	// return NULL;
}

void* consumer(void* parameters) {
	int id = (int)parameters;
	int random;
	int tick = 0;;
	while(1) {
		sem_wait(&numOfFilesLeft); // wait until there are files left
		// Here, there are enough files to delete

		pthread_mutex_lock(&mutex); // wait until no other thread uses the disk
		// Here, no other threads could access the disk

		random = rand() % numOfFilesEver;
		printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		printf("The %d ticks of the consumer %d starts\n", tick, id);
		if(fileTable[random].index >= 0) { // there is this file
			int size = fileTable[random].size;
			char* startPtr = fileTable[random].startLocation;
			int offset = startPtr - diskStartPtr;
			logicalDiskDeallocation(logicalDiskStartPtr, offset, size); // remove the file from logical disk

			// memset (startPtr, '-', sizeof(char) * offset)

			// delete the file from the disk
			int j;
			for(j = 0;j < size;j++) {
				*(startPtr + j) = '_';
			}

			fileTable[random].index = -1; // set the index of the corresponding file to -1
			// numOfFilesLeft--;
			sem_post(&numOfFilesLeft); // restore the deleted number

			printBlocks(logicalDiskStartPtr);
			// printFiles(diskStartPtr);
			printFileTable(fileTable, numOfFilesEver);
		}
		else {
			printf("the current file %d is already deleted\n", random);
		}
    	printf("The %d ticks of the consumer %d ends\n", tick, id);
		printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		printf("\n\n\n");
    	tick++;
		/* Release the buffer */
		pthread_mutex_unlock(&mutex);

		if (tick % 2 == 1) sleep(1); // interleave
	}
}

int main(void) {
	// initialize
	diskStartPtr = (char*) malloc(sizeof(char) * SIZE_OF_MEMORY);
	fileTable = (fileRecordPtr) malloc(sizeof(struct fileRecord) * NUM_OF_FILES); // file table for existent files

	srand((unsigned) time(NULL));
	logicalDiskStartPtr = createLogicalDisk();
	initializeLogicalDisk(logicalDiskStartPtr, 0, 0 + SIZE_OF_MEMORY);
	
	sem_init(&numOfFilesLeft, 0, 0); // at the beginning, there are no files
	pthread_mutex_init(&mutex, NULL);

	int i;
	for(i = 0;i < NUM_OF_FILES;i++) {
		fileTable[i].index = -2; // indicate that the file has not been initialized yet
		fileTable[i].size = -1;
		fileTable[i].startLocation = NULL;
	}


	pthread_t producerId;	//存储生产者线程ID   
    pthread_t consumerIds[NUM_OF_CONSUMERS];	//存储消费者者线程ID   
	
	// initialize semaphors



    // create producer thread
	if(pthread_create(&producerId, NULL, &producer, NULL) != 0) {
		printf("producer thread created failed\n");
		exit(0);
	}

	// create consumer thread
	for(i = 0;i < NUM_OF_CONSUMERS;i++) {
		if(pthread_create(&consumerIds[i], NULL, &consumer, i) != 0) {
			printf("consumer %d thread created failed\n", i);
			exit(0);
		}
	}

	// wait for threads to terminate
	pthread_join (producerId, NULL);

	for(i = 0;i < NUM_OF_CONSUMERS;i++) {
		pthread_join (consumerIds[i], NULL);
	}

	// free the momery
	free(diskStartPtr);
	free(fileTable);
}
