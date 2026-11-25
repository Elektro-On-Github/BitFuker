#include <stdio.h>
#include <stdlib.h>

FILE *ffmpeg(const char *input) {
    char command[4096];

    // snprintf(destinazione, dimensione_massima, formato, valori);
    // command = destinazione di dovre scrivere la roba
    // sizeof(command) = dimensione massima del buffer
    // "ffmpeg -v error -i \"%s\" -frames:v 1 -f rawvideo -pix_fmt rgb24 pipe:1" =  comando da eseguire
    // input = valore da inserire nel formato della stringa
    snprintf(command, sizeof(command), "ffmpeg -v error -i \"%s\" -frames:v 1 -f rawvideo -pix_fmt rgb24 pipe:1", input);

    return popen(command, "r");
}