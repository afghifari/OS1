#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "PageTable.h"
/*typedef struct {
    int Valid;
    int Frame;
    int Dirty;
    int Requested;
    } page_table_entry;

typedef page_table_entry* page_table_pointer;
*/

int main(int  argc, char *argv[]){
	page_table_pointer PageTable;
	key_t ShmKEY;
    int  ShmID;
    int NumberOfPages;
    int SharedMemoryKey;
    //struct Memory *ShmPTR;

     if (argc != 3) {
          printf("Ketikkan: %s #1 #2 \n", argv[0]);
          exit(1);
     }
     
    ShmKEY = ftok(".", 'x'); 
    SharedMemoryKey = atoi(argv[argc-1]);
	NumberOfPages = atoi(argv[1]);
     //ShmID = shmget(ShmKEY, sizeof(page_table_entry), IPC_CREAT | 0666);
     ShmID = shmget(SharedMemoryKey, NumberOfPages*sizeof(page_table_entry),IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     //printf("Server has received a shared memory of four integers...\n");
     
     PageTable = (page_table_pointer) shmat(ShmID, NULL, 0);
     if ((int) PageTable == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     //printf("Server has attached the shared memory...\n");
     printf("The shared memory key (PID) is %d\n",getpid());
     printf("Initialized page table:\n");

     PageTable[0].statusOS = NOT_READY;
     PageTable[0].Valid=0;
     PageTable[0].Frame=1;
     PageTable[0].Dirty=0;
     PageTable[0].Requested=0;

     PageTable[1].statusOS = NOT_READY;
     PageTable[1].Valid=0;
     PageTable[1].Frame=1;
     PageTable[1].Dirty=0;
     PageTable[1].Requested=0;

     PageTable[2].statusOS = NOT_READY;
     PageTable[2].Valid=0;
     PageTable[2].Frame=1;
     PageTable[2].Dirty=0;
     PageTable[2].Requested=0;

     printf("Valid terinisialisasi\n");
     PageTable[0].statusOS = FILLED;
     PageTable[1].statusOS = FILLED;
     PageTable[2].statusOS = FILLED;
     printf("Please start the client in another window...\n");

     while (PageTable[0].statusOS != TAKEN)
          sleep(1);
      printf("Finish !\n");
	return 0;
}