#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

//h_filez
#include "FXs/imgFX.h"
#include "help/help.h"
#include "SDLimg.h"
#include "SDLvideo.h"

// argc = argument count
// argv = argument vector (argomenti passati al programma sotto forma di array di stringhe)
int main(int argc, char *argv[]) {
    srand(time(NULL));
    // vedi sempre qui se ti serve
    int width, height;

    // param di base
    if (argc < 3) {help();return 0;} // campo ristretto per i param. avoidiamo roba indefinita
    char *IMGPath = argv[1];

    // crea un framebuffer che contiene i pixel raw di input.png
    // framebuffer è un pointer che contiene l'indirizzo di mem di inizio degli unsigned_char di 8 bit fino a 255 in dec
    // &width e &height sono dei refer alle variabili.
    // stbi_load("roba.png", address_of_width, address_of_height, numero_canali_esistenti, canali_in_out); usage:
    unsigned char *framebuffer = stbi_load(IMGPath, &width, &height, NULL, 3);
    
    //piccolo check al framebuffer per vedere se e' un pointer null
    if (!framebuffer) {return 1;}

    //calcolo dimensione del framebuffer
    int dimFB = width * height * 3;
    //printf("%d\n", dimFB);
    //printf("%d", width);

    //param
    if (strcmp(argv[2], "--vertical-line") == 0) {vline(framebuffer, dimFB, width);}
    else if (strcmp(argv[2], "--xor") == 0) {xorfx(framebuffer, dimFB);}
    else if (strcmp(argv[2], "--saturate") == 0) {saturate(framebuffer, dimFB);}
    else if (strcmp(argv[2], "--bug") == 0) {bug(framebuffer, dimFB);}
    else if (strcmp(argv[2], "--nintendo-3ds") == 0) {nintendo3ds_fx(framebuffer, dimFB);}
    else if (strcmp(argv[2], "--soft-ghost") == 0) {soft_ghostfx(framebuffer, dimFB);}
    else if (strcmp(argv[2], "--hard-ghost") == 0) {hard_ghostfx(framebuffer, dimFB);}
    else if (strcmp(argv[2], "--granuly") == 0) {granuly(framebuffer, dimFB);}
    else if (strcmp(argv[2], "--blacknoise") == 0) {blacknoise(framebuffer, dimFB);}
    else if (strcmp(argv[2], "--huge-distortion") == 0) {hugedistortion(framebuffer, dimFB);}
    else {help();}

    SDLimg(framebuffer, height, width);
}