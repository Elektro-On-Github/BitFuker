#include <stdlib.h>

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

void xorfx(unsigned char *framebuffer, int dimFB) {
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
