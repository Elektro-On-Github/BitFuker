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

        else {printf("Stream %u is not a video stream.\n", i);}
    }

    if (video_stream_index == -1) {
        printf("No video stream found in the source.\n"); 
        avformat_close_input(&format_ctx); 
        return 1;
    } // stream error

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

    // il while legge i pacchetti dalla sorgente
    // format_ctx = struct che contiene tutte le info del file/flusso
    // packet = struct in cui ffmpeg inserisce il prossimo paacchetto
    while (av_read_frame(format_ctx, packet) >= 0) {
        if (packet->stream_index == video_stream_index) { // controlliamo a quale stream contiene il pacchetto (se è audio --> FUCKOFF, se è video --> OK, se è subtitle --> FUCKOFF)
            int ret = avcodec_send_packet(codec_ctx, packet); // consegna il pacchetto compresso al decoder
            if (ret < 0) {fprintf(stderr, "Pacchetti non ricevuti.\n");break;} // se è maggiore di 0 è un errore
        }
        av_packet_unref(packet); // rilascia i dati dal pacchetto, cosi il prossimo può usare la struttura. Il pacchetto precedende è stato inviato da avcodec_send_packet() al decoder.
    }

    printf("Source: %s\n", source); // stampa source

    // meaning di ->: accedi a un membro di una struct tramite un puntatore.
    printf("Container: %s\n", format_ctx->iformat->long_name); 

    avformat_close_input(&format_ctx);

    return 0;
}