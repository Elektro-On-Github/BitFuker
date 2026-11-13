#include <stdio.h>
#include <SDL3/SDL.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void vline(unsigned char *framebuffer, int dimFB, int width) {
    int i, x;
    for (i = 0; i < dimFB; i = i + width * 3) {
        int randomnum = (rand() % width * 3);
        for (x = 0; x < width - 1; x++) {
            framebuffer[randomnum + i + x + 0] = rand() % 255 + 1;
            framebuffer[randomnum + i + x + 1] = rand() % 255 + 1;
            framebuffer[randomnum + i + x + 2] = rand() % 255 + 1;
        }
    }
}

void xor(unsigned char *framebuffer, int dimFB) {
    int i;
    for (i = 0; i < dimFB; i++) {
        framebuffer[i] = framebuffer[i] ^ 255; // ^ fa lo xor
    }
}

void saturate(unsigned char *framebuffer, int dimFB) {
    int i;
    for (i = 0; i < dimFB; i++) {
        framebuffer[i] = framebuffer[i] * 2;
    }
}

void bug(unsigned char *framebuffer, int dimFB, int width) {
    int i, x;
    for (i = 0; i < dimFB; i++) {
        for (x = 0; x < width - 1; x++)
        framebuffer[i] = framebuffer[x*100] - framebuffer[i];
    }
}

void nintendo3ds_fx(unsigned char *framebuffer, int dimFB) {
    for (int i = 0; i < dimFB; i = i + 2) {
        framebuffer[i + 10] = framebuffer[i + 20];
        framebuffer[i + 20] = framebuffer[i + 40];
        framebuffer[i + 40] = framebuffer[i + 80];
    }
}

void soft_ghostfx(unsigned char *framebuffer, int dimFB) {
    for (int i = 0; i < dimFB; i = i + 2) {
        framebuffer[i + 1] = framebuffer[i + 10];
        framebuffer[i + 2] = framebuffer[i + 20];
        framebuffer[i + 3] = framebuffer[i + 30];
        framebuffer[i + 4] = framebuffer[i + 40];
        framebuffer[i + 5] = framebuffer[i + 50];
        framebuffer[i + 6] = framebuffer[i + 60];
        framebuffer[i + 7] = framebuffer[i + 70];
        framebuffer[i + 8] = framebuffer[i + 80];
        framebuffer[i + 9] = framebuffer[i + 90];
    }
}

void hard_ghostfx(unsigned char *framebuffer, int dimFB) {
    for (int i = 0; i < dimFB; i = i + 2) {
        framebuffer[i + 2] = framebuffer[i + 20];
        framebuffer[i + 4] = framebuffer[i + 40];
        framebuffer[i + 6] = framebuffer[i + 60];
        framebuffer[i + 8] = framebuffer[i + 80];
        framebuffer[i + 10] = framebuffer[i + 100];
        framebuffer[i + 12] = framebuffer[i + 120];
        framebuffer[i + 14] = framebuffer[i + 140];
        framebuffer[i + 16] = framebuffer[i + 160];
        framebuffer[i + 18] = framebuffer[i + 180];
        framebuffer[i] = framebuffer[i + 4];
        framebuffer[i] = framebuffer[i + 8];
        framebuffer[i] = framebuffer[i + 10];
        framebuffer[i] = framebuffer[i + 12];
        framebuffer[i] = framebuffer[i + 16];
        framebuffer[i] = framebuffer[i + 18];
        framebuffer[i] = framebuffer[i + 20];
        framebuffer[i] = framebuffer[i + 22];
        framebuffer[i] = framebuffer[i + 24];
    }
}

void granuly(unsigned char *framebuffer, int dimFB) {
    for (int i = 0; i < dimFB; i = i + rand() % 23) {
        framebuffer[i + 1] = rand() % 255;
        framebuffer[i + 2] = rand() % 255;
        framebuffer[i + 3] = rand() % 255;
    }
}

void blacknoise(unsigned char *framebuffer, int dimFB) {
    for (int i = 0; i < dimFB; i = i + rand() % 13) {
        framebuffer[i + 1] = 0;
        framebuffer[i + 2] = 0;
        framebuffer[i + 3] = 0;
    }
}

int main() {
    srand(time(NULL));
    // vedi sempre qui se ti serve
    int width, height;

    // crea un framebuffer che contiene i pixel raw di input.png
    // framebuffer è un pointer che contiene l'indirizzo di mem di inizio degli unsigned_char di 8 bit fino a 255 in dec
    // &width e &height sono dei refer alle variabili.
    // stbi_load("roba.png", address_of_width, address_of_height, numero_canali_esistenti, canali_in_out); usage:
    unsigned char *framebuffer = stbi_load("input.jpg", &width, &height, NULL, 3);
    
    //piccolo check al framebuffer per vedere se e' un pointer null
    if (!framebuffer) {return 1;}

    //calcolo dimensione del framebuffer
    int dimFB = width * height * 3;
    printf("%d\n", dimFB);
    printf("%d", width);


    vline(framebuffer, dimFB, width);
    xor(framebuffer, dimFB);
    saturate(framebuffer, dimFB);
    bug(framebuffer, dimFB, width);
    nintendo3ds_fx(framebuffer, dimFB);
    soft_ghostfx(framebuffer, dimFB);
    hard_ghostfx(framebuffer, dimFB);
    granuly(framebuffer, dimFB);
    blacknoise(framebuffer, dimFB);
    
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
