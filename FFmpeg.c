
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

int main(void)
{
    char *input = "https://remotion.media/video-10m.mp4";
    char command[4096];
    int width, height;

    // Dimensioni video
    snprintf(command, sizeof(command), "ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=p=0:s=x \"%s\"", input);

    FILE *probe = popen(command, "r");
    if (!probe || fscanf(probe, "%dx%d", &width, &height) != 2) {return 1;}

    pclose(probe);

    // Buffer RGB24
    size_t size = (size_t)width * height * 3;
    unsigned char *buffer = malloc(size);
    if (!buffer) {return 1;}

    // Decodifica continua a 30 FPS
    snprintf(command, sizeof(command), "ffmpeg -v error -i \"%s\" -an -vf fps=30 -f rawvideo -pix_fmt rgb24 pipe:1", input);

    FILE *stream = popen(command, "r");
    if (!stream) {
        free(buffer);
        return 1;
    }

    // SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {return 1;}

    SDL_Window *window = SDL_CreateWindow("BitFuker", width, height, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);

    if (!window || !renderer || !texture) {return 1;}

    SDL_Event event;
    int running = 1;
    Uint64 next_frame = SDL_GetTicks();

    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {running = 0;}
        }

        if (!running) {break;}

        // Legge un frame completo
        if (fread(buffer, 1, size, stream) != size) {break;}

        // Aggiorna texture
        SDL_UpdateTexture(texture, NULL, buffer, width * 3);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Sincronizzazione a 30 FPS
        double frame_time = 1000.0 / 30.0;
        next_frame += frame_time;

        while (SDL_GetTicks() < next_frame && running) {
            SDL_PollEvent(&event);

            if (event.type == SDL_EVENT_QUIT) {running = 0;}
        }
    }

    pclose(stream);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(buffer);
    return 0;
}
