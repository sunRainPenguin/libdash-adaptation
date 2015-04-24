/*
 * IMultimediaManagerObserver.h
 *****************************************************************************
 * Copyright (C) 2013, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#ifndef QTSAMPLEPLAYER_MANAGERS_IMULTIMEDIAMANAGEROBSERVER_H_
#define QTSAMPLEPLAYER_MANAGERS_IMULTIMEDIAMANAGEROBSERVER_H_

#include <stdint.h>

namespace sampleplayer
{
    namespace managers
    {
        class IMultimediaManagerObserver
        {
            public:
                virtual ~IMultimediaManagerObserver () {}

				virtual void OnRateChanged		        (int segmentNumber, uint32_t downloadRate) = 0;
				virtual void OnBWChanged		        		(uint32_t BW) = 0;
				virtual void OnSegmentDisplayIndexChanged		(uint32_t segmentDisplayIndex) = 0;			//2015.4.10 - php
				virtual void OnAudioSegmentIndexChanged (uint32_t segmentDisplayIndex) = 0;			//2015.4.18 - php
				virtual void OnProgressCompleted     () = 0;			//2015.4.15 - php

                virtual void OnVideoBufferStateChanged          (uint32_t fillstateInPercent) = 0;
                virtual void OnVideoSegmentBufferStateChanged   (uint32_t fillstateInPercent) = 0;
                virtual void OnAudioBufferStateChanged          (uint32_t fillstateInPercent) = 0;
                virtual void OnAudioSegmentBufferStateChanged   (uint32_t fillstateInPercent) = 0;
                // todo subtitles
        };
    }
}
#endif /* QTSAMPLEPLAYER_MANAGERS_IMULTIMEDIAMANAGEROBSERVER_H_ */
