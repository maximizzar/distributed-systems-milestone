#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <wait.h>

int main(int argc, char *argv[])
{
    //Pipes erstellen
    int pipe_des[2];
    int pipe_des2[2];
    if (pipe(pipe_des) < 0 ) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe_des2) < 0 ) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    //Forks erstellen
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        //Elternprozess
        close(pipe_des[0]);
        close(pipe_des2[1]);
        //Latenzmessung
        char message[1];
        message[0] = 'A';

        //Startzeit
        struct timeval start, end;
        gettimeofday(&start, NULL);

        //1000 Mal das Byte senden
        for (int i = 0; i < 1000; i++) {
            write(pipe_des[1], message, 1);
            read(pipe_des2[0], message, 1);

        }
        printf("Parent hat 1000 mal 1 Byte gesendet und empfangen, das gegebene Byte: %c\n", message[0]);
        //Endzeit
        gettimeofday(&end, NULL);

        //Durchschnittliche Latenzzeit berechnen
        double latency = (end.tv_sec - start.tv_sec) * 1000.0;
        latency += (end.tv_usec - start.tv_usec) / 1000.0;

        //Ausgabe der Latenz
        printf("Latency: %f ms\n", latency);


        //Sende Messwerte zurück für Bandbreite
        for (int i = 1024; i <= (1024 * 1024) * 16; i*=2) {

            //Malloc
            char *buffer = (char *) malloc (1);

            //Die Datenmenge senden
            for (int j = 1; j <= i; j=j+ 1024) {
                read(pipe_des2[0], buffer, i);
                write(pipe_des[1], buffer, i);
            }

            free(buffer);
        }

        close(pipe_des[1]);
        close(pipe_des2[0]);
    } else {
        //Kindprozess
        close(pipe_des[1]);
        close(pipe_des2[0]);
        //Latenzmessung
        char message[1];
        message[0] = 'A';
        //1000 Mal das Byte senden
        for (int i = 0; i < 1000; i++) {
            read(pipe_des[0], message, 1);
            write(pipe_des2[1], message, 1);
        }
        printf("Child hat 1000 mal 1 Byte gesendet und empfangen, das gegebene Byte: %c\n", message[0]);

        //Bandbreitenmessung
        for (int i = 1024; i <= (1024 * 1024) * 16; i*=2) {

            //Malloc
            char *buffer = (char *) malloc (1);
            //printf("I = %d\n", i);
            //Startzeit
            struct timeval start, end;
            gettimeofday(&start, NULL);

            //Die Datenmenge senden
            for (int j = 1; j <= i; j=j+ 1024) {
                write(pipe_des2[1], buffer, i);
                read(pipe_des[0], buffer, i);
                //      printf("J = %d\n", j);
            }
            //Endzeit
            gettimeofday(&end, NULL);

            //Durchschnittliche Bandbreite berechnen
            double time = (end.tv_sec - start.tv_sec) * 1000;
            time += (end.tv_usec - start.tv_usec) / 1000;
            double bandwidth = i;
            bandwidth = bandwidth / time;

            //Ausgabe der Bandbreite
            printf("ByteSize: %d KiB, Bandwidth: %f Kbit/s, %f Mbit/s\n", i / 1024, bandwidth, bandwidth / 1000.0);
            free(buffer);
            close(pipe_des[0]);
            close(pipe_des2[1]);
        }
        close(pipe_des[0]);
        close(pipe_des[1]);
        close(pipe_des2[0]);
        close(pipe_des2[1]);
        wait(NULL);
    }

    return 0;
}