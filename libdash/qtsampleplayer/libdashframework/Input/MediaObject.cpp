/*
 * MediaObject.cpp
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#include "MediaObject.h"

using namespace libdash::framework::input;

using namespace dash::mpd;
using namespace dash::network;
using namespace dash::metrics;

MediaObject::MediaObject    (ISegment *segment, IRepresentation *rep, uint32_t segmentIndex, IMediaObjectObserver* observer) :
					segment        (segment),
					rep            (rep),
					segmentIndex		(segmentIndex),
					observer (observer)
{
	//IMediaObjectObserver* Receiver = observer;
	//IMediaObjectObserver** MediaObjectObserver = this->observer;
    InitializeConditionVariable (&this->stateChanged);
    InitializeCriticalSection   (&this->stateLock);
}
MediaObject::~MediaObject   ()
{
    if(this->state == IN_PROGRESS)
    {
        this->segment->AbortDownload();
        this->OnDownloadStateChanged(ABORTED);
    }
    this->segment->DetachDownloadObserver(this);
    this->WaitFinished();
    DeleteConditionVariable (&this->stateChanged);
    DeleteCriticalSection   (&this->stateLock);
	//2015.4.24 - php
	delete(this->segment);
	this->segment = NULL;
}

bool                MediaObject::StartDownload          ()
{
    this->segment->AttachDownloadObserver(this);
    return this->segment->StartDownload();
}
void                MediaObject::AbortDownload          ()
{
    this->segment->AbortDownload();
    this->OnDownloadStateChanged(ABORTED);
}
void                MediaObject::WaitFinished           ()
{
    EnterCriticalSection(&this->stateLock);

    while(this->state != COMPLETED && this->state != ABORTED)
        SleepConditionVariableCS(&this->stateChanged, &this->stateLock, INFINITE);

    LeaveCriticalSection(&this->stateLock);
}
int                 MediaObject::Read                   (uint8_t *data, size_t len)
{
    return this->segment->Read(data, len);
}
int                 MediaObject::Peek                   (uint8_t *data, size_t len)
{
    return this->segment->Peek(data, len);
}
int                 MediaObject::Peek                   (uint8_t *data, size_t len, size_t offset)
{
    return this->segment->Peek(data, len, offset);
}
IRepresentation*    MediaObject::GetRepresentation      ()
{
    return this->rep;
}
uint32_t    MediaObject::GetSegmentIndex		()
{
	return this->segmentIndex;
}
void                MediaObject::OnDownloadStateChanged (DownloadState state)
{
    EnterCriticalSection(&this->stateLock);

    this->state = state;

    WakeAllConditionVariable(&this->stateChanged);
    LeaveCriticalSection(&this->stateLock);
}
void                MediaObject::OnDownloadRateChanged  (uint64_t bytesDownloaded)
{
	this->downloaded = bytesDownloaded;
	
	if (this->observer!=NULL)
	{
		//DASHReceiver* MediaObjectObserver = (*(this->observer));
		//IMediaObjectObserver* Receiver = observer;
		//IMediaObjectObserver** MediaObjectObserver = this->observer;
		this->observer->OnDownloadRateChanged(bytesDownloaded);
	}
	
}


const std::vector<ITCPConnection *>&    MediaObject::GetTCPConnectionList   () const
{
    return this->segment->GetTCPConnectionList();
}
const std::vector<IHTTPTransaction *>&  MediaObject::GetHTTPTransactionList () const
{
    return this->segment->GetHTTPTransactionList();
}
