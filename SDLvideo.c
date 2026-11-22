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

    AVFormatContext *format_ctx = NULL;

    int ret = avformat_open_input(&format_ctx, source, NULL, NULL);

    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_stderror(ret, errbuf, sizeof(errbuf));
        fprintf(stderr, "BitFuker: %s\n", errbuf);
        return 1;
    }

    printf("Source: %s\n", source);
    printf("Container: %s\n", format_ctx->iformat->long_name);

    avformat_close_input(&format_ctx);

    return 0;
}