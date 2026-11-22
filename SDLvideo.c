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
// gestione mem & errori, math, timestamp e altre cose niose

#include <libswscale/swscale.h>
// può fare conversione di pixel (tipo da NV12 a RGB24) e resizing (anche color space conversion e altro)

int SDLvideo_main(const char *source) {

    AVFormatContext *format_ctx = NULL; // AVFormatContext = Tipo struttura di ffmpeg. scheda tecnica del file multimediale

    // apriamo la sorgente video
    // avformat_open_input(passiamo l'indirizzo del pointer dove buttare la struttura dati del file/flusso, sorgente, autotype (mp4, mkv, avi...), roba aggiuntiva per i protocolli);
    int avopen = avformat_open_input(&format_ctx, source, NULL, NULL);

    if (avopen < 0) { // se ci sono errori
        char errbuf[AV_ERROR_MAX_STRING_SIZE]; // dimensione datat da fmmpeg che dichiara la lunghezza del msg di errore
        av_strerror(avopen, errbuf, sizeof(errbuf)); // da codice errore (ex: -2) a text (ex: file not found)
        printf("BitFuker: %s\n", errbuf); // stampa errore
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

    printf("Source: %s\n", source); // stampa source

    // meaning di ->: accedi a un membro di una struct tramite un puntatore.
    printf("Container: %s\n", format_ctx->iformat->long_name); 

    avformat_close_input(&format_ctx);

    return 0;
}