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

static int reqAvailable = 0;
static int isLoop = 1;

//----Used for delayed tasks
void ContinueHandler(int Signal) {
    //----Nothing to do
}

//----Used for receive SIGUSR2 signal
void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        reqAvailable = 1;
    }
}

void my_handler_SIGCONT(int signum)
{
    if (signum == SIGCONT)
    {
        isLoop = 0;
    }
}

void writeDiskAccesses(int num){
    printf("%d disk acess",num);
    if(num>1)
    {    
        printf("es");
    }
    printf(" required\n");
}

void PrintPageTable(page_table_entry PageTable[],int NumberOfPages) {
    int Index;
    for (Index =  0;Index < NumberOfPages;Index++) 
    {
        printf("%2d: Valid=%1d Frame=%2d Dirty=%1d Requested=%1d\n",Index,
        PageTable[Index].Valid,PageTable[Index].Frame,PageTable[Index].Dirty,
        PageTable[Index].Requested);
    }
}

void printUsage(char* argv[]){
    printf("Usage : %s <nP> <nF> \n", argv[0]);
    printf("<nP> = Number of Page\n");
    printf("<nF> = Number of Frame\n");
}

int main(int argc, char *argv[]){
    int i, j, indeksLRU = 0;
    int minimum;
    int victimsFrame;
    int SharedMemoryKey;
    int NumberOfPages;
    int MMUPID;
    int SegmentID;
    page_table_pointer PageTable;
    int nFrame;
    int indeksFrame = 0;
    boolean found = false, check;
    int numberOfDiskAccess=0;


    // validate input
    if (argc != 3) {
        printUsage(argv);
        exit(1);
    }
    
    // initialize variable
    SharedMemoryKey = getpid();
    NumberOfPages = atoi(argv[1]);
    SegmentID = shmget(SharedMemoryKey, NumberOfPages*sizeof(page_table_entry),IPC_CREAT | 0666);
    nFrame = atoi(argv[argc-1]); // banyak frame

    // validate segmentID
    if (SegmentID < 0) {
        perror("ERROR: Could not get page table (OS)");
        exit(EXIT_FAILURE);
    }
    
    PageTable = (page_table_pointer)shmat(SegmentID,NULL,0);

    if ((page_table_pointer) PageTable == NULL) {
        printf("ERROR: Could not initialize continue handler (OS)\n");
        exit(1);
    }

    printf("The shared memory key (PID) is %d\n",getpid());
    
    printf("Initialize page table :\n");
    for (i=0;i<NumberOfPages;i++){
        PageTable[i].LRU=-1;
        PageTable[i].Valid=0;
        PageTable[i].Frame=-1;
        PageTable[i].Dirty=0;
        PageTable[i].Requested=0;
    }

    printf("Please start the client in another window...\n\n");
    
    signal(SIGUSR1,my_handler);
    signal(SIGCONT, my_handler_SIGCONT);
    while(isLoop){
        while(!reqAvailable){
            sleep(1);
        }

        while(!found && isLoop){
            if (i>NumberOfPages)
                i=0;
            if (PageTable[i].Requested!=0)
                found = true;
            else
                i++;
        }

        if(found){
            MMUPID = PageTable[i].Requested;
            printf("Process %d has requested page %d\n", PageTable[i].Requested, i);
            PageTable[i].Requested=0;
            j=0;
            check=false;
            // to check the used of frame
            if (indeksFrame==nFrame){
                check=true;
            }
            
            if (!check){
                PageTable[i].Valid = 1;
                PageTable[i].Frame = indeksFrame;
                PageTable[i].LRU = indeksLRU;
                minimum = i;
                printf("put it in free frame %d\n",indeksFrame);
                kill(MMUPID,SIGUSR2);
                indeksFrame++;
            }else{
                j=0;
                // this is the algoritma of LRU
                while(j<NumberOfPages){
                    if (PageTable[j].Frame>-1)
                        minimum=j;
                    j++;
                }
                j=0;
                while(j<NumberOfPages){
                    if (PageTable[j].LRU>-1 && PageTable[j].Frame>-1)
                    {
                        if (PageTable[j].LRU < PageTable[minimum].LRU)
                            minimum=j;
                    }
                    j++;
                }
                // end algoritma of LRU
                printf("Choose a victim page %d\n", minimum);
                
                if (PageTable[minimum].Dirty==1){
                    printf("Victim is dirty, write out\n");
                    sleep(1);
                    PageTable[minimum].Dirty=0;
                    numberOfDiskAccess++;
                }
                PageTable[minimum].Valid=0;
                victimsFrame = PageTable[minimum].Frame;
                PageTable[minimum].Frame=-1;
                printf("Put in victim's frame %d\n", victimsFrame);
                PageTable[i].Valid=1;
                PageTable[i].Frame = victimsFrame;
                PageTable[i].LRU = indeksLRU;
                kill(MMUPID,SIGUSR2);
            }
            reqAvailable=0;
            indeksLRU++;
            found=false;
            numberOfDiskAccess++;
            printf("Unblock MMU\n");
        }
        sleep(1);
        printf("----------------------------------------------------\n");
    }
    
    signal(SIGUSR1,my_handler);
    while(!reqAvailable){
        sleep(1);
    }
    reqAvailable=0;

    printf("The MMU has finished !\n");
    PrintPageTable(PageTable,NumberOfPages);

    writeDiskAccesses(numberOfDiskAccess);

    //----Free the shared memory
    if (shmdt(PageTable) == -1) {
        perror("ERROR: Error detaching segment");
        exit(EXIT_FAILURE);
    }
	return 0;
}