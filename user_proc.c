/*
 *Jared Migneco
 *Hauschild Projecr 6
 *Due: 12/13/22
 *
 *
 *User_proc Class
 *
 *
 *
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
#include <sys/msg.h>
#include <string.h>
#include <time.h>

#define SHMKEY  960158     /* Parent and child agree on common key.*/
#define BUFF_SZ sizeof ( int )

//message buffer from https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_message_queues.htm
#define PERMS 0644
struct my_msgbuf {
   long mtype;
   char mtext[200];
};

int main(int argc, char** argv)
{
        printf("User_Proc: We are in the child class\n");
        int shmid1 = shmget ( SHMKEY, BUFF_SZ, 9045);
        int shmid2 = shmget ( SHMKEY, BUFF_SZ, 9046);



        //initializing needed variables for message buffer
        struct my_msgbuf buf;
        int msqid;
        int toend;
        int len;
        key_t key;
        char *msgText = "(Un)Locked and ready to go";
        system("touch msgq.txt");


        //intializing variables and setting value of m to passed parameter m from parent
        int m = atoi(argv[1]);
        int x;
        int i;

        //shared memory integers
        char *secClock = (char*) (shmat(SHMKEY, NULL, 9045));
        char *nanoClock = (char*) (shmat(SHMKEY, NULL , 9046));


        int *sharedSecClock = (int*)(secClock);
        int *sharedNanoClock = (int*)(nanoClock);

        //allowing child class to access frame and page tables
        char *status = (char*) (shmat(SHMKEY, NULL, 9045));
        char *frameLoc = (char*) (shmat(SHMKEY, NULL, 9046));

        int *sharedStatus = (int*)(status);
        int *sharedFrameLoc = (int*)(frameLoc);


        //random memory access variables
        srand(time(0));
        int randPage = rand() % 32;
        int randOffset = (randPage * 1024) + (rand() % 1023);

        //fate is 0 - 10, skewed 0-6 as read, 7-8 as write, and 9 as terminate
        int fate = rand() % 10;
        if (fate < 7)
        {
                //status of read puts 1 in shared memory
                printf("User_Proc: %d reference is a read at", getpid());
                printf(" page %d", randPage);
                printf(" with offset %d\n", randOffset);
                sharedStatus = 1;
                printf("\n testing %d\n", sharedStatus);
        }
        else if (fate < 9)
        {
                //status of write puts 2 in shared memory so oss knows write
                printf("User_Proc: %d reference is a write at ", getpid());
                printf(" page %d", randPage);
                printf(" with offset %d\n", randOffset);
                sharedStatus = 2;
                printf("\n testing %d\n", sharedStatus);
        }
        else
        {
                printf("User_Proc: reference is fated to terminate, goodbye \n");

        }

        //freeing memory
        shmdt(secClock);
        shmdt(sharedSecClock);
        shmdt(sharedStatus);
        shmdt(sharedFrameLoc);
        sleep(3);
        return EXIT_SUCCESS;
}
