#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <wctype.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_sound.h>
#include <SDL2/SDL_stdinc.h>
#include <libavcodec/codec.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>

#define PATH "/home/kesavan/Music/sample.wav"

static void
encode_audio(AVCodecContext* avctx, AVFrame* avframe, AVPacket* avpkt, FILE* output_file) {
    int ret = avcodec_send_frame(avctx, avframe);

    if(ret < 0) {
        fprintf(stderr, "Could not send frames to the encoder %s\n", av_err2str(ret));
        exit(1);
    }
    // signalling the end of the audio stream
    avcodec_send_frame(avctx, NULL);

    while(1) {
        ret = avcodec_receive_packet(avctx, avpkt);

        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        }else if(ret < 0) {
            fprintf(stderr, "Could not receive packets %s\n", av_err2str(ret));
            exit(1);
        }

        fwrite(avpkt->data, 1, avpkt->size, output_file);
        av_packet_unref(avpkt);
    }
}

static Uint8 *buf;
static Uint32 buf_len;

void audio_callback(void *userdata, Uint8 *stream, int len) {
    if (buf_len == 0) {
        SDL_memset(stream, 0, len);
        return;
    }
    len = (len < buf_len ? len : buf_len);
    SDL_memcpy(stream, buf, len);

    buf += len;
    buf_len -= len;
}

const char* SDL_GetFileFormat(const char* path) {
    int i = 0, index;
    while(i < strlen(path) + 1) {
        if(path[i] == '.') {
            index = i;
            break;
        }
        i = i + 1;
    }

    static char file_format[32];
    int j = 0;
    for(int k = index + 1; k < strlen(path) + 1; ++k) {
        file_format[j] = toupper(path[k]);
        j = j + 1;
    }
    return file_format;
}

int main(int argc, char **argv) {

    FILE* output_file = fopen("music.mp3", "wb");

    SDL_Init(SDL_INIT_AUDIO);
    Sound_Init();

    SDL_RWops *rw = SDL_RWFromFile(PATH, "rb");

    const char* file_format = SDL_GetFileFormat(PATH);
    Sound_Sample *sample = Sound_NewSample(rw, file_format, NULL, 128);

    if (!sample) {
        fprintf(stderr, "%s\n", Sound_GetError());
        exit(-1);
    }

    int bytes_read = Sound_DecodeAll(sample);
    if (!bytes_read) {
        fprintf(stderr, "%s\n", Sound_GetError());
        exit(-1);
    }

    if (bytes_read != sample->buffer_size) {
        fprintf(stderr, "All bytes are not read \n");
        exit(-1);
    }

    SDL_AudioSpec *desiredSpec = malloc(sizeof(SDL_AudioSpec));
    desiredSpec->freq = sample->desired.rate;
    desiredSpec->format = sample->desired.format;
    desiredSpec->channels = sample->desired.channels;
    desiredSpec->callback = audio_callback;
    desiredSpec->userdata = NULL;

    int devid = SDL_OpenAudioDevice(NULL, 0, desiredSpec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (devid == 0) {
        fprintf(stderr, "%s\n", Sound_GetError());
        exit(-1);
    }
    free(desiredSpec);

    buf = sample->buffer;
    buf_len = sample->buffer_size;

    SDL_PauseAudioDevice(devid, 0);
    while (buf_len > 0) {
        SDL_Delay(100);
    }

    enum AVCodecID codec_id = AV_CODEC_ID_MP3;
    AVCodec* codec = avcodec_find_encoder(codec_id);

    if(!codec) {
        fprintf(stderr, "Codec not found\n");
        return -1;
    }

    AVCodecContext* avctx;
    avctx = avcodec_alloc_context3(codec);
    avctx->codec_id = codec_id;
    avctx->sample_fmt = AV_SAMPLE_FMT_S16P;
    avctx->sample_rate = 44100;
    avctx->bit_rate = 64000;
    avctx->channel_layout = AV_CH_LAYOUT_STEREO;

    if(avctx == NULL) {
        fprintf(stderr, "Context error");
        return -1;
    }

    AVDictionary* av_dict = NULL;
    int can_open = avcodec_open2(avctx, codec, &av_dict);
    if(can_open < 0) {
        fprintf(stderr, "Could not open the codec %s\n", av_err2str(can_open));
        exit(1);
    }

    if(avctx < 0) {
        fprintf(stderr, "AVCodecContext not initialized\n");
        return -1;
    }

    AVPacket* avpkt = av_packet_alloc();
    if(!avpkt) {
        fprintf(stderr, "AVPacket not allocated\n");
        return -1;
    }

    uint8_t av_buf[1024];
    avpkt->data = av_buf;
    avpkt->size = sizeof(av_buf);

    AVFrame* avframe = av_frame_alloc();
    if(!avframe) {
        fprintf(stderr, "AVFrame not allocated\n");
        return -1;
    }
    avframe->nb_samples = avctx->frame_size;
    avframe->format = avctx->sample_fmt;
    avframe->channel_layout = avctx->channel_layout;
    av_frame_get_buffer(avframe, 0);

    // int ret = av_frame_make_writable(avframe);
    // if(ret < 0) {
    //     fprintf(stderr, "AVFrame not writable: %s\n", av_err2str(ret));
    //     return -1;
    // }

    // do this for a stereo channel
    memcpy(avframe->data[0], sample->buffer, avframe->nb_samples * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));
    encode_audio(avctx, avframe, avpkt, output_file);

    av_frame_free(&avframe);
    av_packet_free(&avpkt);
    avcodec_free_context(&avctx);

    Sound_Quit();

    Sound_FreeSample(sample);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Quit();
    return 0;
}
