/*
 * IDASHManagerObserver.h
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#ifndef LIBDASH_FRAMEWORK_INPUT_IDASHMANAGEROBSERVER_H_
#define LIBDASH_FRAMEWORK_INPUT_IDASHMANAGEROBSERVER_H_

#include <QImage>
#include "../Buffer/AudioChunk.h"
#include "../Buffer/VideoFrame.h"
#include "../Buffer/AudioSamples.h"

namespace libdash
{
    namespace framework
    {
        namespace input
        {
            class IDASHManagerObserver
            {
                public:
                    virtual ~IDASHManagerObserver           () {}

                    virtual void AddFrame       (buffer::VideoFrame *frame)                 = 0;			//2015.4.8 - php
                    virtual void AddSamples     (buffer::AudioSamples *samples)   = 0;				//2015.4.18 - php
                    //virtual void AddSubtitle    ()  = 0;
                    virtual void OnSegmentBufferStateChanged   (uint32_t fillstateInPercent) = 0;
					virtual void OnRateChanged (int segmentNumber, uint32_t downloadRate) = 0;
            };
        }
    }
}
#endif /* LIBDASH_FRAMEWORK_INPUT_IDASHMANAGEROBSERVER_H_ */
