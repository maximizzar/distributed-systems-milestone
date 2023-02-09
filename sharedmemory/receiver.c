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

    struct sembuf sem_wait = {0, -1, 0}; /* struct für semop warten/schließen */
    struct sembuf sem_signal = {0, 1, 0}; /* struct für semop signalisieren/öffnen */

    int dataSize = 1024;
    char *data = (char *) malloc(16777216);

    while (1)
    {   
        /* Warte auf das Semaphore */
        if (semop(semid, &sem_wait, 1) < 0)
        {
            perror("semop wait");
            return 1;
        }
        //printf("%ld\n", strlen(shm));
        dataSize = strlen(shm); 

        /* Lies Bytes aus dem Shared Memory */
        memcpy(data, shm, dataSize);

        /* Sende Bytes zurück an den Sender */
        memset(shm, 'b', dataSize);

        /* Signalisiere das Semaphore */
        if (semop(semid, &sem_signal, 1) < 0)
        {
            perror("semop signal");
            return 1;
        }
    }  
    return 0;
}
