/*
 * IAudioObserver.h
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#ifndef IAUDIOOBSERVER_H_
#define IAUDIOOBSERVER_H_

#include <stdint.h>

namespace sampleplayer
{
    namespace decoder
    {
        struct audioFrameProperties
        {
            int     streamIndex;
            int     linesize;
            int     sampleRate;
            int     channels;
            int     samples;
            int     chunkSize;
            bool    fireError;
            char*   errorMessage;
			uint32_t		segmentIndex;			//2015.4.18 - php
			bool				isLastFrame;					//2015.4.18 - php
        };

        class  IAudioObserver
        {
            public:
                virtual ~IAudioObserver () {}

                virtual void OnAudioDataAvailable (const uint8_t **data, audioFrameProperties* props) = 0;
        };
    }
}

#endif /* IAUDIOOBSERVER_H_ */
