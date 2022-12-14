#include "videocontroller.h"
#include <stdio.h>
#include <thread>
#include  <direct.h>
#include <string>


//静态变量定义
VideoState VideoController::g_VideoState = {0};

VideoController::VideoController()
{
//    打印结果在缓冲区未被释放，
    DEBUG_INFO("avcodec_version %d", avcodec_version());
    DEBUG_INFO("av_version_info %s", av_version_info());
    fflush(stdout);
    m_pConditon_Video = new Cond;
    mConditon_Audio = new Cond;
    mAudioID = -1;
    av_log_set_level(AV_LOG_DEBUG); //设置日志级别
}

bool VideoController::startPlay(std::string &filePath)
{
    if (!filePath.empty())
        mFilePath = filePath;

    //启动新的线程实现读取视频文件
    std::thread([&](VideoController *pointer) {
        pointer->readVideoFile();

    }, this).detach();

    return true;
}

 int VideoController::readVideoFile()
{
    DEBUG_INFO("xinong readVideoFile");

    int ret = 0; //错误返回到时候再设计
    int audioStreamIdx ,videoStreamIdx;
    m_pInFormatCtx = nullptr;
    m_pInFormatCtx = avformat_alloc_context();
    if ((ret = avformat_open_input(&m_pInFormatCtx, mFilePath.c_str(), 0, NULL)) != 0) {
        DEBUG_INFO("file open error!");
        return ret;
    }

    if ((ret = avformat_find_stream_info(m_pInFormatCtx, NULL)) < 0) {
        DEBUG_INFO("cannot retrive video info");
        return ret;
    }

    audioStreamIdx = -1;
    videoStreamIdx = -1;
    av_dump_format(m_pInFormatCtx, 0, mFilePath.c_str(), 0);
    for (unsigned int i = 0; i < m_pInFormatCtx->nb_streams; i++) {
        if (m_pInFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIdx = i;
        }

        if (m_pInFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO  && audioStreamIdx < 0) {
            audioStreamIdx = i;
        }
    }
    //打开视频解码器，并启动视频线程
   if(!beforeVideoDecode(videoStreamIdx)) {
        DEBUG_INFO("beforeVideoDecode call failed!");
        return ret;
   }

   if(!beforeAudioDecode(audioStreamIdx)) {
        DEBUG_INFO("beforeAudioDecode call failed!");
        return ret;
   }

   while (1) {
       //这里做了个限制  当队列里面的数据超过某个大小的时候 就暂停读取  防止一下子就把视频读完了，导致的空间分配不足
       //这个值可以稍微写大一些

       if (mAudioPacktList.size() > MAX_AUDIO_SIZE || mVideoPacktList.size() > MAX_VIDEO_SIZE) {
           mSleep(10);
           continue;
       }

       AVPacket packet;
       if ((ret = av_read_frame(m_pInFormatCtx, &packet)) < 0) {
           if(ret == AVERROR_EOF) {
                DEBUG_INFO("the decoder has been fully flushed!");
                break;
           }
           else if(ret == AVERROR(EAGAIN)) {
                DEBUG_INFO("return error from library functions");
                 break;
           }
           else {
                continue;
           }
       }

       if (packet.stream_index == videoStreamIdx) {

           inputVideoQuene(packet);
           //这里我们将数据存入队列 因此不调用 av_free_packet 释放
       }
       else if( packet.stream_index == audioStreamIdx) {

           inputAudioQuene(packet);
                           //这里我们将数据存入队列 因此不调用 av_free_packet 释放
       }
       else {
           // Free the packet that was allocated by av_read_frame
           av_packet_unref(&packet);
       }
   }
   closeSDL();
   //可以设计编码后的函数，释放相关的指针
   ret = 1;
   return ret;
}

bool VideoController::inputVideoQuene(const AVPacket &pkt)
{
    /**
     *AVPacket本身只是容器，它本身并不包含压缩的媒体数据，而是通过data指针引用数据的缓存空间。
     *使用av_dup_avpacket大致原因是：循环调用av_read_frame,会将packet中的data释放掉,导致Packet->buf为空，
     *所以一般情况下调用av_dup_avpacket重新复制一份缓存数据，保证读取下一帧的时候，data引用没有被释放掉。???
     * */
    AVPacket* dst = av_packet_alloc();
    if (av_packet_ref(dst ,(AVPacket*) &pkt) < 0) {
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

int VideoController::openSDL()
{
    ///打开SDL，并设置播放的格式为:AUDIO_S16LSB 双声道，44100hz
    ///后期使用ffmpeg解码完音频后，需要重采样成和这个一样的格式，否则播放会有杂音
    SDL_AudioSpec wanted_spec, spec;
    wanted_spec.freq = m_outSampleRate;
    wanted_spec.channels = 2; //SDL仅接收一到两个通道
    wanted_spec.samples = FFMAX(512, 2 << av_log2(wanted_spec.freq / 30));//？
    wanted_spec.format = AUDIO_S16LSB; //AUDIO_S16LSB，LSB小端模式，S有符号，U无符号

    wanted_spec.silence = 0;            // 0指示静音
    wanted_spec.callback = sdlAudioCallBackFunc;  // 回调函数
    wanted_spec.userdata = this;                  // 传给上面回调函数的外带数据？

    //出现有多个音频设置时，默认选择一个，而那个设置未接入音响，导致不出声，选择特定的设置，如果没有找到，再去随便选择一个
    int num = SDL_GetNumAudioDevices(0);
     for (int i=0;i < num;i++) {
        std::string device = SDL_GetAudioDeviceName(i,0);
        if(device.find("EDIFIER") != std::string :: npos) {
            mAudioID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(i,0), false, &wanted_spec, &spec,
                                            SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
        }
        DEBUG_INFO("mAudioID %d %s", mAudioID ,SDL_GetAudioDeviceName(i,0));
     }

     if(mAudioID < 0) {
         DEBUG_INFO("SDL_GetError %s", SDL_GetError() );
     }

     for (int i=0;(i<num) && (mAudioID < 0);i++) {
         mAudioID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(i,0), false, &wanted_spec, &spec,0);
         if (mAudioID > 0) {
             break;
         }
     }

     if (mAudioID < 0) {
         mIsAudioThreadFinished = true;
         return -1;
     }
    DEBUG_INFO("mAudioID=%d\n", mAudioID);
    return 0;
}

void VideoController::closeSDL()
{
    if (mAudioID >= 0) {
        SDL_LockAudioDevice(mAudioID);
        SDL_PauseAudioDevice(mAudioID, 1);
        SDL_UnlockAudioDevice(mAudioID);
        SDL_CloseAudioDevice(mAudioID);
    }

    mAudioID = 0;
}

double VideoController::calculateDelay(AVPacket *pkt)
{
    //音视频同步,待改善
    double sync_threshold, diff = 0;
    //解码前后前后的pts,解码前的pts是以AVStream.time_base为基准
    //计算上一帧音频与视频的差值
    double delay = 0.0;
    diff =  ((g_VideoState.last_vframe_pts* av_q2d(m_pVideoStream->time_base)) -
             (g_VideoState.last_aframe_pts  * av_q2d(m_pAudioStream->time_base)));
    delay = ((pkt->pts - g_VideoState.last_vframe_pts) * av_q2d(m_pVideoStream->time_base));

    DEBUG_INFO("xinhong  diff:%f ", diff);

    sync_threshold = AV_SYNC_THRESHOLD_MAX;
    if (diff < -AV_SYNC_THRESHOLD_MIN)
        delay = FFMAX(0, delay + diff);
    else if ( diff > AV_SYNC_THRESHOLD_MIN && diff < AV_SYNC_THRESHOLD_MAX)
        delay = delay + diff;
    else if(diff >= AV_SYNC_THRESHOLD_MAX)
        delay = delay*2;
    return delay;
}

bool VideoController::beforeVideoDecode(int streamId)
{
    m_pVideoCodecCtx = nullptr;
    m_pVidoeCodec = nullptr;
    m_pVideoCodecPar = nullptr;
    bool result = false;

    do {
        if (streamId >= 0) {
            m_pVideoCodecPar = m_pInFormatCtx->streams[streamId]->codecpar;
            m_pVidoeCodec = avcodec_find_decoder(m_pVideoCodecPar->codec_id);

            if (m_pVidoeCodec == nullptr) {
                DEBUG_INFO("PCodec not found.\n");
                break;
            }
            m_pVideoCodecCtx = avcodec_alloc_context3(m_pVidoeCodec);
            avcodec_parameters_to_context(m_pVideoCodecCtx, m_pVideoCodecPar);
            if (avcodec_open2(m_pVideoCodecCtx, m_pVidoeCodec, NULL) < 0) { //打开视频解码器
                DEBUG_INFO("Could not open video codec.\n");
                break;
            }

            m_pVideoStream = m_pInFormatCtx->streams[streamId];

            ///创建一个线程专门用来解码视频
            std::thread([&](VideoController *pointer) {
                pointer->decodeVideoThread();

            }, this).detach();
        }
        result = true;
    } while(0);
    return result;
}

bool VideoController::beforeAudioDecode(int streamId)
{
    m_pAudioCodecPar = nullptr;
    m_pAFrame_ReSample =nullptr;
    m_pSwrCtx = nullptr;
    out_buffer[0] = (uint8_t *) av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE * 2);
    out_buffer[1] = (uint8_t *) av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE * 2);

    bool result = false;
    do {
        ///SDL初始化需要放入子线程中，否则有些电脑会有问题。
        if (SDL_Init(SDL_INIT_AUDIO)) {
            DEBUG_INFO("Could not initialize SDL - %s. \n", SDL_GetError());
            break;
        }

        if(streamId >= 0) {
            //nb_samples:一般指，在一个通道内，每一帧的采样率，一般情况是固定的，如MP3为1152，AAC为1024。
            //实际上ffmpeg音频方面很多计算都与该数据有关，实际开发中记得注意该值
            ///查找音频解码器
            m_pAudioCodecPar = m_pInFormatCtx->streams[streamId]->codecpar;
            m_pAudioCodec = avcodec_find_decoder(m_pAudioCodecPar->codec_id);
            if (m_pAudioCodec == nullptr) {
                DEBUG_INFO("AudioCodec not found.\n");
                break;
            }

            m_pAudioCodecCtx = avcodec_alloc_context3(m_pAudioCodec);
            avcodec_parameters_to_context(m_pAudioCodecCtx, m_pAudioCodecPar);
            if (avcodec_open2(m_pAudioCodecCtx, m_pAudioCodec, NULL) < 0) { //打开视频解码器
                DEBUG_INFO("Could not open video codec.\n");
                break;
            }
            m_pAudioStream = m_pInFormatCtx->streams[streamId];

            //frame->16bit 44100 PCM 统一音频采样格式与采样率
            m_pAudioFrame = av_frame_alloc();
            //重采样的设置的输出格式是否需要和SDL设置的期盼格式保持一致？
            //swr_alloc_set_opts为ffmpeg设置，SDL为播放驱动设置，因此，输出二者需要保持一致
            //输入的声道布局
            int64_t in_channel_layout = m_pAudioCodecCtx->channel_layout;
            if (in_channel_layout <= 0) {
                in_channel_layout = av_get_default_channel_layout(m_pAudioCodecCtx->channels);
            }
            //输出
            m_outFormat = AV_SAMPLE_FMT_S16;//输出格式S16
            m_outSampleRate =  m_pAudioCodecCtx->sample_rate; //输出采样率
            m_outChannelLayout = AV_CH_LAYOUT_STEREO;
            DEBUG_INFO("xinhong  sample_fmt: %d\n", m_pAudioCodecCtx->sample_fmt);
            /// 2019-5-13添加
            /// wav/wmv 文件获取到的aCodecCtx->channel_layout为0会导致后面的初始化失败，因此这里需要加个判断。

            m_pSwrCtx = swr_alloc_set_opts(m_pSwrCtx, m_outChannelLayout, m_outFormat, m_outSampleRate,
                                                 in_channel_layout, m_pAudioCodecCtx->sample_fmt, m_pAudioCodecCtx->sample_rate, 0, nullptr);

            int ret = swr_init(m_pSwrCtx); //设置了参数就需要调用一初始化

            if(ret < 0) {
                char buff[128]={0};
                av_strerror(ret, buff, 128);
                DEBUG_INFO("Could not open resample context: %s\n", buff);
                swr_free(&m_pSwrCtx);
                m_pSwrCtx = nullptr;
                break;
            }
            ///打开SDL播放声音
            int code = openSDL();

            if (code == 0) {
                SDL_LockAudioDevice(mAudioID);
                SDL_PauseAudioDevice(mAudioID,0);
                SDL_UnlockAudioDevice(mAudioID);
                mIsAudioThreadFinished = false;
            }
        }
        result = true;
    } while(0);
    return result;
}
