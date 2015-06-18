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



#define SEGMENTBUFFER_SIZE 5

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
                   isAudioRendering             (false),
				   Orate                        (0.3),
				   Yrate                        (0.3),
				   NextT                        (0),
				   R                            (0),
				   OfillstateInPercent           (0)

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
void    MultimediaManager::Start                            (int progress)
{
    /* Global Start button for start must be added to interface*/
    if (this->isStarted)
        this->Stop();
	if (progress>0)
		SetSegmentDisplayIndex(progress);
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
	if (this->monitorMutex.RecursionCount>=1)
		return false;
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
	OfillstateInPercent =  fillstateInPercent;			//wen
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
	this->NotifyRateChanged(segmentNumber, downloadRate);
	const std::vector<IRepresentation *> reps = this->videoAdaptationSet->GetRepresentation();
	double k = 0.14;
	double w = 0.03;
	double afa = 0.2;
	double bta = 0.2; 
	double deta_up = 0 ;
	double deta_down = 0;
	double e = 0.15;
	uint32_t R_up=0 ;
	uint32_t R_down=0 ;
	int temp;
	int Tmin = 4;
	this->videoRepresentation = reps.at(R);
	int bw = reps.at(R)->GetBandwidth();
	fstream foud;
	foud.open("C:\\Users\\Administrator\\Desktop\\Out.txt",ios::app);


	if(segmentsDownloaded+1 == segmentNumber )
	{
		foud<<segmentNumber<<'\t';


		foud<<downloadRate*8<<'\t';

		if(segmentsDownloaded == 0)
		{
			Orate = double(downloadRate)*8/1000/1000 ;
			Yrate = Orate ;
		}
		//Panda算法实现-第一步：估值，Orate为MultimediaManager自添成员，returnDur()为logger自添成员
		Orate = Orate + NextT*k*(w-max(0,Orate-double(downloadRate)*8/1000/1000+w))   ;

		//	foud<<"Orate:"<<Orate*1000<<'\t';

		//Panda算法实现-第二步：平滑处理，Yrate为MultimediaManager自添成员
		Yrate = Yrate + NextT*(-afa*(Yrate-Orate))  ;

		foud<<Yrate*1000*1000<<'\t';


		//Panda算法实现-第三步：量化处理
		deta_up = e * Yrate ;
		temp =(Yrate - deta_up)*1000*1000;
		//		foud<<"R_up:"<<temp<<'\t';

		for(int Ti=0;Ti<reps.size();Ti++)    
		{
			if(reps.at(Ti)->GetBandwidth()<= temp )
				R_up=Ti;
			if(Ti>R_up)
				break;
		}

		temp = (Yrate - deta_down)*1000*1000;
		//		foud<<"R_down:"<<temp<<'\t';

		for(int Tii=0;Tii<reps.size();Tii++)    
		{
			if(reps.at(Tii)->GetBandwidth()<= temp )
				R_down=Tii; 
			if(Tii>R_down)
				break;
		}

		if (R < R_up)
			R = R_up;
		else 
			if(R <= R_down && R >= R_up)
				R = R;
			else
				R = R_down ;
		//Panda算法实现-第四步：调度下一次的下载 

		//		foud<<"Ro:"<<R<<'\t';
		//foud<<"Ro-BW:"<<reps.at(R)->GetBandwidth()<<'\t';
		//foud<<"OSetFrameRate:"<<frameRate<<'\t';

		if(OfillstateInPercent<=1.5*Tmin*100/(SEGMENTBUFFER_SIZE*2))
		{
			SetFrameRate(20);
			if(R>=1)
				R--;
		}
		if(OfillstateInPercent>2*Tmin*100/(SEGMENTBUFFER_SIZE*2))
		{
			SetFrameRate(24);
		}

		if(OfillstateInPercent>=4*Tmin*100/(SEGMENTBUFFER_SIZE*2))
		{
			if(R<reps.size()-1)
				R++;


		}
		//		foud<<"Rr:"<<R<<'\t';
		foud<<reps.at(R)->GetBandwidth()<<'\t';
		//foud<<"SetFrameRate:"<<frameRate<<'\t'; 
		foud<<OfillstateInPercent<<endl;	
		OnSegmentDownloaded();
		this->videoRepresentation = reps.at(R);
		bw = reps.at(R)->GetBandwidth();


		NextT = (2*bw)/(Yrate*1000*1000)+bta*((SEGMENTBUFFER_SIZE*2*OfillstateInPercent)/100-Tmin);
		if(NextT<0)
			NextT=0;	
		//foud<<"NextT:"<<NextT<<endl; 
		foud.close();
	}

	if(segment!=segmentNumber && this->SetVideoQuality(this->period, this->videoAdaptationSet, this->videoRepresentation))	
	{
		segment = segmentNumber;
		//this->logger->rateLog(this->segmentsDownloaded, bw/8);
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