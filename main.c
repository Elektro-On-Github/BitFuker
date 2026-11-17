#include <stdio.h>
#include <SDL3/SDL.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

//h_filez
#include "imgFX.h"
#include "help.h"

// argc = argument count
// argv = argument vector (argomenti passati al programma sotto forma di array di stringhe)
int main(int argc, char *argv[]) {
    srand(time(NULL));
    // vedi sempre qui se ti serve
    int width, height;

    // param di base
    if (strcmp(argv[1], "--help") == 0) {help();} //check se argv[1] == "--help"
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
    printf("%d\n", dimFB);
    printf("%d", width);

    //param
    if (argc > 3) {help();}
    if (argv[2] == NULL) {help();} // se argv[2] == NULL il programma fa seg fault bcz of strcmp. Cosi evitiamo il crash
    if (strcmp(argv[2], "--vertical-line") == 0) {vline(framebuffer, dimFB, width);}
    if (strcmp(argv[2], "--xor") == 0) {xorfx(framebuffer, dimFB);}
    if (strcmp(argv[2], "--saturate") == 0) {saturate(framebuffer, dimFB);}
    if (strcmp(argv[2], "--bug") == 0) {bug(framebuffer, dimFB, width);}
    if (strcmp(argv[2], "--nintendo-3ds") == 0) {nintendo3ds_fx(framebuffer, dimFB);}
    if (strcmp(argv[2], "--soft-ghost") == 0) {soft_ghostfx(framebuffer, dimFB);}
    if (strcmp(argv[2], "--hard-ghost") == 0) {hard_ghostfx(framebuffer, dimFB);}
    if (strcmp(argv[2], "--granuly") == 0) {granuly(framebuffer, dimFB);}
    if (strcmp(argv[2], "--blacknoise") == 0) {blacknoise(framebuffer, dimFB);}

    // inizializza SDL
    bool InitVal = SDL_Init(SDL_INIT_VIDEO);

    // checkka SDL_init(); se ritorna roba che non è 0. Altrimenti errore
    if (!InitVal) {
        fprintf(stderr, "%s\n", SDL_GetError());
        stbi_image_free(framebuffer); // togliamo la merda
        return 1;
    }

    // il puntatore window contiene un refer alla finestra gestita da SDL
    // SDL_CreateWindow("nome finestra", larghezza, altezza, opzioni_strane (0 per niente))
    SDL_Window *window = SDL_CreateWindow("Framebuffer", width, height, 0);

    //image punta alla roba di SDL che crea l'immagine
    // SDL_CreateRGBSurfaceWithFormatFrom(nome_pointer_rawpixel, width, height, 8 bit * 3 canali = 24, width * 3 (ogni pixel è 3 byte), setting);
    SDL_Surface *image = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGB24, framebuffer, width * 3);


    // copia l'immagine nella finestra
    // SDL_BlitSurface(image pointer, NULL per tutta la img, copia in x, NULL per tutta la img)
    bool checkBlit = SDL_BlitSurface(image, NULL, SDL_GetWindowSurface(window), NULL);
    // refresha la tela che l'utente vede
    bool checkUpdate = SDL_UpdateWindowSurface(window);

    // check di errori
    if (!window || !image || !checkBlit || !checkUpdate) {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_DestroySurface(image);
        SDL_DestroyWindow(window);
        SDL_Quit();
        stbi_image_free(framebuffer);
        return 1;
    }

    SDL_Event event; // entry point per gli eventi di SDL
    // se l'entry point non dice di quittare si continua in loop a mostrare l'img

    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            break;
        }
    }

    SDL_DestroySurface(image);
    SDL_DestroyWindow(window);
    SDL_Quit();
    stbi_image_free(framebuffer);
    return 0;
}