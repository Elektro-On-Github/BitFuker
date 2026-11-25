
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void hard_ghostfx(unsigned char *framebuffer, int dimFB) {
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


int main(void) {
    char *input = "/home/elektrowindows/subwooferazzo_ok_h264.mp4";
    char command[4096];
    int width = 1920;
    int height = 1080;
    int fps = 60;

    size_t dimFB = (size_t)width * height * 3;
    unsigned char *framebuffer = malloc(dimFB);

    if (!framebuffer) return 1;

    // FFmpeg: decodifica in RGB24
    snprintf(command, sizeof(command),
        "ffmpeg -v error -i \"%s\" -an -f rawvideo "
        "-pix_fmt rgb24 pipe:1", input);

    FILE *decoder = popen(command, "r");
    if (!decoder) {
        free(framebuffer);
        return 1;
    }

    // FFplay: riceve RGB24 modificato
    snprintf(command, sizeof(command),
        "ffplay -v error -f rawvideo -pixel_format rgb24 "
        "-video_size %dx%d -framerate %d -i pipe:0",
        width, height, fps);

    FILE *player = popen(command, "w");
    if (!player) {
        pclose(decoder);
        free(framebuffer);
        return 1;
    }

    // Elaborazione frame per frame
    while (fread(framebuffer, 1, dimFB, decoder) == dimFB) {

        hard_ghostfx(framebuffer, dimFB);

        if (fwrite(framebuffer, 1, dimFB, player) != dimFB)
            break;
    }

    pclose(decoder);
    pclose(player);
    free(framebuffer);

    return 0;
}