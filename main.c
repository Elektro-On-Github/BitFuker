#include <stdio.h>
#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

int main(void)
{
    // variabili usate per stabilire la larghezza e l'altezza
    int width, height;

    // crea un framebuffer che contiene i pixel raw di input.png
    // framebuffer è un pointer che contiene l'indirizzo di mem di inizio degli unsigned_char di 8 bit fino a 255 in dec
    // &width e &height sono dei refer alle variabili.
    // stbi_load("roba.png", address_of_widht, address_of_height, numero_canali_esistenti, canali_in_out); usage:
    unsigned char *framebuffer = stbi_load("input.jpg", &width, &height, NULL, 3);

    // inizializza SDL
    int InitVal = SDL_Init(SDL_INIT_VIDEO);

    // checkka SDL_init(); se ritorna roba che non è 0. Altrimenti errore
    if (InitVal != 0) {
        fprintf(stderr, "%s\n", SDL_GetError());
        stbi_image_free(framebuffer); // togliamo la merda
        return 1;
    }

    // il puntatore window contiene un refer alla finestra gestita da SDL
    // SDL_CreateWindow("nome finestra", Posizione x, Posizione y, larghezza, altezza, opzioni_strane (0 per niente))
    SDL_Window *window = SDL_CreateWindow("Framebuffer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);

    //image punta alla roba di SDL che crea l'immagine
    // SDL_CreateRGBSurfaceWithFormatFrom(nome_pointer_rawpixel, width, height, 8 bit * 3 canali = 24, width * 3 (ogni pixel è 3 byte), setting);
    SDL_Surface *image = SDL_CreateRGBSurfaceWithFormatFrom(framebuffer, width, height, 24, width * 3, SDL_PIXELFORMAT_RGB24);


    // copia l'immagine nella finestra
    // SDL_BlitSurface(image pointer, NULL per tutta la img, copia in x, NULL per tutta la img)
    int checkBlit = SDL_BlitSurface(image, NULL, SDL_GetWindowSurface(window), NULL) != 0;
    // refresha la tela che l'utente vede
    int checkUpdate = SDL_UpdateWindowSurface(window) != 0;

    // check di errori
    if (!window || !image || checkBlit || checkUpdate) {
        fprintf(stderr, "%s\n", SDL_GetError());
        SDL_FreeSurface(image);
        SDL_DestroyWindow(window);
        SDL_Quit();
        stbi_image_free(framebuffer);
        return 1;
    }

    SDL_Event event; // entry point per gli eventi di SDL
    // se l'entry point non dice di quittare si continua in loop a mostrare l'img
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {}

    SDL_FreeSurface(image);
    SDL_DestroyWindow(window);
    SDL_Quit();
    stbi_image_free(framebuffer);
    return 0;
}
