/*
 * MultimediaManager.h
 *****************************************************************************
 * Copyright (C) 2013, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#ifndef QTSAMPLEPLAYER_MANAGERS_MULTIMEDIAMANAGER_H_
#define QTSAMPLEPLAYER_MANAGERS_MULTIMEDIAMANAGER_H_

#include "IMPD.h"
#include "MultimediaStream.h"
#include "IMultimediaManagerObserver.h"
#include "../libdashframework/Adaptation/IAdaptationLogic.h"
#include "../libdashframework/Adaptation/AdaptationLogicFactory.h"
#include "../Renderer/QTGLRenderer.h"
#include "../Renderer/QTAudioRenderer.h"
#include "../libdashframework/Portable/MultiThreading.h"
#include "../libdashframework/Buffer/AudioChunk.h"
#include <QtMultimedia/qaudiooutput.h>
#include <fstream>
using namespace std;

namespace sampleplayer
{
    namespace managers
    {
        class MultimediaManager : public IStreamObserver
        {
            public:
                MultimediaManager           (renderer::QTGLRenderer *videoElement, renderer::QTAudioRenderer *audioElement);
                virtual ~MultimediaManager  ();

                bool                Init    (const std::string& url);
                void                Start   (int progress=0);
                void                Stop    ();
                dash::mpd::IMPD*    GetMPD  ();

				void  SetSegmentDisplayIndex		(uint32_t segmentDisplayIndex);			//2015.4.12 - php
				uint32_t  GetSegmentTotalSize     ();			//2015.4.12 - php
				void  SetProgress    (uint32_t  segmentDisplayIndex);			//2015.4.12 - php
                bool SetVideoQuality      (dash::mpd::IPeriod* period, dash::mpd::IAdaptationSet *adaptationSet, dash::mpd::IRepresentation *representation);
                bool SetAudioQuality      (dash::mpd::IPeriod* period, dash::mpd::IAdaptationSet *adaptationSet, dash::mpd::IRepresentation *representation);

                bool SetVideoAdaptationLogic    (libdash::framework::adaptation::LogicType type);
                bool SetAudioAdaptationLogic    (libdash::framework::adaptation::LogicType type);

                void AttachManagerObserver  (IMultimediaManagerObserver *observer);

                void SetFrameRate               (double frameRate);

                /* IStreamObserver */
                void OnSegmentDownloaded        ();
                void OnSegmentBufferStateChanged(StreamType type, uint32_t fillstateInPercent);
                void OnVideoBufferStateChanged  (uint32_t fillstateInPercent);
                void OnAudioBufferStateChanged  (uint32_t fillstateInPercent);
				void OnRateChanged 				(int segmentNumber, uint32_t downloadRate);


            private:
                dash::IDASHManager                                          *manager;
                dash::mpd::IMPD                                             *mpd;
                renderer::QTGLRenderer                                      *videoElement;
                renderer::QTAudioRenderer                                   *audioElement;
                dash::mpd::IPeriod                                          *period;
                dash::mpd::IAdaptationSet                                   *videoAdaptationSet;
                dash::mpd::IRepresentation                                  *videoRepresentation;
                libdash::framework::adaptation::IAdaptationLogic            *videoLogic;
                MultimediaStream                                            *videoStream;
                dash::mpd::IAdaptationSet                                   *audioAdaptationSet;
                dash::mpd::IRepresentation                                  *audioRepresentation;
                libdash::framework::adaptation::IAdaptationLogic            *audioLogic;
                MultimediaStream                                            *audioStream;
                std::vector<IMultimediaManagerObserver *>                   managerObservers;
                bool                                                        isStarted;
                uint64_t                                                    framesDisplayed;
                uint64_t                                                    segmentsDownloaded;
				uint32_t													segmentDisplayIndex;			//2015.4.10 - php
				uint32_t													audioSegmentIndex;				//2015.4.18 - php

                double											            OfillstateInPercent ;
				double   													Orate ;//缓存下载预期起始目标速率
				double    												    Yrate ;//自主添加的平滑处理结果记录器
				double     												    NextT;
				uint32_t													R ;

                CRITICAL_SECTION                                            monitorMutex;
                double                                                      frameRate;
				 libdash::framework::input::Logger							*logger;

                THREAD_HANDLE                                               videoRendererHandle;
                THREAD_HANDLE                                               audioRendererHandle;
                bool                                                        isVideoRendering;
                bool                                                        isAudioRendering;

                /* Threads for Rendering Audio & Video */
                bool            StartVideoRenderingThread   ();
                void            StopVideoRenderingThread    ();
                static void*    RenderVideo                 (void *data);

                bool            StartAudioRenderingThread   ();
                void            StopAudioRenderingThread    ();
                static void*    RenderAudio                 (void *data);

                void    InitVideoRendering  (uint32_t offset);
                void    InitAudioPlayback   (uint32_t offset);
                void    StopVideo           ();
                void    StopAudio           ();

                /* IMultimediaManager Notifiers */
                void NotifyVideoBufferObservers         (uint32_t fillstateInPercent);
                void NotifyVideoSegmentBufferObservers  (uint32_t fillstateInPercent);
                void NotifyAudioBufferObservers         (uint32_t fillstateInPercent);
                void NotifyAudioSegmentBufferObservers  (uint32_t fillstateInPercent);

				void NotifySegmentDisplayIndexChanged	(uint32_t segmentDisplayIndex);			//2015.4.10 - php
				void NotifyAudioSegmentIndexChanged   (uint32_t segmentDisplayIndex);				//2015.4.18 - php

				void NotifyProgressCompleted();				//2015.4.15 - php
				 void NotifyRateChanged					(int segmentNumber, uint32_t downloadRate);
        };
    }
}

#endif /* QTSAMPLEPLAYER_MANAGERS_MULTIMEDIAMANAGER_H_ */