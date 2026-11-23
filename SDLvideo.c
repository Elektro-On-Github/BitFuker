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

        if (video_stream_index == -1) {printf("No video stream found in the source.\n"); avformat_close_input(&format_ctx); return 1;} // stream error

        else {printf("Stream %u is not a video stream.\n", i);}
    }

    printf("Source: %s\n", source); // stampa source

    // meaning di ->: accedi a un membro di una struct tramite un puntatore.
    printf("Container: %s\n", format_ctx->iformat->long_name); 

    avformat_close_input(&format_ctx);

    return 0;
}