#include "videocontroller.h"
#include <stdio.h>
#include <thread>
#include "log.h"
#include "utils.h"

VideoController::VideoController()
{
//    打印结果在缓冲区未被释放，
    DEBUG_INFO("avcodec_version %d", avcodec_version());
    DEBUG_INFO("av_version_info %s", av_version_info());
    fflush(stdout);
    m_pConditon_Video = new Cond;
}

bool VideoController::startPlay(std::string &filePath)
{
    if (!filePath.empty())
        mFilePath = filePath;

    //启动新的线程实现读取视频文件
    std::thread([&](VideoController *pointer)
    {
        pointer->readVideoFile();

    }, this).detach();

    return true;
}

void VideoController::readVideoFile()
{
//    avcodec_register_all();
    DEBUG_INFO("xinong readVideoFile");

    int ret = 0;
    int audioStreamIdx ,videoStreamIdx;
    m_pInFormatCtx = nullptr;
    m_pVideoCodecCtx = nullptr;
    m_pavCodecParameters = nullptr;
    m_pCodec = nullptr;

    do {
        m_pInFormatCtx = avformat_alloc_context();
        if ((ret = avformat_open_input(&m_pInFormatCtx, mFilePath.c_str(), 0, NULL)) != 0) {
            DEBUG_INFO("file open error!");
            break;
        }

        if ((ret = avformat_find_stream_info(m_pInFormatCtx, NULL)) < 0) {
            DEBUG_INFO("cannot retrive video info");
            break;
        }

        audioStreamIdx = -1;
        videoStreamIdx = -1;
        av_dump_format(m_pInFormatCtx, 0, mFilePath.c_str(), 0);
        for (unsigned int i = 0; i < m_pInFormatCtx->nb_streams; i++) {
            if (m_pInFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStreamIdx = i;
            }

            if (m_pInFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO  && audioStreamIdx < 0)
            {
                audioStreamIdx = i;
            }
        }

        //打开视频解码器，并启动视频线程
        if (videoStreamIdx >= 0)
        {
            m_pVideoCodecPar = m_pInFormatCtx->streams[videoStreamIdx]->codecpar;
            DEBUG_INFO("codec_id %d\n", m_pVideoCodecPar->codec_id);
            m_pCodec = avcodec_find_decoder(m_pVideoCodecPar->codec_id);

            if (m_pCodec == nullptr)
            {
                DEBUG_INFO("PCodec not found.\n");
                break;
            }
            m_pVideoCodecCtx = avcodec_alloc_context3(m_pCodec);
            avcodec_parameters_to_context(m_pVideoCodecCtx, m_pVideoCodecPar);
            ///打开视频解码器
            if (avcodec_open2(m_pVideoCodecCtx, m_pCodec, NULL) < 0)
            {
                DEBUG_INFO("Could not open video codec.\n");
                break;
            }

            m_pVideoStream = m_pInFormatCtx->streams[videoStreamIdx];

            ///创建一个线程专门用来解码视频
            std::thread([&](VideoController *pointer)
            {
                pointer->decodeVideoThread();

            }, this).detach();
        }

        while (1) {
            if (mAudioPacktList.size() > MAX_AUDIO_SIZE || mVideoPacktList.size() > MAX_VIDEO_SIZE)
            {
                mSleep(10);
                continue;
            }

            AVPacket packet;
            if (av_read_frame(m_pInFormatCtx, &packet) < 0)
            {
                DEBUG_INFO("Video data reading completed .\n");
                mSleep(10);
                continue;
            }

            if (packet.stream_index == videoStreamIdx)
            {
                inputVideoQuene(packet);
                //这里我们将数据存入队列 因此不调用 av_free_packet 释放
            }
        }
    }
    while (0);
}


bool VideoController::inputVideoQuene(const AVPacket &pkt)
{
    /**
     *AVPacket本身只是容器，它本身并不包含压缩的媒体数据，而是通过data指针引用数据的缓存空间。
     *使用av_dup_avpacket大致原因是：循环调用av_read_frame,会将packet中的data释放掉,导致Packet->buf为空，
     *所以一般情况下调用av_dup_avpacket重新复制一份缓存数据，保证读取下一帧的时候，data引用没有被释放掉。???
     * */
    AVPacket* dst = av_packet_alloc();
    if (av_packet_ref(dst ,(AVPacket*) &pkt) < 0)
    {
        av_packet_unref(dst);
        return false;
    }

    m_pConditon_Video->Lock();
    mVideoPacktList.push_back(*dst);
    m_pConditon_Video->Signal();
    m_pConditon_Video->Unlock();

    return true;
}

void VideoController::setVideoCallback(VideoCallback *vcb)
{
    m_pVideoCallback = vcb;
}
