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
//----Used for delayed tasks
void ContinueHandler(int Signal) {
//----Nothing to do
}

void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("Received SIGUSR1!\n");
    }
}


int main(int  argc, char *argv[]){
	/*key_t ShmKEY;
    int  ShmID;
    int NumberOfPages;
    int SharedMemoryKey;*/
    int i;
    int SharedMemoryKey;
    int NumberOfPages;
    int MMUPID;
    int SegmentID;
    page_table_pointer PageTable;
    int frame;
    int indeksFrame = 0;
    boolean found = false;
    //struct Memory *ShmPTR;

    if (argc != 3) {
        printf("Ketikkan: %s #1 #2 \n", argv[0]);
        exit(1);
    }
    
    SharedMemoryKey = getpid();
    NumberOfPages = atoi(argv[1]);
    SegmentID = shmget(SharedMemoryKey, NumberOfPages*sizeof(page_table_entry),IPC_CREAT | 0666);
    frame = atoi(argv[argc-1]);

    //ShmKEY = (key_t) getpid(); 
    
     //ShmID = shmget(ShmKEY, sizeof(page_table_entry), IPC_CREAT | 0666);
     if (SegmentID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     //printf("Server has received a shared memory of four integers...\n");
     PageTable = (page_table_pointer)shmat(SegmentID,NULL,0);
     /*Deklarasi untuk shmat() adalah void *shmat(int shmid,const void *shmaddr, int shmflg). 
     Parameter shmid disini merupakan ID dari shared memory.
     Parameter Shmaddr merupakan lokasi shared memory di main memory,
     jika NULL akan dicarikan lokasi pada memory yang ingin ditempati.
     Return value dari perintah ini adalah pointer ke lokasi shared memory.*/

     //PageTable = (page_table_pointer) shmat(ShmID, NULL, 0);
     if ((page_table_pointer) PageTable == NULL) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     //printf("Server has attached the shared memory...\n");
     printf("The shared memory key (PID) is %d\n",getpid());
     printf("Initialized page table:\n");

     for (i=0;i<NumberOfPages;i++){
        PageTable[i].Valid=0;
        PageTable[i].Frame=-1;
        PageTable[i].Dirty=0;
        PageTable[i].Requested=0;   
     }
     printf("Valid terinisialisasi\n");

     //SegmentID = shmget(SharedMemoryKey, NumberOfPages*sizeof(page_table_entry),IPC_CREAT | 0666);
   //  PageTable = (page_table_pointer)shmat(SegmentID,NULL,0);

    printf("Please start the client in another window...\n");
    PageTable[0].statusOS = NOT_READY;
    signal(SIGUSR1,my_handler);
    while(PageTable[0].statusOS!=FINISH){
        i=0;
        while(!found){
            if (i==NumberOfPages)
                i=0;
            if (PageTable[i].Requested!=0)
                found = true;
            else
                i++;
        }
        if(found){
            printf("Process %d has requested page %d\n", PageTable[i].Requested, i);
            PageTable[i].Requested=0;
            if (indeksFrame<=frame){
                printf("put it in free frame %d\n",indeksFrame);
                indeksFrame++;
            }
            printf("Unblock MMU\n");
            found=false;
        }

        sleep(1);
    }
    //return EXIT_SUCCESS;

    //if (signal(SIGUSR1, my_handler) == SIG_ERR)
      //  printf("\ncan't catch SIGUSR1\n");
    //signal(SIGUSR1,my_handler) ;


    //while()
     //signal(SIGUSR1,ContinueHandler) ;
//     for (i=0; i<NumberOfPages ; i++){
  //      PageTable[i].statusOS = FILLED;
    // }
     
         //while (PageTable[0].statusOS != TAKEN)
          //sleep(1);


     

     
    
     PageTable[i].Requested = 0;
     PageTable[i].Valid = 1;
     PageTable[i].Frame = 0;
     
     
    printf("Finish !\n");
	return 0;
}