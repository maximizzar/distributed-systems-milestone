#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define SHMSIZE 16777216

int main()
{
    int shmid;
    key_t key;
    char *shm;
    int semid;

    /* Erstelle einen Key für das Shared Memory */
    key = ftok("receiver.c", 'R');

    /* Erstelle das Shared Memory */
    shmid = shmget(key, SHMSIZE, 0666);
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

    /* Verbinde den Prozess mit dem Semaphore */
    semid = semget(key, 1, 0);
    if (semid < 0)
    {
        perror("semget");
        return 1;
    }

    struct sembuf sem_wait = {0, -1, 0}; /* struct für semop warten/schließen */
    struct sembuf sem_signal = {0, 1, 0}; /* struct für semop signalisieren/öffnen */
    struct timeval start, end; /* struct für Zeitmessung */
    
    /* Startzeit */
    gettimeofday(&start, NULL);
 
    /* malloc allocate one Byte */
    char *message = (char *) malloc(1);
    message[0] = 'a';

    /* 1000 mal senden */
    for (int i = 0; i < 1000; i++) {

    /* Sende Byte an den Receiver */
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
    while(byte != 'b') {
    memcpy(&byte, shm, 1);
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

    printf("Latenz: %fms\n", latency);

////////////////// ab hier Bandbreite ///////////////////////////////////////////////////////////////////////////////////////////////////////
  
    int dataSize = 1024;  
    char *data = (char *) malloc(16777216);

    for (int j = 0; j < 15; j++) {

    /* Startzeit */
    gettimeofday(&start, NULL);

    /* 1000 mal senden */
    for (int i = 0; i < 1000; i++) {

    /* Sende Bytes an den Receiver */
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
    memcpy(data, shm, dataSize);
    //printf("%d %c\n", dataSize, data[0]);
    //printf("%d %d Empfangenes Byte: %s\n",i, dataSize, data);
    }

    /* Endzeit */
    gettimeofday(&end, NULL);

     /* Durchschnittliche bandbreite */
    double time = (end.tv_sec - start.tv_sec);
    time += (end.tv_usec - start.tv_usec) / ((double) 1e6); /* mikrosekunden in sekunden umrechnen */
    double bandwidth;
    bandwidth = dataSize / time;
    printf("Zeit: %fs Bandbreite: %.2fKB/s / %.2fMB/s Bytes: %d\n", time, bandwidth, bandwidth / 1000, dataSize);

    dataSize *= 2; /* dataSize für nächsten Durchgang verdoppeln */
    }
    shmdt(shm); /* Shared Memory von prozess entfernen */
    shmctl(shmid, IPC_RMID, NULL); /* Shared Memory löschen */
    semctl(semid, 0, IPC_RMID); /* Semaphore löschen */
    return 0;
}
