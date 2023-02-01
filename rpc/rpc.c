#include <stdio.h> 
#include <rpc/rpc.h> 
#include <stdlib.h> 
#include <sys/time.h> 
#include <time.h> 

#define MAX_BYTES 16777216 

/* Struktur für RPC-Funktion */ 
struct rpcData { 
    int dataSize; 
    char data[MAX_BYTES]; 
}; 

/* Funktion im RPC-Server */ 
struct rpcData *rpc_function(struct rpcData *in) { 
    /* Der Pointer auf die Struktur */ 
    struct rpcData *out = (struct rpcData *) malloc(sizeof(struct rpcData)); 
    
    /* Die Datenmenge die gesendet wird */ 
    out->dataSize = in->dataSize; 
    
    /* Die Daten die gesendet werden */ 
    for (int i = 0; i < in->dataSize; i++) { 
        out->data[i] = in->data[i]; 
    } 
    
    /* Sende die Daten zurück */ 
    return out; 
} 

/* Funktion für die Messung der Latenz */ 
void latencyTest(int dataSize) { 
    /* Der Pointer auf die Struktur */ 
    struct rpcData *in = (struct rpcData *) malloc(sizeof(struct rpcData)); 
    
    /* Fülle die Struktur mit den Daten */ 
    in->dataSize = dataSize; 
    for (int i = 0; i < dataSize; i++) { 
        in->data[i] = 'A'; 
    } 
    
    /* Die Zeiten die für die Messung benötigt werden */ 
    struct timeval startTime, endTime; 
    
    /* Starte die Messung */ 
    gettimeofday(&startTime, NULL); 
    
    /* Sende die Daten 1000 Mal */ 
    for (int i = 0; i < 1000; i++) { 
        /* Sende die Daten */ 
        struct rpcData *out = rpc_function(in); 
        
        /* Überprüfe ob die Daten empfangen wurden */ 
        if (out == NULL) { 
            printf("Senden der Daten fehlgeschlagen!\n"); 
            return; 
        } 
    } 
    
    /* Beende die Messung */ 
    gettimeofday(&endTime, NULL); 
    
    /* Berechne die durchschnittliche Latenz */ 
    double latency = (endTime.tv_sec - startTime.tv_sec) * 1000.0; 
    latency += (endTime.tv_usec - startTime.tv_usec) / 1000.0; 
    //latency /= 1000.0; 
    
    /* Gebe die Latenz aus */ 
    printf("Latenz für %d Byte: %f ms\n", dataSize, latency); 
} 

/* Funktion für die Messung der Bandbreite */ 
void bandwidthTest(int dataSize) { 
    /* Der Pointer auf die Struktur */ 
    struct rpcData *in = (struct rpcData *) malloc(sizeof(struct rpcData)); 
    
    /* Fülle die Struktur mit den Daten */ 
    in->dataSize = dataSize; 
    for (int i = 0; i < dataSize; i++) { 
        in->data[i] = 'A'; 
    } 
    
    /* Die Zeiten die für die Messung benötigt werden */ 
    struct timeval startTime, endTime; 
    
    /* Starte die Messung */ 
    gettimeofday(&startTime, NULL); 
    
    /* Sende die Daten 1000 Mal */ 
    for (int i = 0; i < 1000; i++) { 
        /* Sende die Daten */ 
        struct rpcData *out = rpc_function(in); 
        
        /* Überprüfe ob die Daten empfangen wurden */ 
        if (out == NULL) { 
            printf("Senden der Daten fehlgeschlagen!\n"); 
            return; 
        } 
    } 
    
    /* Beende die Messung */ 
    gettimeofday(&endTime, NULL); 
    
    /* Berechne die durchschnittliche Bandbreite */ 
    double duration = (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000000.0;
    double bandwidth = (dataSize * 8) / duration; /* Berechne die Datenrate in Bit pro Sekunde */
    bandwidth /= 1024.0; /* Konvertiere von Bit pro Sekunde zu Kilobit pro Sekunde */

    
    /* Gebe die Bandbreite aus */ 
    printf("Bandbreite für %d Byte: %.2f Kbit/s\n", dataSize, bandwidth); 
} 

int main(int argc, char *argv[]) { 
    /* Starte die Latenzmessung */ 
    latencyTest(1); 
    
    /* Starte die Bandbreitenmessung */ 
    int dataSize = 1024; 
    while (dataSize <= MAX_BYTES) { 
        bandwidthTest(dataSize); 
        
        /* Erhöhe die Datenmenge */ 
        dataSize *= 2; 
    } 
    
    return 0; 
}
