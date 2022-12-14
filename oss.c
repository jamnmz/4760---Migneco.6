/*
 *Jared Migneco
 *Project 6 OSS Class
 *Hauschild
 *Due: 12/13/22
 *
 *
 *oss class
 *
 */
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>


//shared memory
#define SHMKEY  960158     /* Parent and child agree on common key.*/
#define BUFF_SZ sizeof ( int )

//message queue struct from https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_message_queues.htm
#define PERMS 0644
struct my_msgbuf {
   long mtype;
   char mtext[200];
};

//resource data structure
struct frame_table
{
        char name[10];
        int desc[256];

};
struct page_table
{
        char name[10];
        int data[256];
};


//using form provided OS_tutorial
void print_usage ( const char * app )
{
        fprintf (stderr, "usage: %s [-h] \n",app);
        fprintf (stderr, "    Calling with no arguments starts program\n");
}


//main body
int main(int argc, char** argv)
{

        int f;
        int g;

        //message queue
        //https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_message_queues.htm
        struct my_msgbuf buf;
        int msqid;
        int len;
        key_t key;
        system("touch msgq.txt");

        //variables used in framelocation math
        int q;
        int w;
        int s;
        int x;

        //clock variable
        int clockSec = 0;
        int clockNano = 0;
        int clockRand = 0;
        int clockRandNano = 0;
        int clockNeeded;

        //variable for process time
        int procLen = 0;
        int timeNeeded[54];
        int timeRemain;

        //originally thought program needed to handle user input and reread task
        int c = argc;
        int opt;


        //help statement found by calling with an argument
        switch(c)
        {
                case 1:
                        break;
                case 2:
                        print_usage ( argv[0] );
                        return ( EXIT_SUCCESS );
                default: /* '?' */
                        printf ( "Invalid option %c\n", optopt );
                        print_usage ( argv[0] );
                        return ( EXIT_FAILURE );

        }


        //CREATION OF SHARED MEMORY
        int shmid1 = shmget ( SHMKEY, BUFF_SZ, 9045 | IPC_CREAT );
        int shmid2 = shmget ( SHMKEY, BUFF_SZ, 9046 | IPC_CREAT );

        char *secClock = (char*) shmat(shmid1,NULL, 0);
        char *nanoClock = (char*) shmat(shmid2,NULL, 0);

        int *sharedClockSec = (int*) (secClock);
        int *sharedClockNano = (int*) (nanoClock);

        //arrays for frams and page tables in shared memory
        char *status = (char*) shmat(shmid1,NULL, 0);
        char *frameLoc = (char*) shmat(shmid2,NULL, 0);

        int *sharedStatus = (int*)(status);
        int *sharedFrameLoc = (int*)(frameLoc);

        int frameArr[256];
        int pageArr[32];

        int i;
        //overflow value used to adjust past 18 in the timeNeeded array
        int j = 0;

        int procs;

        //argument array for user_proc, cannot figure out how to convert m value into a string so just passing default m value
        static char *workerArr[] = {"./user_proc", NULL, NULL};
        workerArr[1] = "1";


        //keeps number of active processes below 18
        for(procs = 0; procs < 18; procs++)
        {
                //checks if clock has reached a second yet
                if(clockNano >= 1000000000)
                {
                        clockNano -= 1000000000;
                        clockSec += 1;
                }

                //forks and increments to active process counter
                pid_t childPid = fork();
                procs++;

                //randomly determines the amount of time processes get to run
                clockRand = rand() % 1000;
                clockRandNano = clockRand * 10000;


                wait(1);

                //periodically displays the frame table
                if(clockRandNano > 9000000)
                {
                        for(w = 0; w < 256; w++)
                        {

                                printf("Frame: %d: \t", w);
                                if (frameArr[w] > 0)
                                {
                                        printf("YES\n");
                                }
                                else
                                {
                                        printf("NO\n");
                                }
                        }
                }



                //I may be struggling to keep up, but this is all that keeps me going at this point
                //insert comedy.txt
                if (childPid < 0)
                {
                        perror("ERROR: something got a little forked up\n");
                        exit(EXIT_FAILURE);
                }
                else if (childPid == 0)
                {
                        //increments nanocounter and relays what process is running for how long
                        clockNano = clockNano + clockRandNano;
                        printf("OSS: Process %d requested at", childPid);
                        printf(" %d:", clockSec);
                        printf("%d seconds", clockNano);
                        //uses overflow counter to dtermine how many processes do not get to finish, and if they need to run again
                        if(timeNeeded[procs] != 0)
                        {
                                //incrementing j as overflow value
                                timeNeeded[j+18] = timeNeeded[procs];
                                j++;
                        }

                        execv(workerArr[0], workerArr);
                        fprintf(stderr,"Exec failed, terminating\n");
                        procs--;
                        exit(1);
                }

                wait(1);


                //supposed to check what the status of user_proc decided, but could not make work in my code
                /*
                if(sharedStatus == 1)
                {
                        printf("OSS: process is trying to read from location %d", sharedFrameLoc);
                }
                else if(sharedStatus == 2)
                (
                        q = (int)sharedFrameLoc % 1024;
                        s = (int)sharedFrameLoc - q;
                        x = s / 1024;

                )
                */
        }

        printf("OSS: The Great Termination Cometh\n");

        //freeing from shared memory
        shmdt(sharedClockSec);
        shmctl(shmid1, IPC_RMID, NULL);
        shmdt(sharedClockNano);
        shmctl(shmid2, IPC_RMID, NULL);
        shmdt(sharedStatus);
        shmctl(shmid1, IPC_RMID, NULL);
        shmdt(sharedFrameLoc);
        shmctl(shmid2, IPC_RMID, NULL);
        return EXIT_SUCCESS;

}
