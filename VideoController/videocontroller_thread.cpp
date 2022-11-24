#include "videocontroller.h"
#include "log.h"
#include "utils.h"

void VideoController::decodeVideoThread()
{
    DEBUG_INFO("start decodeVideoThread");
    fflush(stdout);
    AVFrame *pFrame = nullptr;
    AVFrame *pFrameYUV = nullptr;
    uint8_t *yuv420pBuffer = nullptr; //解码后的yuv数据
    struct SwsContext *imgConvertCtx = nullptr;  //用于解码后的视频格式转换

    AVCodecContext *pCodecCtx = m_pVideoCodecCtx; //视频解码器
    pFrame = av_frame_alloc();

    double video_pts = 0; //当前视频的pts
    double audio_pts = 0; //音频pts

    int videoWidth  = 0;
    int videoHeight = 0;

    while (1) {

        m_pConditon_Video->Lock();
        if (mVideoPacktList.size() <= 0)
        {
            mSleep(1); //队列只是暂时没有数据而已
            continue;
        }
        AVPacket pkt1 = mVideoPacktList.front();
        mVideoPacktList.pop_front();

        m_pConditon_Video->Unlock();
        AVPacket *packet = &pkt1;

        //收到这个数据 说明刚刚执行过跳转 现在需要把解码器的数据 清除一下
        if(strcmp((char*)packet->data, FLUSH_DATA) == 0)
        {
            avcodec_flush_buffers(m_pVideoCodecCtx);
            av_packet_unref(packet);
            continue;
        }


        if (avcodec_send_packet(m_pVideoCodecCtx, packet) != 0)
        {
           DEBUG_INFO("input AVPacket to decoder failed!\n");
           av_packet_unref(packet);
           continue;
        }

        while(0 == avcodec_receive_frame(pCodecCtx, pFrame))
        {
            if(pFrame->pts == AV_NOPTS_VALUE && pFrame->opaque && *(int64_t*)pFrame->opaque != AV_NOPTS_VALUE)
            {
                video_pts = *(uint64_t *) pFrame->opaque;
            }
            else if( packet->dts != AV_NOPTS_VALUE )
            {
                video_pts = packet->dts;
            }
            else
            {
                video_pts = 0;
            }

            //将pts转为以秒为单位的值。
            video_pts *= av_q2d(m_pVideoStream->time_base);
            m_videoClock = video_pts;

            if (pFrame->width != videoWidth || pFrame->height != videoHeight)
            {
                videoWidth  = pFrame->width;
                videoHeight = pFrame->height;

                if (pFrameYUV != nullptr)
                {
                    av_free(pFrameYUV);
                }

                if (yuv420pBuffer != nullptr)
                {
                    av_free(yuv420pBuffer);
                }

                if (imgConvertCtx != nullptr)
                {
                    sws_freeContext(imgConvertCtx);
                }
                pFrameYUV = av_frame_alloc();

                int yuvSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, videoWidth, videoHeight, 1);  //按1字节进行内存对齐,得到的内存大小最接近实际大小???
                unsigned int numBytes = static_cast<unsigned int>(yuvSize);
                yuv420pBuffer = static_cast<uint8_t *>(av_malloc(numBytes * sizeof(uint8_t)));
                av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, yuv420pBuffer, AV_PIX_FMT_YUV420P, videoWidth, videoHeight, 1);
                ///由于解码后的数据不一定都是yuv420p，因此需要将解码后的数据统一转换成YUV420P
                imgConvertCtx = sws_getContext(videoWidth, videoHeight,
                        (AVPixelFormat)pFrame->format, videoWidth, videoHeight,
                        AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
            }

            sws_scale(imgConvertCtx,
                    (uint8_t const * const *) pFrame->data,
                    pFrame->linesize, 0, videoHeight, pFrameYUV->data,
                    pFrameYUV->linesize);
            mSleep(30);
            doDisplayVideo(yuv420pBuffer, videoWidth, videoHeight);
        }
        av_packet_unref(packet);
    }

    av_free(pFrame);
    if (pFrameYUV != nullptr)
    {
        av_free(pFrameYUV);
    }

    if (yuv420pBuffer != nullptr)
    {
        av_free(yuv420pBuffer);
    }

    if (imgConvertCtx != nullptr)
    {
        sws_freeContext(imgConvertCtx);
    }
}

void VideoController::doDisplayVideo(const uint8_t *yuv420Buffer, const int &width, const int &height)
{
    if(m_pVideoCallback != nullptr)
    {
         std::shared_ptr<VideoFrame> videoFrame = std::make_shared<VideoFrame>();
         VideoFrame * ptr = videoFrame.get();
         ptr->initBuffer(width, height);
         ptr->setYUVbuf(yuv420Buffer);
//         mSleep(30);
         m_pVideoCallback->onDisplayVideo(videoFrame);
    }
}
