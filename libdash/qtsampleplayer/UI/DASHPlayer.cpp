/*
 * DASHPlayer.cpp
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#include "DASHPlayer.h"
#include <iostream>

using namespace libdash::framework::adaptation;
using namespace libdash::framework::mpd;
using namespace libdash::framework::buffer;
using namespace sampleplayer;
using namespace sampleplayer::renderer;
using namespace sampleplayer::managers;
using namespace dash::mpd;
using namespace std;

DASHPlayer::DASHPlayer  (QtSamplePlayerGui &gui) :
            gui         (&gui)
{
    InitializeCriticalSection(&this->monitorMutex);

    this->SetSettings(0, 0, 0, 0, 0);
    this->videoElement      = gui.GetVideoElement();
    this->audioElement      = new QTAudioRenderer(&gui);
    this->multimediaManager = new MultimediaManager(this->videoElement, this->audioElement);

    this->multimediaManager->SetFrameRate(24);
    this->multimediaManager->AttachManagerObserver(this);
    this->gui->AddWidgetObserver(this);

    QObject::connect(this, SIGNAL(VideoSegmentBufferFillStateChanged(int)), &gui, SLOT(SetVideoSegmentBufferFillState(int)));
    QObject::connect(this, SIGNAL(VideoBufferFillStateChanged(int)), &gui, SLOT(SetVideoBufferFillState(int)));
    QObject::connect(this, SIGNAL(AudioSegmentBufferFillStateChanged(int)), &gui, SLOT(SetAudioSegmentBufferFillState(int)));
    QObject::connect(this, SIGNAL(AudioBufferFillStateChanged(int)), &gui, SLOT(SetAudioBufferFillState(int)));
	//2015.4.10 - php
	QObject::connect(this, SIGNAL(DurationChanged(int)), &gui, SLOT(SetProgressSliderRange	(int)));		
	QObject::connect(this, SIGNAL(ProgressValueChanged(int)), &gui, SLOT(SetProgressSlider(int)));
	QObject::connect(this, SIGNAL(ProgressValueChanged(int)), &gui, SLOT(SetVideoProgressLabel(int)));     //2015.4.18 - php
	QObject::connect(this, SIGNAL(AudioProgressChanged(int)), &gui, SLOT(SetAudioProgressLabel(int)));    //2015.4.18 - php
	QObject::connect(this, SIGNAL(ProgressCompleted()), &gui,  SLOT(SetEndOfProgressSlider()));

	QObject::connect(this, SIGNAL(RateChanged(int, unsigned int)), &gui, SLOT(SetRateChangedLabel(int, unsigned int)));
	QObject::connect(this, SIGNAL(BWChanged(unsigned int)), &gui, SLOT(SetBWChangedLabel(unsigned int)));
}  
DASHPlayer::~DASHPlayer ()
{
    this->multimediaManager->Stop();
    this->audioElement->StopPlayback();
    this->audioElement = NULL;

    delete(this->multimediaManager);
    delete(this->audioElement);

    DeleteCriticalSection(&this->monitorMutex);
}

void DASHPlayer::OnStartButtonPressed               (int period, int videoAdaptationSet, int videoRepresentation, int audioAdaptationSet, int audioRepresentation, int progress)
{
    /*
    std::string url = this->gui->GetUrl();
    if(!this->multimediaManager->Init(url))
    {
        this->gui->SetStatusBar("Error parsing mpd at: " + url);
        return; // TODO dialog or symbol that indicates that error
    }

    this->gui->SetStatusBar("Successfully parsed MPD at: " + url);

    this->OnSettingsChanged(period, videoAdaptationSet, videoRepresentation, audioAdaptationSet, audioRepresentation); */
	if (progress>0)
		this->multimediaManager->Start(progress);
	else
		this->multimediaManager->Start();
}
void DASHPlayer::OnPauseButtonPressed                ()
{
    this->multimediaManager->Stop();
}
void DASHPlayer::OnStopButtonPressed                ()
{
	this->multimediaManager->Stop();
	this->multimediaManager->SetSegmentDisplayIndex(0);
	emit ProgressValueChanged(0);
}

void DASHPlayer::OnSettingsChanged                  (int period, int videoAdaptationSet, int videoRepresentation, int audioAdaptationSet, int audioRepresentation)
{
    if(this->multimediaManager->GetMPD() == NULL)
        return; // TODO dialog or symbol that indicates that error

    if (!this->SettingsChanged(period, videoAdaptationSet, videoRepresentation, audioAdaptationSet, audioRepresentation))
        return;

    IPeriod                         *currentPeriod      = this->multimediaManager->GetMPD()->GetPeriods().at(period);
    std::vector<IAdaptationSet *>   videoAdaptationSets = AdaptationSetHelper::GetVideoAdaptationSets(currentPeriod);
    std::vector<IAdaptationSet *>   audioAdaptationSets = AdaptationSetHelper::GetAudioAdaptationSets(currentPeriod);

    if (videoAdaptationSet >= 0 && videoRepresentation >= 0)
    {
        this->multimediaManager->SetVideoQuality(currentPeriod,
                                                 videoAdaptationSets.at(videoAdaptationSet),
                                                 videoAdaptationSets.at(videoAdaptationSet)->GetRepresentation().at(videoRepresentation));
    }
    else
    {
        this->multimediaManager->SetVideoQuality(currentPeriod, NULL, NULL);
    }

    if (audioAdaptationSet >= 0 && audioRepresentation >= 0)
    {
        this->multimediaManager->SetAudioQuality(currentPeriod,
                                                 audioAdaptationSets.at(audioAdaptationSet),
                                                 audioAdaptationSets.at(audioAdaptationSet)->GetRepresentation().at(audioRepresentation));
    }
    else
    {
        this->multimediaManager->SetAudioQuality(currentPeriod, NULL, NULL);
    }

}
void DASHPlayer::OnVideoBufferStateChanged          (uint32_t fillstateInPercent)
{
    emit VideoBufferFillStateChanged(fillstateInPercent);
}
void DASHPlayer::OnVideoSegmentBufferStateChanged   (uint32_t fillstateInPercent)
{
    emit VideoSegmentBufferFillStateChanged(fillstateInPercent);
}
void DASHPlayer::OnAudioBufferStateChanged          (uint32_t fillstateInPercent)
{
    emit AudioBufferFillStateChanged(fillstateInPercent);
}
void DASHPlayer::OnAudioSegmentBufferStateChanged   (uint32_t fillstateInPercent)
{
    emit AudioSegmentBufferFillStateChanged(fillstateInPercent);
}

//2015.4.10 - php
void	DASHPlayer::OnSegmentDisplayIndexChanged		(uint32_t segmentDisplayIndex)
{
	emit ProgressValueChanged(segmentDisplayIndex);
}
//2015.4.18 - php
void DASHPlayer::OnAudioSegmentIndexChanged (uint32_t audioSegmentIndex)
{
	emit AudioProgressChanged(audioSegmentIndex);
}

void  DASHPlayer::OnProgressCompleted  ()
{
	emit ProgressCompleted();
}

void DASHPlayer::OnRateChanged (int segmentNumber, uint32_t downloadRate){
	//std::cout << "MMPlayer #" << segmentNumber << std::endl;
	emit RateChanged(segmentNumber, downloadRate);
}

void DASHPlayer::OnBWChanged (uint32_t BW){
	//std::cout << "MMPlayerBW #" << BW << std::endl;
	emit BWChanged(BW);
}

//2015.4.13 - php
void  DASHPlayer::OnProgressSliderReleased (int progress)
{
	this->multimediaManager->SetProgress(progress);
	this->multimediaManager->Start();
}
void  DASHPlayer::OnProgressSliderPressed() 
{
	this->multimediaManager->Stop();
}

void DASHPlayer::OnDownloadMPDPressed               (const std::string &url)
{
    if(!this->multimediaManager->Init(url))
    {
        this->gui->SetStatusBar(QString::fromLocal8Bit("解析错误：") + QString(url.c_str()));
        return; // TODO dialog or symbol that indicates that error
    }

	//2015.4.12 - php
	int segmentTotalSize = this->multimediaManager->GetSegmentTotalSize();
	emit DurationChanged(segmentTotalSize);
	
	this->multimediaManager->SetSegmentDisplayIndex(0);			//2015.4.12 - php
	this->OnSegmentDisplayIndexChanged(0);				//2015.4.12 - php - 防止进度条重新开始时反应慢一拍
	//int segmentTotalSize = this->multimediaManager->GetMPD()->GetPeriods().at(0)->GetAdaptationSets().at(0)->GetRepresentation().at(0)->GetSegmentList()->GetSegmentURLs().size();
	
    this->SetSettings(-1, -1, -1, -1, -1);
    this->gui->SetStatusBar(QString::fromLocal8Bit("成功解析MPD: ") + QString(url.c_str()));
    this->gui->SetGuiFields(this->multimediaManager->GetMPD());
}
bool DASHPlayer::SettingsChanged                    (int period, int videoAdaptationSet, int videoRepresentation, int audioAdaptationSet, int audioRepresentation)
{
    EnterCriticalSection(&this->monitorMutex);

    bool settingsChanged = false;

    if (this->currentSettings.videoRepresentation != videoRepresentation ||
        this->currentSettings.audioRepresentation != audioRepresentation ||
        this->currentSettings.videoAdaptationSet != videoAdaptationSet ||
        this->currentSettings.audioAdaptationSet != audioAdaptationSet ||
        this->currentSettings.period != period)
        settingsChanged = true;

    if (settingsChanged)
        this->SetSettings(period, videoAdaptationSet, videoRepresentation, audioAdaptationSet, audioRepresentation);

    LeaveCriticalSection(&this->monitorMutex);

    return settingsChanged;
}
void DASHPlayer::SetSettings                        (int period, int videoAdaptationSet, int videoRepresentation, int audioAdaptationSet, int audioRepresentation)
{
    this->currentSettings.period                = period;
    this->currentSettings.videoAdaptationSet    = videoAdaptationSet;
    this->currentSettings.videoRepresentation   = videoRepresentation;
    this->currentSettings.audioAdaptationSet    = audioAdaptationSet;
    this->currentSettings.audioRepresentation   = audioRepresentation;
}
