#include <stdlib.h>

void hline(unsigned char *framebuffer, int dimFB, int width) {
    int height = dimFB / (width * 3);

    for (int n = 0; n < 30; n++) {
        int y = rand() % height;

        for (int x = 0; x < width; x++) {
            int pos = (y * width + x) * 3;

            framebuffer[pos + 0] = rand() % 256;
            framebuffer[pos + 1] = rand() % 256;
            framebuffer[pos + 2] = rand() % 256;
        }
    }
}

void xorfx(unsigned char *framebuffer, int dimFB, int width) {
    int i;
    for (i = 0; i < dimFB; i++) {
        framebuffer[i] = framebuffer[i] ^ 255; // ^ fa lo xor
    }
}

void saturate(unsigned char *framebuffer, int dimFB, int width) {
    int i;
    for (i = 0; i < dimFB; i++) {
        if (framebuffer[i] * 5 > 255) {framebuffer[i] = 255;} // se supera il 255 con la molt * 5 moltiplica per 2
        else {framebuffer[i] = framebuffer[i] * 2;}
    }
}

void bug(unsigned char *framebuffer, int dimFB, int width) {
    int i, x;
    for (i = 0; i < dimFB; i++) {
        x = i * 0.001;
        framebuffer[i] = framebuffer[x] - framebuffer[i];
    }
}


void nintendo3ds_fx(unsigned char *framebuffer, int dimFB, int width) {
    for (int i = 0; i < dimFB - 81; i = i + 2) {
        framebuffer[i + 10] = framebuffer[i + 20];
        framebuffer[i + 20] = framebuffer[i + 40];
        framebuffer[i + 40] = framebuffer[i + 80];
    }
}

void soft_ghostfx(unsigned char *framebuffer, int dimFB, int width) {
    for (int i = 0; i < dimFB - 100; i = i + 2) {
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

// da ottimizzare SUBITO
void hard_ghostfx(unsigned char *framebuffer, int dimFB, int width) {
    for (int i = 0; i < dimFB - 200; i = i + 2) {
        framebuffer[i + 2] = framebuffer[i + 20];
        framebuffer[i + 4] = framebuffer[i + 40];
        framebuffer[i + 6] = framebuffer[i + 60];
        framebuffer[i + 8] = framebuffer[i + 80];
        framebuffer[i + 10] = framebuffer[i + 100];
        framebuffer[i + 12] = framebuffer[i + 120];
        framebuffer[i + 14] = framebuffer[i + 140];
        framebuffer[i + 16] = framebuffer[i + 160];
        framebuffer[i + 18] = framebuffer[i + 180];
    }
}

void granuly(unsigned char *framebuffer, int dimFB, int width) {
    for (int i = 0; i < dimFB - 4; i = i + rand() % 46 + 1) {
        framebuffer[i + 1] = rand() % 255;
        framebuffer[i + 2] = rand() % 255;
        framebuffer[i + 3] = rand() % 255;
    }
}

void blacknoise(unsigned char *framebuffer, int dimFB, int width) {
    for (int i = 0; i < dimFB - 4; i = i + rand() % 12 + 1) {
        framebuffer[i + 1] = 0;
        framebuffer[i + 2] = 0;
        framebuffer[i + 3] = 0;
    }
}

void hugedistortion(unsigned char *framebuffer, int dimFB, int width) {
    int i;
    int kounter = 1;
    for (i = 0; i < (dimFB - (kounter + 1)); i++) {
        framebuffer[i] = framebuffer[i+kounter] - framebuffer[i+15];
    }
}

void CRTglitch(unsigned char *framebuffer, int dimFB, int width) {
    int i;
    int kounter = 42;
    for (i = 0; i < (dimFB - (kounter + 1)); i ++) {
        framebuffer[i] = framebuffer[i] - framebuffer[i+kounter];
    }
}

// questa func non fa un cazzo.
// se non vuoi applicare effetti usala
void nofx(unsigned char *framebuffer, int dimFB, int width) {}