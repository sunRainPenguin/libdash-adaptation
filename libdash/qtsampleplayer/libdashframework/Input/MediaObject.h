/*
 * MediaObject.h
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#ifndef LIBDASH_FRAMEWORK_INPUT_MEDIAOBJECT_H_
#define LIBDASH_FRAMEWORK_INPUT_MEDIAOBJECT_H_

#include "IMPD.h"
#include "IDownloadObserver.h"
#include "IDASHMetrics.h"
#include "../Portable/MultiThreading.h"
#include "IMediaObjectObserver.h"

namespace libdash
{
    namespace framework
    {
        namespace input
        {
            class MediaObject : public dash::network::IDownloadObserver, public dash::metrics::IDASHMetrics
            {
                public:
					MediaObject             (dash::mpd::ISegment *segment, dash::mpd::IRepresentation *rep, uint32_t segmentIndex, IMediaObjectObserver* observer);
                    virtual ~MediaObject    ();

                    bool                        StartDownload       ();
                    void                        AbortDownload       ();
                    void                        WaitFinished        ();
                    int                         Read                (uint8_t *data, size_t len);
                    int                         Peek                (uint8_t *data, size_t len);
                    int                         Peek                (uint8_t *data, size_t len, size_t offset);
                    dash::mpd::IRepresentation* GetRepresentation   ();
					uint32_t GetSegmentIndex   ();
                    virtual void    OnDownloadStateChanged  (dash::network::DownloadState state);
                    virtual void    OnDownloadRateChanged   (uint64_t bytesDownloaded);
                    /*
                     * IDASHMetrics
                     */
                    const std::vector<dash::metrics::ITCPConnection *>&     GetTCPConnectionList    () const;
                    const std::vector<dash::metrics::IHTTPTransaction *>&   GetHTTPTransactionList  () const;

                private:
					uint64_t								downloaded;				//2015.4.20 - php
					 IMediaObjectObserver*  observer;			//2015.4.20 - php

                    dash::mpd::ISegment             *segment;
                    dash::mpd::IRepresentation      *rep;
                    dash::network::DownloadState    state;
					uint32_t segmentIndex;				//2015.4.7 - php

                    mutable CRITICAL_SECTION    stateLock;
                    mutable CONDITION_VARIABLE  stateChanged;
            };
        }
    }
}

#endif /* LIBDASH_FRAMEWORK_INPUT_MEDIAOBJECT_H_ */