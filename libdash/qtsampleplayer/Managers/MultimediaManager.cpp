/*
 * MultimediaManager.cpp
 *****************************************************************************
 * Copyright (C) 2013, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/
#include <iostream>
using namespace std;
#include "MultimediaManager.h"

using namespace libdash::framework::adaptation;
using namespace libdash::framework::buffer;
using namespace sampleplayer::managers;
using namespace sampleplayer::renderer;
using namespace dash::mpd;
using namespace libdash::framework::input;
using namespace libdash::framework::helpers;



#define SEGMENTBUFFER_SIZE 2

MultimediaManager::MultimediaManager            (QTGLRenderer *videoElement, QTAudioRenderer *audioElement) :
                   videoElement                 (videoElement),
                   audioElement                 (audioElement),
                   mpd                          (NULL),
                   period                       (NULL),
                   videoAdaptationSet           (NULL),
                   videoRepresentation          (NULL),
                   videoLogic                   (NULL),
                   videoStream                  (NULL),
                   audioAdaptationSet           (NULL),
                   audioRepresentation          (NULL),
                   audioLogic                   (NULL),
                   audioStream                  (NULL),
                   isStarted                    (false),
                   framesDisplayed              (0),
                   segmentsDownloaded           (0),
				   segmentDisplayIndex			(0),
                   isVideoRendering             (false),
                   isAudioRendering             (false)
{
    InitializeCriticalSection (&this->monitorMutex);
	this->logger  = new Logger();
    this->manager = CreateDashManager();
    av_register_all();
}
MultimediaManager::~MultimediaManager           ()
{
    this->Stop();
    this->manager->Delete();

    DeleteCriticalSection (&this->monitorMutex);
}

IMPD*   MultimediaManager::GetMPD                           ()
{
    return this->mpd;
}
bool    MultimediaManager::Init                             (const std::string& url)
{
    EnterCriticalSection(&this->monitorMutex);

    this->mpd = this->manager->Open((char *)url.c_str());

    if(this->mpd == NULL)
    {
        LeaveCriticalSection(&this->monitorMutex);
        return false;
    }

    LeaveCriticalSection(&this->monitorMutex);
    return true;
}
void    MultimediaManager::Start                            ()
{
    /* Global Start button for start must be added to interface*/
    if (this->isStarted)
        this->Stop();

    EnterCriticalSection(&this->monitorMutex);

    if (this->videoAdaptationSet && this->videoRepresentation)
    {
        this->InitVideoRendering(this->segmentDisplayIndex);				//2015.4.12 - php
        this->videoStream->Start();
        this->StartVideoRenderingThread();
    }

    if (this->audioAdaptationSet && this->audioRepresentation)
    {
        this->InitAudioPlayback(this->segmentDisplayIndex);
        this->audioElement->StartPlayback();
        this->audioStream->Start();
        this->StartAudioRenderingThread();
    }

    this->isStarted = true;

    LeaveCriticalSection(&this->monitorMutex);
}
void    MultimediaManager::Stop                             ()
{
    if (!this->isStarted)
        return;

    EnterCriticalSection(&this->monitorMutex);

    this->StopVideo();
    this->StopAudio();

    this->isStarted = false;

    LeaveCriticalSection(&this->monitorMutex);
}
void    MultimediaManager::StopVideo                        ()
{
    if(this->isStarted && this->videoStream)
    {
        this->videoStream->Stop();
        this->StopVideoRenderingThread();

        delete this->videoStream;
        delete this->videoLogic;

        this->videoStream = NULL;
        this->videoLogic = NULL;
    }
}
void    MultimediaManager::StopAudio                        ()
{
    if (this->isStarted && this->audioStream)
    {
        this->audioStream->Stop();
        this->StopAudioRenderingThread();

        this->audioElement->StopPlayback();

        delete this->audioStream;
        delete this->audioLogic;

        this->audioStream = NULL;
        this->audioLogic = NULL;
    }
}

//2015.4.12 - php
void    MultimediaManager::SetSegmentDisplayIndex		(uint32_t segmentDisplayIndex)
{
	this->segmentDisplayIndex = segmentDisplayIndex;
}
uint32_t  MultimediaManager::GetSegmentTotalSize ()
{
	uint32_t segmentTotalSize = 0;
	IMPD* mpd = this->GetMPD();
	if (mpd)
	{
		std::vector<IPeriod*>  periods = mpd->GetPeriods();
		if (periods.size()>0)
		{
			ISegmentList* segmentList = periods.at(0)->GetSegmentList();
			if (segmentList)
			{
				segmentTotalSize = segmentList->GetSegmentURLs().size();
			}else
			{
				std::vector<IAdaptationSet*> adaptionsets = periods.at(0)->GetAdaptationSets();
				if (adaptionsets.size()>0)
				{
					segmentList = adaptionsets.at(0)->GetSegmentList();
					if (segmentList)
					{
						segmentTotalSize = segmentList->GetSegmentURLs().size();
					} 
					else
					{
						std::vector<IRepresentation *> representations = adaptionsets.at(0)->GetRepresentation();
						if (representations.size()>0)
						{
							segmentList = representations.at(0)->GetSegmentList();
							if (segmentList)
							{
								segmentTotalSize = segmentList->GetSegmentURLs().size();
							} 
						}
					}
				}
			}
		}
	}
	return segmentTotalSize;
}
void	   MultimediaManager::SetProgress(uint32_t  segmentDisplayIndex)
{
	this->SetSegmentDisplayIndex(segmentDisplayIndex);
	//this->videoStream->SetPosition(segmentDisplayIndex);
	//this->audioStream->SetPosition(segmentDisplayIndex);
}


bool    MultimediaManager::SetVideoQuality                  (IPeriod* period, IAdaptationSet *adaptationSet, IRepresentation *representation)
{
    EnterCriticalSection(&this->monitorMutex);

    this->period                = period;
    this->videoAdaptationSet    = adaptationSet;
    this->videoRepresentation   = representation;

    if (this->videoStream)
        this->videoStream->SetRepresentation(this->period, this->videoAdaptationSet, this->videoRepresentation);

    LeaveCriticalSection(&this->monitorMutex);
    return true;
}
bool    MultimediaManager::SetAudioQuality                  (IPeriod* period, IAdaptationSet *adaptationSet, IRepresentation *representation)
{
    EnterCriticalSection(&this->monitorMutex);

    this->period                = period;
    this->audioAdaptationSet    = adaptationSet;
    this->audioRepresentation   = representation;

    if (this->audioStream)
        this->audioStream->SetRepresentation(this->period, this->audioAdaptationSet, this->audioRepresentation);

    LeaveCriticalSection(&this->monitorMutex);
    return true;
}
bool    MultimediaManager::SetVideoAdaptationLogic          (libdash::framework::adaptation::LogicType type)
{
    //Currently unused, always using ManualAdaptation.
    return true;
}
bool    MultimediaManager::SetAudioAdaptationLogic          (libdash::framework::adaptation::LogicType type)
{
    //Currently unused, always using ManualAdaptation.
    return true;
}
void    MultimediaManager::AttachManagerObserver            (IMultimediaManagerObserver *observer)
{
    this->managerObservers.push_back(observer);
}
void	   MultimediaManager::NotifySegmentDisplayIndexChanged	(uint32_t segmentDisplayIndex)
{
	for (size_t i = 0; i < this->managerObservers.size(); i++)
		this->managerObservers.at(i)->OnSegmentDisplayIndexChanged(segmentDisplayIndex);
}
void MultimediaManager::NotifyAudioSegmentIndexChanged  (uint32_t segmentDisplayIndex)
{
	for (size_t i = 0; i < this->managerObservers.size(); i++)
		this->managerObservers.at(i)->OnAudioSegmentIndexChanged(segmentDisplayIndex);
}

void MultimediaManager::NotifyProgressCompleted()
{
	for (size_t i = 0; i < this->managerObservers.size(); i++)
		this->managerObservers.at(i)->OnProgressCompleted();
}
void    MultimediaManager::NotifyVideoBufferObservers       (uint32_t fillstateInPercent)
{
    for (size_t i = 0; i < this->managerObservers.size(); i++)
        this->managerObservers.at(i)->OnVideoBufferStateChanged(fillstateInPercent);
}
void    MultimediaManager::NotifyVideoSegmentBufferObservers(uint32_t fillstateInPercent)
{
    for (size_t i = 0; i < this->managerObservers.size(); i++)
        this->managerObservers.at(i)->OnVideoSegmentBufferStateChanged(fillstateInPercent);
}
void    MultimediaManager::NotifyAudioSegmentBufferObservers(uint32_t fillstateInPercent)
{
    for (size_t i = 0; i < this->managerObservers.size(); i++)
        this->managerObservers.at(i)->OnAudioSegmentBufferStateChanged(fillstateInPercent);
}
void    MultimediaManager::NotifyAudioBufferObservers       (uint32_t fillstateInPercent)
{
    for (size_t i = 0; i < this->managerObservers.size(); i++)
        this->managerObservers.at(i)->OnAudioBufferStateChanged(fillstateInPercent);
}
void    MultimediaManager::InitVideoRendering               (uint32_t offset)
{
    this->videoLogic = AdaptationLogicFactory::Create(libdash::framework::adaptation::Manual, this->mpd, this->period, this->videoAdaptationSet);

    this->videoStream = new MultimediaStream(sampleplayer::managers::VIDEO, this->mpd, SEGMENTBUFFER_SIZE, 2, 0);
    this->videoStream->AttachStreamObserver(this);
    this->videoStream->SetRepresentation(this->period, this->videoAdaptationSet, this->videoRepresentation);
    this->videoStream->SetPosition(offset);
}
void    MultimediaManager::InitAudioPlayback                (uint32_t offset)
{
    this->audioLogic = AdaptationLogicFactory::Create(libdash::framework::adaptation::Manual, this->mpd, this->period, this->audioAdaptationSet);

    this->audioStream = new MultimediaStream(sampleplayer::managers::AUDIO, this->mpd, SEGMENTBUFFER_SIZE, 0, 10);
    this->audioStream->AttachStreamObserver(this);
    this->audioStream->SetRepresentation(this->period, this->audioAdaptationSet, this->audioRepresentation);
    this->audioStream->SetPosition(offset);
}
void    MultimediaManager::OnSegmentDownloaded              ()
{
    this->segmentsDownloaded++;
}
void    MultimediaManager::OnSegmentBufferStateChanged    (StreamType type, uint32_t fillstateInPercent)
{
    switch (type)
    {
        case AUDIO:
            this->NotifyAudioSegmentBufferObservers(fillstateInPercent);
            break;
        case VIDEO:
            this->NotifyVideoSegmentBufferObservers(fillstateInPercent);
            break;
        default:
            break;
    }
}
void    MultimediaManager::OnVideoBufferStateChanged      (uint32_t fillstateInPercent)
{
    this->NotifyVideoBufferObservers(fillstateInPercent);
}
void    MultimediaManager::OnAudioBufferStateChanged      (uint32_t fillstateInPercent)
{
    this->NotifyAudioBufferObservers(fillstateInPercent);
}
void MultimediaManager::OnRateChanged	(int segmentNumber, uint32_t downloadRate){
	//std::cout << "MMManager #" << segmentNumber << std::endl;
	static uint64_t segment = 0;
	uint32_t bw = 0;
	this->NotifyRateChanged(segmentNumber, downloadRate);
	const std::vector<IRepresentation *> reps = this->videoAdaptationSet->GetRepresentation();
	int i;
	//this->videoRepresentation = reps.at(0);
	for(i = 0; i < reps.size(); i++){
		bw = reps.at(i)->GetBandwidth();
		this->videoRepresentation = reps.at(i);
		if(bw > downloadRate*8){
			//bw = reps.at(i-1)->GetBandwidth();
			break;
		}
	}
	//std::cout << "BW " << bw << " " << downloadRate << std::endl;
	if(segment != this->segmentsDownloaded || (bw - downloadRate*8 > 101492 && bw - downloadRate*8 != reps.at(0)->GetBandwidth())){

		this->SetVideoQuality(this->period, this->videoAdaptationSet, this->videoRepresentation);
		segment = this->segmentsDownloaded;
		this->logger->rateLog(this->segmentsDownloaded, bw/8);
		for (size_t i = 0; i < this->managerObservers.size(); i++)
			this->managerObservers.at(i)->OnBWChanged(bw);
	}
}
void    MultimediaManager::SetFrameRate                     (double framerate)
{
    this->frameRate = framerate;
}

bool    MultimediaManager::StartVideoRenderingThread        ()
{
    this->isVideoRendering = true;

    this->videoRendererHandle = CreateThreadPortable (RenderVideo, this);

    if(this->videoRendererHandle == NULL)
        return false;

    return true;
}
void    MultimediaManager::StopVideoRenderingThread         ()
{
    this->isVideoRendering = false;

    if (this->videoRendererHandle != NULL)
    {
        JoinThread(this->videoRendererHandle);
        DestroyThreadPortable(this->videoRendererHandle);
    }
}
bool    MultimediaManager::StartAudioRenderingThread        ()
{
    this->isAudioRendering = true;

    this->audioRendererHandle = CreateThreadPortable (RenderAudio, this);

    if(this->audioRendererHandle == NULL)
        return false;

    return true;
}
void    MultimediaManager::StopAudioRenderingThread         ()
{
    this->isAudioRendering = false;

    if (this->audioRendererHandle != NULL)
    {
        JoinThread(this->audioRendererHandle);
        DestroyThreadPortable(this->audioRendererHandle);
    }
}
void*   MultimediaManager::RenderVideo        (void *data)
{
    MultimediaManager *manager = (MultimediaManager*) data;

	//2015.4.10 - php
	VideoFrame *frame = manager->videoStream->GetFrame();

    while(manager->isVideoRendering)
    {
		//2015.4.15 - php
        if (frame)
        {
			if (frame->IsLastFrame())
			{
				if (frame->SegmentIndex() == manager->GetSegmentTotalSize()-1)
				{
					manager->NotifyProgressCompleted();
				}
			}else
			{
				manager->videoElement->SetImage(frame->FrameImage());
				manager->videoElement->update();

				manager->framesDisplayed++;
				if (manager->segmentDisplayIndex!=frame->SegmentIndex() && frame->SegmentIndex() >= 0)
				{
					manager->segmentDisplayIndex =frame ->SegmentIndex();
					manager->NotifySegmentDisplayIndexChanged (manager->segmentDisplayIndex);
				}

				PortableSleep(1 / manager->frameRate);

				delete(frame->FrameImage());
			}
        }

        frame = manager->videoStream->GetFrame();
    }

    return NULL;
}
void*   MultimediaManager::RenderAudio        (void *data)
{
    MultimediaManager *manager = (MultimediaManager*) data;

    AudioSamples *samples = manager->audioStream->GetSamples();

    while(manager->isAudioRendering)
    {
        if (samples)
        {
            manager->audioElement->WriteToBuffer(samples->Samples()->Data(), samples->Samples()->Length());

			//2015.4.18 - php
			if (manager->audioSegmentIndex!=samples->SegmentIndex() && samples->SegmentIndex() >= 0)
			{
				manager->audioSegmentIndex =samples ->SegmentIndex();
				manager->NotifyAudioSegmentIndexChanged(manager->audioSegmentIndex);
			}

            PortableSleep(1 / manager->frameRate);

            delete samples;
        }

        samples = manager->audioStream->GetSamples();
    }

    return NULL;
}
void    MultimediaManager::NotifyRateChanged (int segmentNumber, uint32_t downloadRate) //added function
{
	for (size_t i = 0; i < this->managerObservers.size(); i++)
		this->managerObservers.at(i)->OnRateChanged(segmentNumber, downloadRate);
}