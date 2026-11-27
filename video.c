#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include "FXs/imgFX.h"
#include <string.h>

#include "help/help.h"

int video(char *argv[]) {

    char *input = argv[2];

    // char buf dove stanno i comandi
    char command[4096];

    // video in input: width, height ed fps automatici
    // ffprobe restituisce una frazione quando chiedi gli FPS.
    // fps_num = numeratore; fps_den = denominatore;
    int width, height;
    int fps_num, fps_den;

    // snprintf serve per formattare una stringa per poi ficcarla in un buffer
    // snprintf(buffer_destinazione, dimensione_buffer, "testo_con_%s_ecc...", argomenti da formattare)
    snprintf(command, sizeof(command), "ffprobe -v error -select_streams v:0 -show_entries stream=width,height,avg_frame_rate -of csv=p=0:s=x \"%s\"", input);

    // esegue il comando su command e poi legge l'output in modalita' lettura
    FILE *probe = popen(command, "r");

    // se probe e' NULL o probe non ha 4 risultati allora qualcosa non funziona e bang, ret 1!
    if (probe == NULL || fscanf(probe, "%dx%dx%d/%d", &width, &height, &fps_num, &fps_den) != 4) {
        // chiude la pipe e killa ffprobe
        // serche chiudere se il pointer e' diverso da NULL
        if (probe != NULL) {pclose(probe);}
        return 1;
    }

    // killa e chiude se funziona
    pclose(probe);

    // calcola gli fps in input
    // anche se sono interi il risultato sara' in double
    double input_fps = (double)fps_num / fps_den;

    printf("INPUT: %dx%d @ %.3f FPS\n", width, height, input_fps);

    // OUTPUT: parametri da passare via argv
    int out_width = 1920;
    int out_height = 1080;
    double out_fps = 60;

    if (out_width <= 0 || out_height <= 0 || out_fps <= 0) {
        out_width = width;
        out_height = height;
        out_fps = input_fps;
    }

    double MBpersecond = (double)out_width * out_height * out_fps * 3.0 / (1024.0 * 1024.0);
    printf("MB per second: %.1f\n", MBpersecond);
    printf("INPUT: %dx%d @ %.3f FPS\n", out_width, out_height, out_fps);
    
    // Framebuffer RGB24 con i param di out custom
    // size_t si usa per le dimensioni
    size_t dimFB = (size_t)out_width * out_height * 3;

    //alloca byte per quanti sono i byte su dimFB
    unsigned char *framebuffer = malloc(dimFB);

    // cosa potrebbe mai andare storto!?
    // la ram potrebbe finire oppure dimFB e' enorme
    if (!framebuffer) {return 1;}

    // ffmpeg: fa decode con param custom
    snprintf(command, sizeof(command), "ffmpeg -v error -i \"%s\" -an -vf \"scale=%d:%d,fps=%.2f\" -f rawvideo -pix_fmt rgb24 pipe:1", input, out_width, out_height, out_fps);
    // apre come comando
    FILE *decoder = popen(command, "r");
    // se decoder = NULL fanculo al framebuffer :)
    if (!decoder) {free(framebuffer); return 1;}

    // ffplay: riproduzione della roba raw in rgb24 con i parametri custom di prima
    snprintf(command, sizeof(command), "ffplay -v error -f rawvideo -pixel_format rgb24 -video_size %dx%d -framerate %.2f pipe:0", out_width, out_height, out_fps);

    // apre comando con scrittura, apre una pipe in scrittura verso stdin di ffplay
    FILE *player = popen(command, "w");

    // se player == NULL; killa ffmpeg; libera il framebuffer; e poi ret 1
    if (!player) {
        pclose(decoder);
        free(framebuffer);
        return 1;
    }

    void (*fx)(unsigned char *framebuffer, int dimFB, int width) = NULL;

    if (strcmp(argv[3], "--vertical-line") == 0) {fx = vline;}
    else if (strcmp(argv[3], "--xor") == 0) {fx = xorfx;}
    else if (strcmp(argv[3], "--saturate") == 0) {fx = saturate;}
    else if (strcmp(argv[3], "--bug") == 0) {fx = bug;}
    else if (strcmp(argv[3], "--nintendo-3ds") == 0) {fx = nintendo3ds_fx;}
    else if (strcmp(argv[3], "--soft-ghost") == 0) {fx = soft_ghostfx;}
    else if (strcmp(argv[3], "--hard-ghost") == 0) {fx = hard_ghostfx;}
    else if (strcmp(argv[3], "--granuly") == 0) {fx = granuly;}
    else if (strcmp(argv[3], "--blacknoise") == 0) {fx = blacknoise;}
    else if (strcmp(argv[3], "--huge-distortion") == 0) {fx = hugedistortion;}
    else if (strcmp(argv[3], "--CRT-glitch") == 0) {fx = CRTglitch;}
    else {help(); return 0;}

    if (fx == NULL) {printf("fx pointer = NULL"); return 1;}
    // applica fx frame per frame
    // fread legge i byte della pipe di ffmpeg e li copia dentro il framebuffer
    // fread(destinazione, dimensione_elemento, numero_elementi, sorgente)
    while (fread(framebuffer, 1, dimFB, decoder) == dimFB) {

        fx(framebuffer, dimFB, width);

        if (fwrite(framebuffer, 1, dimFB, player) != dimFB) {break;}
    }

    // chiude tutto
    pclose(decoder);
    pclose(player);
    free(framebuffer);

    return 0;
}