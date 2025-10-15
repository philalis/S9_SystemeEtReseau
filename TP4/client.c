#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "segdef.h"

struct sembuf sop;

void initialize(int *shmid, int *semid, segment **seg) {
    // Retrieve the shared memory segment identifier
    *shmid = shmget(cle, segsize, 0);
    if (*shmid == -1) {
        perror("Error shmget");
        exit(EXIT_FAILURE);
    }

    // Retrieve the semaphore set identifier
    *semid = semget(cle, 3, 0);
    if (*semid == -1) {
        perror("Error semget");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    *seg = (segment *) shmat(*shmid, NULL, 0);
    if (*seg == (void *) -1) {
        perror("Error shmat");
        exit(EXIT_FAILURE);
    }

    //Initialize the random number generator (provided by libseg.a)
    init_rand();

    printf("Initialization successful : shmid=%d, semid=%d\n", *shmid, *semid);
}

//Main client function
int main() {
    int shmid, semid;
    segment *seg;
    int i, req;
    long sum, client_avrg;

    //Initializations
    initialize(&shmid, &semid, &seg);

    //Request loop (for example, 5 iterations)
    for (req = 1; req <= 5; req++) {
        printf("Request %d\n", req);

        //Request to acquire seg_dispo
        acq_sem(semid, seg_dispo);
        printf("seg_dispo semaphore acquired\n");

        //Initialize the segment and compute the local average
        seg->pid = getpid();
        seg->req = req;

        sum = 0;
        for (i = 0; i < maxval; i++) {
            seg->tab[i] = getrand()%1000;
            sum += seg->tab[i];
        }
        client_avrg = sum / maxval;
        printf("Segment initialized (pid=%d, req=%d)\n", seg->pid, seg->req);

        //Notify the server that the segment is ready
        acq_sem(semid, seg_init);
        printf("seg_init semaphore acquired (signal sent to server)\n");

        //Wait for the server’s response
        wait_sem(semid, res_ok);
        printf("Server result available (res_ok)\n");

        //Read and compare the result
        printf("Client %d - Request %d :\n", seg->pid, seg->req);
        printf("Client calculated average : %ld\n", client_avrg);
        printf("Server returned average : %ld\n", seg->result);

        //Release the semaphores
        lib_sem(semid, seg_init);
        lib_sem(semid, seg_dispo);
        printf("seg_init and seg_dispo semaphores released\n");
    }

    //Detach the shared memory segment and terminate
    if (shmdt(seg) == -1) {
        perror("Error shmdt");
        exit(EXIT_FAILURE);
    }

    printf("End of client (pid=%d)\n", getpid());
    return 0;
}