#include "VideoFrame.h"

using namespace libdash::framework::buffer;

VideoFrame::VideoFrame( QImage *frameImage, uint32_t segmentIndex, bool isLastFrame):
frameImage    (frameImage),
	segmentIndex   (segmentIndex),
	isLastFrame		(isLastFrame)
{

}


VideoFrame::~VideoFrame()
{
	delete this->frameImage;
}

QImage*  VideoFrame::FrameImage()
{
	return this->frameImage;	
}

uint32_t  VideoFrame::SegmentIndex	()
{
	return this->segmentIndex;
}
bool		VideoFrame::IsLastFrame		()
{
	return  this->isLastFrame;
}