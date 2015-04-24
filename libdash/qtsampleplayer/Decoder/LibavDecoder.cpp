/*
 * LibavDecoder.cpp
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#include "LibavDecoder.h"

using namespace libdash::framework::input;
using namespace sampleplayer::decoder;

#define AVERROR_RESSOURCE_NOT_AVAILABLE -11

static int          IORead                           (void *opaque, uint8_t *buf, int buf_size)
{
    IDataReceiver* datrec = (IDataReceiver*)opaque;
    int ret = datrec->Read(buf, buf_size);
    return ret;
}

LibavDecoder::LibavDecoder  (IDataReceiver* rec) :
         receiver           (rec),
         errorHappened      (false),
         bufferSize         (32768)
{
}
LibavDecoder::~LibavDecoder ()
{
}

void                LibavDecoder::AttachAudioObserver     (IAudioObserver *observer)
{
    audioObservers.push_back(observer);
}
void                LibavDecoder::AttachVideoObserver     (IVideoObserver *observer)
{
    videoObservers.push_back(observer);
}
void                LibavDecoder::Notify                  (FrameConfig  *frameconfig, StreamConfig *config)
{
    switch(config->stream->codec->codec_type)
    {
        case AVMEDIA_TYPE_VIDEO:
            this->NotifyVideo(frameconfig, config);			//2015.4.9 - php
            break;
        case AVMEDIA_TYPE_AUDIO:
            this->NotifyAudio(frameconfig, config);			//2015.4.8 - php
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            break;
        case AVMEDIA_TYPE_UNKNOWN:
            break;
        default:
            break;
    }
}

void                LibavDecoder::NotifyVideo             (FrameConfig  *frameconfig, StreamConfig* decoConf)
{
    videoFrameProperties props;

    props.fireError     = false;
    props.frame         = decoConf->frameCnt;
    props.height        = decoConf->stream->codec->height;
    props.width         = decoConf->stream->codec->width;
    props.linesize      = frameconfig->frame->linesize;
    props.streamIndex   = decoConf->stream->index;
	props.segmentIndex = frameconfig->segmentIndex;			//2015.4.9 - php
	props.isLastFrame = frameconfig->isLastFrame;			//2015.4.15 - php

    if(decoConf->stream->codec->pix_fmt == PIX_FMT_YUV420P)
        props.pxlFmt = yuv420p;
    if(decoConf->stream->codec->pix_fmt == PIX_FMT_YUV422P)
        props.pxlFmt = yuv422p;

		for(size_t i = 0; i < videoObservers.size(); i++)
			videoObservers.at(i)->OnVideoDataAvailable((const uint8_t **)frameconfig->frame->data, &props);

}
void                LibavDecoder::NotifyAudio             (FrameConfig  *frameconfig, StreamConfig* decoConf)
{
    audioFrameProperties props;

    props.fireError   = false;
    props.streamIndex = decoConf->stream->index;
    props.linesize    = frameconfig->frame->linesize[0];
    props.channels    = decoConf->stream->codec->channels;
    props.sampleRate  = decoConf->stream->codec->sample_rate;
    props.samples     = frameconfig->frame->nb_samples;
	props.segmentIndex = frameconfig->segmentIndex;			//2015.4.18 - php
	props.isLastFrame = frameconfig->isLastFrame;			//2015.4.18 - php

    for(size_t i = 0; i < audioObservers.size(); i++)
        audioObservers.at(i)->OnAudioDataAvailable((const uint8_t **) frameconfig->frame->data, &props);
}
AVFormatContext*    LibavDecoder::OpenInput               ()
{
    AVFormatContext *avFormatContextPtr = NULL;
    int             err                 = 0;

    this->iobuffer                      = (unsigned char*) av_malloc(bufferSize);
    avFormatContextPtr                  = avformat_alloc_context();
    avFormatContextPtr->pb              = avio_alloc_context(this->iobuffer, bufferSize, 0, receiver, IORead, NULL, NULL);
    avFormatContextPtr->pb->seekable    = 0;

    err = avformat_open_input(&avFormatContextPtr, "", NULL, NULL);
	///*******************************test************************/
	// AVStream      *tempStream   = NULL;
	//for (size_t streamcnt = 0; streamcnt < avFormatContextPtr->nb_streams; ++streamcnt)
	//{
	//	tempStream = avFormatContextPtr->streams[streamcnt];
	//	 if (! tempStream->codec->codec)
	//	 {
	//		 tempStream = avFormatContextPtr->streams[streamcnt];
	//	 }
	//		 
	//}
    if (err < 0)
    {
        this->Error("Error while calling avformat_open_input", err, NULL, NULL);
        return NULL;
    }

    err = avformat_find_stream_info(avFormatContextPtr, 0);
    if (err < 0)
    {
        this->Error("Error while calling avformat_find_stream_info", err, NULL, NULL);
        return NULL;
    }

    return avFormatContextPtr;
}
void                LibavDecoder::InitStreams             (AVFormatContext *avFormatContextPtr)
{
    AVStream      *tempStream   = NULL;
    AVCodec       *tempCodec    = NULL;
    StreamConfig  tempConfig;

    for (size_t streamcnt = 0; streamcnt < avFormatContextPtr->nb_streams; ++streamcnt)
    {
        tempStream = avFormatContextPtr->streams[streamcnt];

        if ((tempStream->codec->codec_type == AVMEDIA_TYPE_VIDEO) ||
            (tempStream->codec->codec_type == AVMEDIA_TYPE_AUDIO))
        {
            tempConfig.stream         = tempStream;
            tempCodec                 = avcodec_find_decoder(tempStream->codec->codec_id);
            tempConfig.codecContext   = tempStream->codec;
            tempConfig.frameCnt       = 0;

            int i =avcodec_open2(tempConfig.codecContext, tempCodec, NULL);
            this->streamconfigs.push_back(tempConfig);
        }
    }
}
StreamConfig*       LibavDecoder::GetNextPacket           (AVFormatContext* avFormatContextPtr, AVPacket* avpkt, int segmentIndex)
{
    while (true)
    {
        int err = av_read_frame(avFormatContextPtr, avpkt);

        if ((size_t)err == AVERROR_EOF)
		{
			//StreamConfig *tempConfig = this->FindStreamConfig(this->avpkt.stream_index);
			//this->frameconfig.isLastFrame = true;
			//NotifyVideo(&this->frameconfig, tempConfig);
			//2015.4.15 - php
			StreamConfig *tempConfig = this->FindStreamConfig(avpkt->stream_index);
			if (tempConfig  && tempConfig->codecContext->codec_type  == AVMEDIA_TYPE_VIDEO)
			{
				videoFrameProperties   videoprops;
				videoprops.isLastFrame = true;
				videoprops.segmentIndex = segmentIndex;
				for (size_t i = 0; i < videoObservers.size(); i++)
					videoObservers.at(i)->OnVideoDataAvailable(NULL, &videoprops);
			}
			
            return NULL;
		}

        if (err == AVERROR_RESSOURCE_NOT_AVAILABLE)
        {
            av_free_packet(avpkt);
            continue;
        }

        if (err < 0)
        {
			//2015.4.15 - php
			StreamConfig *tempConfig = this->FindStreamConfig(avpkt->stream_index);
			if (tempConfig  && tempConfig->codecContext->codec_type  == AVMEDIA_TYPE_VIDEO)
			{
				videoFrameProperties   videoprops;
				videoprops.isLastFrame = true;
				videoprops.segmentIndex = segmentIndex;
				this->Error("Error while av_read_frame", err, &videoprops, NULL);
			}
			
            return NULL;
        }

        StreamConfig *tempConfig = this->FindStreamConfig(avpkt->stream_index);
        if (tempConfig)
            return tempConfig;	
        av_free_packet(avpkt);
    }
}
StreamConfig*       LibavDecoder::FindStreamConfig        (int streamIndex)
{
    size_t configsCount = this->streamconfigs.size();

    for (size_t i = 0; i < configsCount; i++)
    {
        if (this->streamconfigs.at(i).stream->index == streamIndex)
            return &this->streamconfigs.at(i);
    }

    return NULL;
}
int                 LibavDecoder::DecodeMedia             (AVFrame *frame, AVPacket *avpkt, StreamConfig *decConfig, int *got_frame)
{
    switch(decConfig->stream->codec->codec_type)
    {
        case AVMEDIA_TYPE_VIDEO:
            return avcodec_decode_video2(decConfig->codecContext, frame, got_frame, avpkt);
        case AVMEDIA_TYPE_AUDIO:
            return avcodec_decode_audio4(decConfig->codecContext, frame, got_frame, avpkt);
        case AVMEDIA_TYPE_SUBTITLE:
            break;
        case AVMEDIA_TYPE_UNKNOWN:
            break;
        default:
            break;
    }

    return -1;
}
int                 LibavDecoder::DecodeFrame             (FrameConfig  *frameconfig, AVPacket* avpkt, StreamConfig* decConfig)
{
    int len         = 0;
    int got_frame   = 1;

    while (avpkt->size > 0)
    {
        /* TODO handle multi frame packets */
        len = this->DecodeMedia(frameconfig->frame, avpkt, decConfig, &got_frame);			//2015.4.8 - php

        if(len < 0)
        {
            this->Error("Error while decoding frame", len, NULL, NULL);
            return -1;
        }

        if(got_frame)
        {
            this->Notify(frameconfig, decConfig);			//2015.4.8 - php
            decConfig->frameCnt++;
        }

        av_free_packet(avpkt);
    }
    return 0;
}
void                LibavDecoder::Flush                   ()
{
    int gotFrame        = 0;
    int configsCount    = this->streamconfigs.size();

    av_free_packet(&this->avpkt);
    av_init_packet(&this->avpkt);

    for (int i = 0; i < configsCount; i++)
    {
        do
        {
            this->DecodeMedia(this->frameconfig.frame, &this->avpkt, &this->streamconfigs.at(i), &gotFrame);			//2015.4.9 - php

            if (gotFrame)
			{
                this->Notify(&this->frameconfig, &this->streamconfigs.at(i));		//2015.4.9 - php
			}

            av_free_packet(&this->avpkt);

        } while(gotFrame);
    }
}
bool                LibavDecoder::Init                    ()
{
    this->avFormatContextPtr = this->OpenInput();

    if (this->errorHappened)
        return false;

    this->frameconfig.frame = avcodec_alloc_frame();				//2015.4.8 - php
	this->frameconfig.segmentIndex = 0;			//2015.4.9 - php
	this->frameconfig.isLastFrame = false;			//2015.4.14 - php

    av_init_packet(&this->avpkt);
    this->InitStreams(avFormatContextPtr);

    return true;
}
bool                LibavDecoder::Decode                  (uint32_t segmentIndex)			
{

    StreamConfig *decConfig = this->GetNextPacket(this->avFormatContextPtr, &this->avpkt, segmentIndex);
	this->frameconfig.segmentIndex = segmentIndex;					//2015.4.8 - php

	//2015.4.15 - php
    if(decConfig == NULL)
	{
		//StreamConfig *tempConfig = this->FindStreamConfig(this->avpkt.stream_index);
		//this->frameconfig.isLastFrame = true;
		//NotifyVideo(&this->frameconfig, tempConfig);
        return false;
	}
	
	int err = this->DecodeFrame(&this->frameconfig, &this->avpkt, decConfig);
    if(err < 0)			//2015.4.8 - php
        return false;

    return true;
}

void                LibavDecoder::Stop                    ()
{
    this->FreeConfigs();
    avformat_close_input(&this->avFormatContextPtr);
    av_free(this->frameconfig.frame);					//2015.4.9 - php
}
void                LibavDecoder::FreeConfigs             ()
{
    for(size_t i = 0; i < this->streamconfigs.size(); i++)
        avcodec_close(this->streamconfigs.at(i).codecContext);
}
void                LibavDecoder::Error                   (std::string errormsg, int errorcode, videoFrameProperties *videoProps, audioFrameProperties *audioProps)
{
	videoFrameProperties    videoprops;
	audioFrameProperties    audioprops;
	//2015.4.15 - php
	if (videoProps)
		videoprops = *videoProps;
	if (audioProps)
		audioprops = *audioProps;

    char                    errorCodeMsg[1024];
    std::stringstream       sstm;

    av_strerror(errorcode, errorCodeMsg, 1024);
    sstm << "ERROR: " << errormsg << ", ErrorCodeMsg: " << errorCodeMsg;

    videoprops.fireError        = true;
    videoprops.errorMessage = (char*)malloc(sstm.str().size()+1);
    strcpy(videoprops.errorMessage, sstm.str().c_str());

    for (size_t i = 0; i < videoObservers.size(); i++)
        videoObservers.at(i)->OnVideoDataAvailable(NULL, &videoprops);

    audioprops.fireError       = true;
    audioprops.errorMessage = (char*)malloc(sstm.str().size()+1);
    strcpy(audioprops.errorMessage, sstm.str().c_str());

    for (size_t j = 0; j < audioObservers.size(); j++)
        audioObservers.at(j)->OnAudioDataAvailable(NULL, &audioprops);

    this->errorHappened    = true;
    free(audioprops.errorMessage);
    free(videoprops.errorMessage);
}
