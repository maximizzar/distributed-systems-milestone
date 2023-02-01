#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    //Pipe erstellen
    int pipe_des[2];
    if (pipe(pipe_des) < 0 ) {
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
        //Schließen des nicht benötigten Eingangspipes
        close(pipe_des[0]);

        //Latenzmessung
        char message[1];
        message[0] = 'A';

        //Startzeit
        struct timeval start, end;
        gettimeofday(&start, NULL);

        //1000 Mal das Byte senden
        for (int i = 0; i < 1000; i++) {
            write(pipe_des[1], message, 1);
            read(pipe_des[1], message, 1);
        }

        //Endzeit
        gettimeofday(&end, NULL);

        //Durchschnittliche Latenzzeit berechnen
        double latency = (end.tv_sec - start.tv_sec) * 1000.0;
        latency += (end.tv_usec - start.tv_usec) / 1000.0;
        latency = latency/1000;

        //Ausgabe der Latenz
        printf("Latency: %f ms\n", latency);

        //Bandbreitenmessung
        int buffer_sizes[15] = {1024, 2048, 4096, 8192, 16384, 32758, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216};

    } else {
        //Kindprozess
        //Schließen des nicht benötigten Ausgangspipes
        close(pipe_des[1]);

        //Malloc zum Speichern der gelesenen Daten
        char *buffer = (char *) malloc (BUFFER_SIZE * sizeof(char));

        //Bandbreitenmessung
        int buffer_sizes[15] = {1024, 2048, 4096, 8192, 16384, 32758, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216};

        for (int i = 0; i < 15; i++) {
            //Startzeit
            struct timeval start, end;
            gettimeofday(&start, NULL);

            //Die Datenmenge senden
            for (int j = 0; j < buffer_sizes[i]; j++) {
                write(pipe_des[0], buffer, buffer_sizes[i]);
                read(pipe_des[0], buffer, buffer_sizes[i]);
            }

            //Endzeit
            gettimeofday(&end, NULL);

            //Durchschnittliche Bandbreite berechnen
            double bandwith = buffer_sizes[i] * 1000.0;
            double time = (end.tv_sec - start.tv_sec) * 1000.0;
            time += (end.tv_usec - start.tv_usec) / 1000.0;
            bandwith = bandwith/time;

            //Ausgabe der Bandbreite
            printf("Bandwidth: %f Kbps\n", bandwith);
        }

        //Speicher freigeben
        free(buffer);
    }

    //Pipe schließen
    close(pipe_des[0]);
    close(pipe_des[1]);

    return 0;
}