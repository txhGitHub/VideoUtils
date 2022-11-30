#ifndef VIDEOENCODE_H
#define VIDEOENCODE_H

#include<string>
#include<list>

#include "cond.h"
#include "videocallback.h"
#include "utils.h"
#include "log.h"

#define MAX_AUDIO_SIZE (50 * 20)
#define MAX_VIDEO_SIZE (25 * 20)

#define FLUSH_DATA "FLUSH"
//在某些操作系统上，SDL_main()没有被定义为程序的入口点，会导致SDL_init初始化失败，添加SDL_MAIN_HANDLED规避这个条件
#define SDL_MAIN_HANDLED

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/channel_layout.h>
    #include <libswresample/swresample.h>
    #include <libavutil/opt.h>

    #include <SDL_audio.h>
    #include <SDL.h>
};
//基类的保护成员可以在派生类的成员函数中被访问

class VideoController
{
public:
    VideoController();
    bool startPlay(std::string &filePath);
    int readVideoFile();
    bool inputVideoQuene(const AVPacket &pkt);
    void setVideoCallback(VideoCallback *vcb);
    int openSDL();
    void closeSDL();

protected:
    void decodeVideoThread();
    void doDisplayVideo(const uint8_t *yuv420Buffer, const int &width, const int &height);
    static void sdlAudioCallBackFunc(void *userdata, Uint8 *stream, int len);
    int decodeAudioFrame();
    void sdlAudioCallBack(Uint8 *stream, int len);
    bool inputAudioQuene(const AVPacket &pkt);
    bool beforeVideoDecode(int streamId);
    bool beforeAudioDecode(int streamId);

private:
    std::string mFilePath;

    AVFormatContext* m_pInFormatCtx = nullptr;
    AVCodecContext * m_pVideoCodecCtx = nullptr;
    AVCodecParameters* m_pVideoCodecPar = nullptr;
    const AVCodec* m_pVidoeCodec;
    AVStream* m_pVideoStream; //视频流


    std::list<AVPacket> mVideoPacktList;

    ///音频相关-------------------------------
    AVCodecParameters *m_pAudioCodecPar;
    AVCodecContext * m_pAudioCodecCtx = nullptr;
    AVCodec *m_pAudioCodec;
    AVFrame *m_pAudioFrame;
    SwrContext *m_pSwrCtx;
    AVStream* m_pAudioStream; //音频流
    uint8_t *out_buffer[2];
    int m_outSampleRate;
    int m_outChannelLayout;
    int m_outNB_Sample;
    enum AVSampleFormat m_outFormat;



    std::list<AVPacket> mAudioPacktList;

    ///音频帧队列
    Cond *mConditon_Audio;
    double audio_clock; ///音频时钟

    ///以下变量用于音频重采样
    /// 由于ffmpeg解码出来后的pcm数据有可能是带平面的pcm，因此这里统一做重采样处理，
    /// 重采样成44100的16 bits 双声道数据(AV_SAMPLE_FMT_S16)
    AVFrame *m_pAFrame_ReSample;

    unsigned int audio_buf_size = 0;
    unsigned int audio_buf_index = 0;



    ///本播放器中SDL仅用于播放音频，不用做别的用途
    ///SDL播放音频相关
    SDL_AudioDeviceID mAudioID;
    bool mIsAudioThreadFinished; //音频播放线程


    //锁变量
    Cond *m_pConditon_Video;

    //音视频同步
    double m_videoClock;

    //帧回调
    VideoCallback* m_pVideoCallback;
};

#endif // VIDEOENCODE_H
