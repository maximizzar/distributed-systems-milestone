#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SHMSIZE 16777216

int main()
{
    int shmid;
    key_t key;
    char *shm;
    int semid;
    union semun
    {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } sem_arg;

    /* Erstelle einen Key für das Shared Memory */
    key = ftok("receiver.c", 'R');

    /* Erstelle das Shared Memory */
    shmid = shmget(key, SHMSIZE, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        return 1;
    }

    /* Verknüpfe den Prozess mit dem Shared Memory */
    shm = shmat(shmid, NULL, 0);
    if (shm == (char *) -1)
    {
        perror("shmat");
        return 1;
    }

    /* Erstelle das Semaphore */
    semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid < 0)
    {
        perror("semget");
        return 1;
    }

    /* Initialisiere das Semaphore auf 1 */
    sem_arg.val = 1;
    if (semctl(semid, 0, SETVAL, sem_arg) < 0)
    {
        perror("semctl");
        return 1;
    }

    struct sembuf sem_wait = {0, -1, 0};
    struct sembuf sem_signal = {0, 1, 0};

    /* malloc allocate one Byte */
    char *test = (char *) malloc(16777216);
    //read[0] = 'b';

    int dataSize = 1024;

    while (1)
    {   
        /* Warte auf das Semaphore */
        if (semop(semid, &sem_wait, 1) < 0)
        {
            perror("semop wait");
            return 1;
        }
        printf("%ld <- dataSize im SharedMemory\n", strlen(shm));
        dataSize = strlen(shm);
        memcpy(test, shm, dataSize);

        /* Lies das Byte aus dem Shared Memory */
        char byte[dataSize];
        memcpy(byte, shm, dataSize);

        /* Sende das Byte zurück an den Client */
        //*shm = byte;
        memset(shm, 'b', dataSize);

        /* Signalisiere das Semaphore */
        if (semop(semid, &sem_signal, 1) < 0)
        {
            perror("semop signal");
            return 1;
        }
    }

    shmdt(shm); /* Shared Memory von prozess entfernen */
    shmctl(shmid, IPC_RMID, NULL); /* Shared Memory löschen */
    semctl(semid, 0, IPC_RMID); /* Semaphore löschen */

    return 0;
}
