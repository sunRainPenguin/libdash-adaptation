/*
 * LibavDecoder.h
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#ifndef LIBAVDECODER_H_
#define LIBAVDECODER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>

#include "../libdashframework/Input/IDataReceiver.h"
#include "../libdashframework/Portable/MultiThreading.h"
#include "IVideoObserver.h"
#include "IAudioObserver.h"

namespace sampleplayer
{
    namespace decoder
    {
        struct StreamConfig
        {
            AVStream       *stream;
            AVCodecContext *codecContext;
            int             frameCnt;
        };
		//2015.4.8 - php
		struct FrameConfig
		{
			AVFrame                 *frame;
			uint32_t					segmentIndex;		
			bool							isLastFrame;			//2015.4.14 - php
		};

        class LibavDecoder
        {
            public:
                LibavDecoder                (libdash::framework::input::IDataReceiver *rec);
                virtual ~LibavDecoder       ();

                bool Decode                 (uint32_t segmentIndex);			//2015.4.8 - php
                bool Init                   ();
                void Stop                   ();
                void AttachVideoObserver    (IVideoObserver *observer);
                void AttachAudioObserver    (IAudioObserver *observer);
                void Flush                  ();

            private:
                libdash::framework::input::IDataReceiver    *receiver;
                std::vector <StreamConfig>                  streamconfigs;
                std::vector <IVideoObserver*>               videoObservers;
                std::vector <IAudioObserver*>               audioObservers;
                unsigned char                               *iobuffer;
                int                                         bufferSize;
                AVFormatContext                             *avFormatContextPtr;
                //AVFrame                                     *frame;
				FrameConfig								   frameconfig;			//2015.4.8 - php
                AVPacket                                    avpkt;
                bool                                        errorHappened;

                AVFormatContext*    OpenInput       ();
                void                InitStreams     (AVFormatContext *ctx);
                StreamConfig*       GetNextPacket   (AVFormatContext *ctx, AVPacket *pkt, int segmentIndex);
                int                 DecodeMedia     (AVFrame *frame, AVPacket *pkt, StreamConfig *streamCfg, int *gotFrame);
                int                 DecodeFrame     (FrameConfig  *frameconfig, AVPacket *pkt, StreamConfig *streamCfg);			//2015.4.8 - php
                void                FreeConfigs     ();
                void                Error           (std::string errormsg, int errorcode, videoFrameProperties *videoProps, audioFrameProperties *audioProps);
                void                Notify          (FrameConfig  *frameconfig, StreamConfig *config);			//2015.4.9 - php
				void                NotifyVideoLastFrame   (FrameConfig  *frameconfig);					//2015.4.15 - php
                void                NotifyVideo     (FrameConfig  *frameconfig, StreamConfig *config);		//2015.4.9 - php
                void                NotifyAudio     (FrameConfig  *frameconfig, StreamConfig *config);		//2015.4.18 - php
                StreamConfig*       FindStreamConfig(int streamIndex);

        };
    }
}
#endif /* LIBAVDECODER_H_ */
