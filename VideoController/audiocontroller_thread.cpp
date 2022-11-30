#include "videocontroller.h"


//c++ 调用动态链接库 回调函数必须是静态函数
//那么就有问题来了，回调函数必须是静态的话，如何调用成员函数和变量
//SDL2,数据播放模式有两种，一种推流式，一种是拉流式，其余看博客
//SDL2 不支持平面音频。在对音频进行排队之前，您需要将平面音频格式重新采样为非平面音频格式
void VideoController::sdlAudioCallBackFunc(void *userdata, Uint8 *stream, int len)
{

    VideoController *controller = (VideoController*)userdata;
    controller->sdlAudioCallBack(stream, len);
}

int VideoController::decodeAudioFrame()
{
    DEBUG_INFO("xinhong  decodeAudioFrame");
    int res;
    int audioBufferSize = 0;
    AVFrame *pFrame = nullptr;
    double audio_clock;

    pFrame = av_frame_alloc();
    do {
        mConditon_Audio->Lock();
        if(mAudioPacktList.size() <= 0) {
             mSleep(10);
             mConditon_Audio->Unlock();
              break;
        }
        AVPacket packet = mAudioPacktList.front();
        mAudioPacktList.pop_front();
        mConditon_Audio->Unlock();

        AVPacket *pkt = &packet;
        /* if update, update the audio clock w/pts */
//        if (pkt->pts != AV_NOPTS_VALUE) {
//            audio_clock = av_q2d(m_pAudioStream->time_base) * pkt->pts;
//        }

        //收到这个数据 说明刚刚执行过跳转 现在需要把解码器的数据 清除一下
        if(strcmp((char*)pkt->data,FLUSH_DATA) == 0) {
            avcodec_flush_buffers(m_pAudioStream->codec);
            av_packet_unref(pkt);
            continue;
        }

        if (avcodec_send_packet(m_pAudioCodecCtx, pkt) != 0) {
           DEBUG_INFO("input AVPacket to decoder failed!\n");
           av_packet_unref(pkt);
           continue;
        }

        //每一帧时间计算为：每一帧样本数*1000ms/采样率，记为：nb_samples
        //进行重采样时，输入输出时间需要一致，也就是一阵的输入输出时间是一致的。因此，
        //重采样之后的采样率计算规则：输入每帧采样率/输入采样率 = 输出每帧采样率/输出采样率
        //如果发生了格式的变化，仅会导致文件体积变化，对采样率或者每帧样本数不影响。
        if(0 == (res= avcodec_receive_frame(m_pAudioCodecCtx, pFrame))) {
         m_outNB_Sample = av_rescale_rnd(swr_get_delay(m_pSwrCtx, m_pAudioCodecCtx->sample_rate) +
                                                    pFrame->nb_samples, m_outSampleRate, m_pAudioCodecCtx->sample_rate, AV_ROUND_UP); //该函数计算：a*b/c
        memset(out_buffer[0],0,AVCODEC_MAX_AUDIO_FRAME_SIZE * 2);
        memset(out_buffer[1],0,AVCODEC_MAX_AUDIO_FRAME_SIZE * 2);
        int len2 = swr_convert(m_pSwrCtx, out_buffer, m_outNB_Sample, (const uint8_t **)pFrame->data, pFrame->nb_samples); //返回的是每个通道的一帧的样本数
        //保存重采样之前的一个声道的数据方法
//        int resampled_data_size = len2 * m_outChannelLayout * av_get_bytes_per_sample(m_outFormat);
        size_t unpadded_linesize = len2 * 2 * av_get_bytes_per_sample(m_outFormat);
        //坑点:ffmpeg设置的平面值与SD不一致导致，音频数据异常。
        //dump音频数据，代码
//        static FILE * fp = fopen("out.pcm", "wb");
//        fwrite(out_buffer[0], 1,  unpadded_linesize, fp);
//        int data_size = av_get_bytes_per_sample(m_outFormat);

        audioBufferSize = unpadded_linesize;
        av_packet_unref(&packet);
        av_free(pFrame);
    }
    } while(0);
    DEBUG_INFO("xinhong audioBufferSize %d", audioBufferSize);
    return audioBufferSize;
}

void VideoController::sdlAudioCallBack(Uint8 *stream, int len)
{
    SDL_memset(stream, 0, len);
    int len1, audio_data_size;
       /*   len是由SDL传入的SDL缓冲区的大小，如果这个缓冲未满，我们就一直往里填充数据 */
       while (len > 0)
       {
           /*  audio_buf_index 和 audio_buf_size 标示我们自己用来放置解码出来的数据的缓冲区，*/
           /*   这些数据待copy到SDL缓冲区， 当audio_buf_index >= audio_buf_size的时候意味着我*/
           /*   们的缓冲为空，没有数据可供copy，这时候需要调用audio_decode_frame来解码出更
            /*   多的桢数据 */
           if (audio_buf_index >= audio_buf_size)
           {
               audio_data_size = decodeAudioFrame();

               /* audio_data_size < 0 标示没能解码出数据，我们默认播放静音 */
               if (audio_data_size <= 0)
               {
                   /* silence */
                   audio_buf_size = 1024;
                   /* 清零，静音 */
                   memset(out_buffer[0], 0, audio_buf_size);
               }
               else
               {
                   audio_buf_size = audio_data_size;
               }
               audio_buf_index = 0;
           }
           /*  查看stream可用空间，决定一次copy多少数据，剩下的下次继续copy */
           len1 = audio_buf_size - audio_buf_index;

           if (len1 > len)
           {
               len1 = len;
           }

           if (out_buffer[0] == nullptr) return;
           memcpy(stream, (uint8_t *)out_buffer[0] + audio_buf_index, len1);

           len -= len1;
           stream += len1;
           audio_buf_index += len1;
       }
}

bool VideoController::inputAudioQuene(const AVPacket &pkt)
{
    AVPacket* dst = av_packet_alloc();
    if (av_packet_ref(dst ,(AVPacket*) &pkt) < 0)
    {
        av_packet_unref(dst);
        return false;
    }

    mConditon_Audio->Lock();
    mAudioPacktList.push_back(*dst);
    mConditon_Audio->Signal();
    mConditon_Audio->Unlock();

    return true;
}
