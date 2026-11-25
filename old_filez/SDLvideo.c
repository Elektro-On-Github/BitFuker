/* gcc -std=c23 -Wall -Wextra -g -I../SDL3-3.4.20/include SDLvideo.c -Lbuild/sdl3 -Wl,-rpath,'$ORIGIN/build/sdl3' $(pkg-config --cflags --libs libavformat libavcodec libavutil libswscale) -lSDL3 -o video_test*/

#include <stdio.h>

#include <libavformat/avformat.h>
// avformat si occupa di aprire le sorgenti multimediali
// tipo file, stream ecc...
// video.mp4
// video.mkv
// https://server/video.mp4
// udp://127.0.0.1:5000
// rtsp://192.168.1.10/stream
// lui apre file e connessioni. gestisce protocolli e formati contenitore. 
// distingue audio e video e legge i pacchetti di dati codificati (non fa decode, solo roba digitale non leggibile).

#include <libavcodec/avcodec.h>
// fa il decode dei codec (like: h264, h265, vp9, av1, mpeg2 ecc...)

#include <libavutil/avutil.h>
// gestione mem & errori, math, timestamp e altre cose noiose

#include <libswscale/swscale.h>
// può fare conversione di pixel (tipo da NV12 a RGB24) e resizing (anche color space conversion e altro)

#include <libavutil/error.h>
// gestione errori, da codice numerico a stringa di errore

#include <SDL3/SDL.h>

#include <libavutil/imgutils.h>
//roba per preparare il frambufeffer RGB24

int SDLvideo_main(const char *source) {
    if (source == NULL) {printf("No source specified.\n"); return 1;} // se non c'è sorgente, bang

    AVFormatContext *format_ctx = NULL; // AVFormatContext = Tipo struttura di ffmpeg. scheda tecnica del file multimediale

    // apriamo la sorgente video
    // avformat_open_input(passiamo l'indirizzo del pointer dove buttare la struttura dati del file/flusso, sorgente, autotype (mp4, mkv, avi...), roba aggiuntiva per i protocolli);
    int avopen = avformat_open_input(&format_ctx, source, NULL, NULL);

    if (avopen < 0) { // se ci sono errori
        char errbuf[AV_ERROR_MAX_STRING_SIZE]; // dimensione datat da fmmpeg che dichiara la lunghezza del msg di errore
        av_strerror(avopen, errbuf, sizeof(errbuf)); // da codice errore (ex: -2) a text (ex: file not found)
        fprintf(stderr, "BitFuker: %s\n", errbuf); // stampa errore
        return 1; // bang
    }

    // Analizziamo gli stream presenti nella sorgente
    // avformat_find_stream_info(); piglia tutte le caratteristiche, tipo codec, tracce, subtitlez, resolution, e altro.
    int stream_info = avformat_find_stream_info(format_ctx, NULL); // contiene solo il codice di ritorno

    if (stream_info < 0) {
        //stampa errore
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(stream_info, errbuf, sizeof(errbuf));
        fprintf(stderr, "BitFuker: %s\n", errbuf);

        avformat_close_input(&format_ctx); // chiudi struttura e killa il video
        return 1;
    }

    int video_stream_index = -1; // -1 = non trovato
    // unsigned int = int senza segno
    for (unsigned int i = 0; i < format_ctx->nb_streams; i++) { // ciclo che si ferma quando i è uguale al numero di stream presenti nel file
        AVStream *stream = format_ctx->streams[i];  // numero di stream (vid, audio, subtitle ecc...) e lo mettiamo in stream

        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) { // verifica se il tipo di codec è video (AVMEDIA_TYPE_VIDEO = 0)
            video_stream_index = i;
            printf("Video stream found! Index: %u\n", i);
            break;
        }

        else {printf("Stream %u is not a video stream.\n", i);}
    }

    if (video_stream_index == -1) {
        printf("No video stream found in the source.\n"); 
        avformat_close_input(&format_ctx); 
        return 1;
    } // stream error

    AVStream *video_stream = format_ctx->streams[video_stream_index]; // prende il numero dello stream
    double fps = av_q2d(video_stream->avg_frame_rate); // crea una var con gli fps del video

    //check FPS
    if (fps <= 0) {fprintf(stderr, "Invalid video framerate.\n"); return 1;}
    
    double frame_time = 1.0 / fps; // frame time in sec (0.00xx)
    printf("Video FPS: %.2f\n", fps); // stampa

    const AVCodec *codec = avcodec_find_decoder(format_ctx->streams[video_stream_index]->codecpar->codec_id); // capisce il codec e poi lo usa per decodificare il video. codecpar->codec_id = tipo di codec (h264, h265 ecc...)

    if (codec == NULL) { // se non trova il decoder droppa errore
        fprintf(stderr, "Decoder not found.\n");
        avformat_close_input(&format_ctx);
        return 1;
    }

    // struct che contiene robe  di stato sui video
    // la struct contiene il bitrate, e robe dinamiche che sa solo ffmpeg, non ci ho capito molto
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);

    if (codec_ctx == NULL) { // check
        fprintf(stderr, "Cannot allocate decoder context.\n");
        avformat_close_input(&format_ctx);
        return 1;
    }

    // copia i param del file / flusso al e li passa al decoder (fondamentale per la decodifica, quindi per visualizzare roba a schermo)
    int fordecodestuff = avcodec_parameters_to_context(codec_ctx, format_ctx->streams[video_stream_index]->codecpar); 

    if (fordecodestuff < 0) {
        fprintf(stderr, "Cannot copy codec parameters.\n");
        avcodec_free_context(&codec_ctx); // male che va killa AVCodecContext
        avformat_close_input(&format_ctx); // close input
        return 1;
    }

    // qui apriamo il decoder del codec rilevato
    fordecodestuff = avcodec_open2(codec_ctx, codec, NULL);

    if (fordecodestuff < 0) { // stessa roba di prima
        fprintf(stderr, "Cannot open decoder.\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return 1;
    }

    // inizializza SDL con dei check integrati
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL Init error: %s\n", SDL_GetError());
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("BitFuker", codec_ctx->width, codec_ctx->height, SDL_WINDOW_RESIZABLE);

    //check errori
    if (window == NULL) {
        fprintf(stderr, "Window error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL); // renderizza la finestra

    // check per vedere se il render funziona, altrimenti pulisce
    if (renderer == NULL) {
        fprintf(stderr, "Renderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }

    // SDL_Texture *texture = SDL_CreateTexture(puntatore a renderer, tipo framebuffer, tipo di accesso (streaming è per le robe che richiedono un fottio di banda), codec_ctx->width, codec_ctx->height);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, codec_ctx->width, codec_ctx->height);

    // idem qui, check e pulizia se va male
    if (texture == NULL) {
        fprintf(stderr, "Texture error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return 1;
    }

    // passa la roba nelle variabili
    int width = codec_ctx->width;
    int height = codec_ctx->height;

    // alloca lo spazio per il framebuffer
    unsigned char *rgb_video_buffer = av_malloc(width * height * 3);

    // check per vedere se il pointer non punta a un cazzo
    if (rgb_video_buffer == NULL) {
        fprintf(stderr, "Cannot allocate RGB buffer.\n");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return 1;
    }

    // unsigned da 0 a 255 con un puntatore (dst_data), crea un array di 4 pointerz, tutti con un init a NULL
    // 4 pointer pe 4 framebuffer diversi (Ma RGB ne ha solo 1)
    uint8_t *dst_data[4] = {0};

    // ogni elemento contiene lo stride di un piano
    // la distanza di byte tra l'inizio di una riga e l'inizio della successiva
    // RGB24 usa solo un piano
    int dst_linesize[4] = {0};

    // av_image_fill_arrays(dove ffmpeg scrive gli indirizzi dei piani, dove scrive gli stride, buffer RGB, specifica formato RGB,larghezza, altezza, allineamento byte)
    // non ho ben capito cosa e' l'allineamento ma so che deve stare a 1
    int image_size = av_image_fill_arrays(dst_data, dst_linesize, rgb_video_buffer, AV_PIX_FMT_RGB24, width, height, 1);

    // check di av_image_fill_arrays()
    if (image_size < 0) {fprintf(stderr, "buffer RGB non inizializzato.\n"); return 1;}

    // prepara convertitore ffmpeg fa boh a rgb24
    // SwsContext tipo di struttura di ffmpeg
    // l'indirizzo contiene il return della func sws_getContext()
    // sws_getContext(width_originale, height_originale, codec_ctx->pix_fmt_formato_px_originale, width_destinazione, height_destinazione, AV_PIX_FMT_RGB24_formato_destinazione, SWS_BILINEAR_calcola_scaling_pulito, NULL_filtri_custom_w, NULL_filtri_custom_h, NULL_roba_plus_conversione);
    struct SwsContext *sws_ctx = sws_getContext(width, height, codec_ctx->pix_fmt, width, height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

    //check per il pointer
    if (sws_ctx == NULL) {printf("sws_getContext() error"); return 1;}

    // pacchetti, pezzi di dati compressi misteriosi che contengono i frame compressi del video. tipo h264, h265 ecc...
    AVPacket *packet = av_packet_alloc();

    if (packet == NULL) { // come a prima
        fprintf(stderr, "Cannot allocate packet.\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return 1;
    }

    // alloca e inizializza la struct che conterrà i frame decodificati e decompressi (tipo RGB24, YUV ecc...)
    // qui ci sta la roba raw
    AVFrame *frame = av_frame_alloc();

    // solito check come quelli di prima
    if (frame == NULL) {
        fprintf(stderr, "Cannot allocate frame.\n");
        av_packet_free(&packet);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return 1;
    }

    // roba per far funzionare gli eventi SDL
    int running = 1;
    SDL_Event event;

    int i = 0; // var per il kounter

    Uint64 frequency = SDL_GetPerformanceFrequency(); // piglia i tick fatti in un sec da SDL

    // il while legge i pacchetti dalla sorgente
    // format_ctx = struct che contiene tutte le info del file/flusso
    // packet = struct in cui ffmpeg inserisce il prossimo paacchetto
    while (av_read_frame(format_ctx, packet) >= 0) {
        if (packet->stream_index == video_stream_index) { // controlliamo a quale stream contiene il pacchetto (se è audio --> FUCKOFF, se è video --> OK, se è subtitle --> FUCKOFF)
            int ret = avcodec_send_packet(codec_ctx, packet); // consegna il pacchetto compresso al decoder
            
            // check che mi ha fatto ChatGPT. FFMpeg in C e' fottutamente difficile
            if (ret == AVERROR(EAGAIN)) {
                fprintf(stderr, "Decoder pieno: bisogna leggere i frame pendenti.\n");
                av_packet_unref(packet);
                continue;
            }

            if (ret < 0) {
                char errbuf[AV_ERROR_MAX_STRING_SIZE];
                av_strerror(ret, errbuf, sizeof(errbuf));
                fprintf(stderr, "Decoder error: %s\n", errbuf);
                av_packet_unref(packet);
                break;
            }

            // ciclo per chiudere la finestra
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {running = 0; break;}
            }

            if (!running) {av_packet_unref(packet); break;}

            while (avcodec_receive_frame(codec_ctx, frame) >= 0) {
                    Uint64 frame_start = SDL_GetPerformanceCounter(); // registra momento preciso in un inizia ad elaborare il frame

                    i = i + 1; // frame kounter
                    printf("Frame %d: %dx%d\n", i, frame->width, frame->height);

                    // sws_scale()
                    // sws_ctx: convertitore configurato prima
                    // frame->data: dati del frame originale
                    // frame->linesize: stride dei piani originali
                    // 0: prima riga da convertire
                    // frame->height: numero di righe da convertire
                    // dst_data: destinazione roba in RGB
                    // dst_linesize: stride destinazione RGB
                    // QUI AVVIENE LA CONVERSIONE EFFETTIVA
                    int check = sws_scale(sws_ctx, (const uint8_t * const *)frame->data, frame->linesize, 0, frame->height, dst_data, dst_linesize);

                    // piccoli checkz
                    if (check < 0) {printf("conversione fallita"); return 1;}
                    if (check != height) {printf("conversione incompleta"); return 1;}

                    // aggiorna texture della finestra
                    // texture: texture di SDL
                    // NULL: TUTTA LA TEXTURE
                    // dst_data[0]: indirizzo pixel RGB
                    // dst_linesize[0]: distanza tra righe
                    SDL_UpdateTexture(texture, NULL, dst_data[0], dst_linesize[0]);

                    // pulisce il contenuto del rendering target (e lo prepara per un nuovo disegno)
                    SDL_RenderClear(renderer);

                    // spara la roba nuova
                    // renderer: roba della finestra
                    // texture: roba da disegnare
                    // NULL: TUTTA la finestra
                    // NULL: Tutta l'area disponibile
                    SDL_RenderTexture(renderer, texture, NULL, NULL);

                    // presenta a schermo, la mostra
                    SDL_RenderPresent(renderer);

                    // frame end
                    Uint64 frame_end = SDL_GetPerformanceCounter();

                    // calcola tempo impiegato per l'elaborazione
                    double elapsed = (double)(frame_end - frame_start) / frequency;
                    double wait_time = frame_time - elapsed; // tempo di attesa

                    if (wait_time > 0.0) {
                        Uint64 delay_ns = (Uint64)(wait_time * 1000000000.0);
                        SDL_DelayPrecise(delay_ns);
                    }
                }
        }

        av_packet_unref(packet); // rilascia i dati dal pacchetto, cosi il prossimo può usare la struttura. Il pacchetto precedende è stato inviato da avcodec_send_packet() al decoder.
    }

    printf("Source: %s\n", source); // stampa source

    // meaning di ->: accedi a un membro di una struct tramite un puntatore.
    printf("Container: %s\n", format_ctx->iformat->long_name); 

    avformat_close_input(&format_ctx);

    return 0;
}

int main() {
<<<<<<< HEAD:old_filez/SDLvideo.c
    char *path[] = {"/home/elektrowindows/out.mp4"};
=======
    char *path[] = {"/home/elektrowindows/subwooferazzo_ok_h264.mp4"};
>>>>>>> 1e37f43 (edit prima del pull):SDLvideo.c
    SDLvideo_main(path[0]);
}