/**
 * 叶海辉
 * QQ群121376426
 * http://blog.yundiantech.com/
 */

#include "videoplayer.h"
#include "QDebug"

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
    #include "libswresample/swresample.h"
    #include "SDL.h"
    #include "libavutil/imgutils.h"
}

//Buffer:
//|-----------|-------------|
//chunk-------pos---len-----|
static  Uint8  *audio_chunk;
static  Uint32  audio_len;
static  Uint8  *audio_pos;

#include <stdio.h>
//音频回调函数
void  fill_audio(void *udata,Uint8 *stream,int len){
    //SDL 2.0
    SDL_memset(stream, 0, len);
    if(audio_len==0)
        return;

    len=(len>audio_len?audio_len:len);	/*  Mix  as  much  data  as  possible  */

    SDL_MixAudio(stream,audio_pos,len,SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}

VideoPlayer::VideoPlayer()
{

}

VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::startPlay()
{
    ///调用 QThread 的start函数 将会自动执行下面的run函数 run函数是一个新的线程
    this->start();

}

void VideoPlayer::run()
{
    //char *file_path = mFileName.toUtf8().data();
    char file_path[512] = {0};
    strcpy(file_path, mFileName.toUtf8().data());
    AVFormatContext *pFormatCtx;
    AVCodecContext *pvCodecCtx;
    AVCodecContext *paCodecCtx;
    AVCodec *pvCodec;
    AVCodec *paCodec;
    AVFrame *pFrame = NULL, *pFrameRGB = NULL;
    AVPacket *packet;
    uint8_t *out_buffer;
    uint8_t			*aout_buffer;//音频缓冲区
    SDL_AudioSpec wanted_spec;

    static struct SwsContext *img_convert_ctx;
    uint32_t len = 0;
    //int got_picture;
    int index = 0;
    int64_t in_channel_layout;
    struct SwrContext *au_convert_ctx;

    int videoStream, i, numBytes;
    int audioStream = -1;
    int ret, got_picture;

    av_register_all(); //初始化FFMPEG  调用了这个才能正常适用编码器和解码器

    //Allocate an AVFormatContext.
    pFormatCtx = avformat_alloc_context();//这个可以不用，因为下面的avformat_open_input内部可以为pFormatCtx分配空间

    if (avformat_open_input(&pFormatCtx, file_path, NULL, NULL) != 0) {
        printf("can't open the file. \n");
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Could't find stream infomation.\n");
        return;
    }
    av_dump_format(pFormatCtx, 0, file_path, 0); //输出视频信息
    videoStream = -1;

    ///循环查找视频中包含的流信息，直到找到视频类型的流
    ///便将其记录下来 保存到videoStream变量中
    ///这里我们现在只处理视频流  音频流先不管他
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }else if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audioStream = i;
        }
    }

    ///如果videoStream为-1 说明没有找到视频流
    if (videoStream == -1) {
        printf("Didn't find a video stream.\n");
        return;
    }
    if (audioStream == -1) {
        printf("Didn't find a audio stream.\n");
        return;
    }

    ///查找视频解码器
    pvCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pvCodec = avcodec_find_decoder(pvCodecCtx->codec_id);

    if (pvCodec == NULL) {
        printf("Codec not found video.\n");
        return;
    }
    //查找音频解码器
    paCodecCtx = pFormatCtx->streams[audioStream]->codec;
    paCodec = avcodec_find_decoder(paCodecCtx->codec_id);
    if (pvCodec == NULL) {
        printf("Codec not found audio.\n");
        return;
    }
    //打开视频解码器：使用pvCodec 来初始化 pvCodecCtx
    if (avcodec_open2(pvCodecCtx, pvCodec, NULL) < 0) {
        printf("Could not open codec video.\n");
        return;
    }
    //打开音频解码器：使用paCodec 来初始化 paCodecCtx
    if (avcodec_open2(paCodecCtx, paCodec, NULL) < 0) {
        printf("Could not open codec audio.\n");
        return;
    }
    pFrame = av_frame_alloc();//这里必须要为AVFrame分配内存
    pFrameRGB = av_frame_alloc();

    //这里我们改成了 将解码后的YUV数据转换成RGB32
    img_convert_ctx = sws_getContext(pvCodecCtx->width, pvCodecCtx->height,
            pvCodecCtx->pix_fmt, pvCodecCtx->width, pvCodecCtx->height,
            AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    //numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pvCodecCtx->width,pvCodecCtx->height);//该函数已经过期，使用下面的函数代替
    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, pvCodecCtx->width,pvCodecCtx->height,1);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_RGB32,
            pvCodecCtx->width, pvCodecCtx->height);
    //int dst_linesize = numBytes * sizeof(uint8_t);
//    if(av_image_fill_arrays(&out_buffer,&dst_linesize,NULL,AV_PIX_FMT_RGB32,pvCodecCtx->width,pvCodecCtx->height,1) <= 0){
//        qDebug()<<"av_image_fill_arrays failed!";
//        return ;
//    }

    int y_size = pvCodecCtx->width * pvCodecCtx->height;

    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
        return ;
    }

    //Out Audio Param
    uint64_t out_channel_layout=AV_CH_LAYOUT_STEREO;
    //nb_samples: AAC-1024 MP3-1152
    int out_nb_samples=paCodecCtx->frame_size;
    enum AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;
    int out_sample_rate=paCodecCtx->sample_rate;//注意，这里不要写成具体的采样频率，写成具体的采样频率可能会出现杂音
    int out_channels=av_get_channel_layout_nb_channels(out_channel_layout);
    //Out Buffer Size
    int out_buffer_size=av_samples_get_buffer_size(NULL,out_channels ,out_nb_samples,out_sample_fmt, 1);

    aout_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);
    pFrame=av_frame_alloc();

    //SDL_AudioSpec
    wanted_spec.freq = out_sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = out_channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = out_nb_samples;
    wanted_spec.callback = fill_audio;
    wanted_spec.userdata = paCodecCtx;

    if (SDL_OpenAudio(&wanted_spec, NULL)<0){
        printf("can't open audio.\n");
        return ;
    }
    //FIX:Some Codec's Context Information is missing
    in_channel_layout=av_get_default_channel_layout(paCodecCtx->channels);
    //Swr

    au_convert_ctx = swr_alloc();
    au_convert_ctx=swr_alloc_set_opts(au_convert_ctx,out_channel_layout, out_sample_fmt, out_sample_rate,
    in_channel_layout,paCodecCtx->sample_fmt , paCodecCtx->sample_rate,0, NULL);
    swr_init(au_convert_ctx);

    //Play
    SDL_PauseAudio(0);
    while (1)
    {
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            break; //这里认为视频读取完了
        }

        if (packet->stream_index == videoStream) {
            ret = avcodec_decode_video2(pvCodecCtx, pFrame, &got_picture,packet);
            if(ret == 0){
                printf("decode finished.\n");
                return;
            }
            if (ret < 0) {
                printf("decode error.\n");
                return;
            }

            if (got_picture) {
                sws_scale(img_convert_ctx,
                        (uint8_t const * const *) pFrame->data,
                        pFrame->linesize, 0, pvCodecCtx->height, pFrameRGB->data,
                        pFrameRGB->linesize);

                //把这个RGB数据 用QImage加载
                QImage tmpImg((uchar *)out_buffer,pvCodecCtx->width,pvCodecCtx->height,QImage::Format_RGB32);
                QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
                emit sig_GetOneFrame(image);  //发送信号
            }
        }else if(packet->stream_index == audioStream){
            ret = avcodec_decode_audio4( paCodecCtx, pFrame,&got_picture, packet);
            if ( ret < 0 ) {
                printf("Error in decoding audio frame.\n");
                return;
            }
            if ( got_picture > 0 ){
                swr_convert(au_convert_ctx,&out_buffer, MAX_AUDIO_FRAME_SIZE,(const uint8_t **)pFrame->data , pFrame->nb_samples);
#if 0
                printf("index:%5d\t pts:%lld\t packet size:%d\n",index,packet->pts,packet->size);
#endif
                index++;
            }
            while(audio_len>0);//Wait until finish
                //SDL_Delay(1);

            //Set audio buffer (PCM data)
            audio_chunk = (Uint8 *) out_buffer;
            //Audio buffer length
            audio_len =out_buffer_size;
            audio_pos = audio_chunk;



        msleep(5); //停一停  不然放的太快了
    }
        //SDL_Delay(20);
        av_free_packet(packet);
    }
    av_free(aout_buffer);
    av_free(pFrameRGB);
    avcodec_close(pvCodecCtx);
    avformat_close_input(&pFormatCtx);
}
