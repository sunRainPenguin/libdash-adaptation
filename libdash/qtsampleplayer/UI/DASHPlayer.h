/*
 * DASHPlayer.h
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#ifndef DASHPLAYER_H_
#define DASHPLAYER_H_

#include <iostream>
#include <sstream>
#include <qobject.h>
#include "libdash.h"
#include "IDASHPlayerGuiObserver.h"
#include "../Managers/IMultimediaManagerObserver.h"
#include "../Renderer/QTGLRenderer.h"
#include "../Renderer/QTAudioRenderer.h"
#include "../Managers/MultimediaManager.h"
#include "../libdashframework/Adaptation/AlwaysLowestLogic.h"
#include "../libdashframework/Adaptation/ManualAdaptation.h"
#include "../libdashframework/Buffer/IBufferObserver.h"
#include "../libdashframework/MPD/AdaptationSetHelper.h"

#include <qimage.h>

namespace sampleplayer
{
    struct settings_t
    {
        int period;
        int videoAdaptationSet;
        int audioAdaptationSet;
        int videoRepresentation;
        int audioRepresentation;
    };

    class DASHPlayer : public IDASHPlayerGuiObserver, public managers::IMultimediaManagerObserver

    {
        Q_OBJECT

        public:
            DASHPlayer          (QtSamplePlayerGui& gui);
            virtual ~DASHPlayer ();

            virtual void OnSettingsChanged      (int period, int videoAdaptationSet, int videoRepresentation, int audioAdaptationSet, int audioRepresentation);
            virtual void OnStartButtonPressed   (int period, int videoAdaptationSet, int videoRepresentation, int audioAdaptationSet, int audioRepresentation, int progress=0);
            virtual void OnPauseButtonPressed    ();
			virtual void OnStopButtonPressed    ();

            /* IMultimediaManagerObserver */
            virtual void OnVideoBufferStateChanged          (uint32_t fillstateInPercent);
            virtual void OnVideoSegmentBufferStateChanged   (uint32_t fillstateInPercent);
            virtual void OnAudioBufferStateChanged          (uint32_t fillstateInPercent);
            virtual void OnAudioSegmentBufferStateChanged   (uint32_t fillstateInPercent);
			virtual void OnSegmentDisplayIndexChanged		(uint32_t segmentDisplayIndex);				//2015.4.10 - php
			virtual void OnAudioSegmentIndexChanged (uint32_t segmentDisplayIndex);			//2015.4.18 - php
			virtual void OnProgressCompleted     ();			//2015.4.15 - php
			virtual void OnRateChanged		        		(int segmentNumber, uint32_t downloadRate);
			virtual void OnBWChanged		        		(uint32_t BW);

            virtual void OnDownloadMPDPressed   (const std::string &url);
			virtual void OnProgressSliderReleased (int progress);			//2015.4.13 - php
			virtual void OnProgressSliderPressed ();			//2015.4.13 - php

        private:
            dash::mpd::IMPD                             *mpd;
            sampleplayer::renderer::QTGLRenderer        *videoElement;
            sampleplayer::renderer::QTAudioRenderer     *audioElement;
            QtSamplePlayerGui                           *gui;
            sampleplayer::managers::MultimediaManager   *multimediaManager;
            settings_t                                  currentSettings;
            CRITICAL_SECTION                            monitorMutex;

            bool    SettingsChanged (int period, int videoAdaptationSet, int videoRepresentation, int audioAdaptationSet, int audioRepresentation);
            void    SetSettings     (int period, int videoAdaptationSet, int videoRepresentation, int audioAdaptationSet, int audioRepresentation);

        signals:
            void VideoSegmentBufferFillStateChanged (int fillStateInPercent);
            void VideoBufferFillStateChanged        (int fillStateInPercent);
            void AudioSegmentBufferFillStateChanged (int fillStateInPercent);
            void AudioBufferFillStateChanged        (int fillStateInPercent);
			void RateChanged						(int segmentNumber, unsigned int downloadRate);
			void BWChanged							(unsigned int BW);
			void DurationChanged(int segmentTotalSize);		//2015.4.10 - php
			void ProgressValueChanged(int segmentDisplayIndex);
			void AudioProgressChanged(int audioSegmentIndex);
			void ProgressCompleted();				//2015.4.15 - php

    };
}
#endif /* DASHPLAYER_H_ */
