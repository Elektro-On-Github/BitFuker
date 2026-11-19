#include "SDLimg.h"
#include <stdio.h>
#include <SDL3/SDL.h>

#include <stb/stb_image.h>

//SDL stuff solo per immagini
int SDLimg(unsigned char *framebuffer, int height, int width) {
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