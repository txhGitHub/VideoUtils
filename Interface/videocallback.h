#ifndef VIDEODECODE_H
#define VIDEODECODE_H

#include <stdio.h>
#include <stdint.h>
#include "videoframe.h"

class VideoCallback
{
public:
    virtual void onDisplayVideo(std::shared_ptr<VideoFrame> videoFrame) = 0;
};

#endif // VIDEODECODE_H
