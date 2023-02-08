#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int main()
{
    int shmid;
    key_t key;
    char *shm;
    int semid;
    /* Erstelle einen Key für das Shared Memory */
    key = ftok("receiver.c", 'R');

    /* Verbinde den Prozess mit dem Shared Memory */
    shmid = shmget(key, 0, 0);
    if (shmid < 0)
    {
        perror("shmget");
        return 1;
    }

    shm = shmat(shmid, NULL, 0);
    if (shm == (char *) -1)
    {
        perror("shmat");
        return 1;
    }

    /* Verbinde den Prozess mit dem Semaphore */
    semid = semget(key, 0, 0);
    if (semid < 0)
    {
        perror("semget");
        return 1;
    }

    struct sembuf sem_wait = {0, -1, 0};
    struct sembuf sem_signal = {0, 1, 0};

    struct timeval start, end;
    
    /* Startzeit */
    gettimeofday(&start, NULL);

    
    /* malloc allocate one Byte */
    char *message = (char *) malloc(1);
    message[0] = 'a';


        /* 1024 mal senden */
        for (int i = 0; i < 1024; i++) {

    /* Warte auf das Semaphore */
    if (semop(semid, &sem_wait, 1) < 0)
    {
        perror("semop wait");
        return 1;
    }

    /* Sende ein Byte an den Server */
    //*shm = 'a';
    memcpy(shm, message, 1);

    /* Signalisiere das Semaphore */
    if (semop(semid, &sem_signal, 1) < 0)
    {
        perror("semop signal");
        return 1;
    }
    
    /* Warte auf das Semaphore */
    if (semop(semid, &sem_wait, 1) < 0)
    {
        perror("semop wait");
        return 1;
    }

    /* Lies das Byte aus dem Shared Memory */
    char byte;
    memcpy(&byte, shm, 1);

    /* Signalisiere das Semaphore */
    if (semop(semid, &sem_signal, 1) < 0)
    {
        perror("semop signal");
        return 1;
    }

    //printf("%d Empfangenes Byte: %c\n",i, byte);
    }

    /* Endzeit */
    gettimeofday(&end, NULL);

    /* Durchschnittliche Latenz für ein Byte */
    double sec = (end.tv_sec - start.tv_sec);
    double microsec = (end.tv_usec - start.tv_usec);
    double diff = (sec + microsec / ((double) 1e6)) / 1000; /* Zeit in Sekunden für eine Messung */
    double latency = (diff * ((double) 1e3)) / 2; /* Da hin und rückweg gemessen */

    printf("Latenz: %f  ms\n", latency);

    int dataSize = 1024;

//////////////////////// ab hier Bandbreite /////////////////////////////////////////////////////////////////////////////////////////////////

    /* Startzeit */
    gettimeofday(&start, NULL);
    for (int j = 0; j < 15; j++) {

    /* Startzeit */
    gettimeofday(&start, NULL);
        /* 1000 mal senden */
        for (int i = 0; i < 1000; i++) {

    /* Warte auf das Semaphore */
    if (semop(semid, &sem_wait, 1) < 0)
    {
        perror("semop wait");
        return 1;
    }

    /* Sende Bytes an den Server */
    memset(shm, 'a', dataSize);
    //printf("%d Datasize %d\n", i, dataSize);

    /* Signalisiere das Semaphore */
    if (semop(semid, &sem_signal, 1) < 0)
    {
        perror("semop signal");
        return 1;
    }
    
    /* Warte auf das Semaphore */
    if (semop(semid, &sem_wait, 1) < 0)
    {
        perror("semop wait");
        return 1;
    }

    /* Lies die Byte aus dem Shared Memory */
    char data[dataSize];
    memcpy(data, shm, dataSize);

    /* Signalisiere das Semaphore */
    if (semop(semid, &sem_signal, 1) < 0)
    {
        perror("semop signal");
        return 1;
    }

    //printf("%d Empfangenes Byte: %s\n",i, data);
        }

    /* Endzeit */
    gettimeofday(&end, NULL);
    double sek = (end.tv_sec - start.tv_sec);
    double microsek = (end.tv_usec - start.tv_usec) / ((double) 1e6);
    double cache = (sek + microsek / ((double) 1e6)) / 1000;
    double time = sek + microsek;

     /* Durchschnittliche bandbreite */
    double time2 = (end.tv_sec - start.tv_sec);
    time2 += (end.tv_usec - start.tv_usec) / ((double) 1e6);
    double bandwidth = dataSize / time2;
    printf("Zeit: %fs Bandbreite %.2fKB/s Bytes: %d\n", time, bandwidth, dataSize);

    dataSize *= 2;
    }
    return 0;
}
