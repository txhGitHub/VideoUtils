#ifndef VIDEOENCODE_H
#define VIDEOENCODE_H

#include<string>
#include<list>

#include "cond.h"
#include "videocallback.h"

#define MAX_AUDIO_SIZE (50 * 20)
#define MAX_VIDEO_SIZE (25 * 20)

#define FLUSH_DATA "FLUSH"

extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
};
//基类的保护成员可以在派生类的成员函数中被访问

class VideoController
{
public:
    VideoController();
    bool startPlay(std::string &filePath);
    void readVideoFile();
    bool inputVideoQuene(const AVPacket &pkt);
    void setVideoCallback(VideoCallback *vcb);

protected:
    void decodeVideoThread();
    void doDisplayVideo(const uint8_t *yuv420Buffer, const int &width, const int &height);

private:
    std::string mFilePath;
    AVFormatContext* m_pInFormatCtx = nullptr;
    AVCodecContext * m_pVideoCodecCtx = nullptr;
    AVCodecParameters* m_pVideoCodecPar = nullptr;
    const AVCodec* m_pCodec;
    AVStream* m_pVideoStream; //视频流
    AVStream* m_pAudioStream; //音频流
    AVCodecParameters* m_pavCodecParameters = nullptr;

    std::list<AVPacket> mVideoPacktList;
    std::list<AVPacket> mAudioPacktList;

    //锁变量
    Cond *m_pConditon_Video;

    //音视频同步
    double m_videoClock;

    //帧回调
    VideoCallback* m_pVideoCallback;
};

#endif // VIDEOENCODE_H
