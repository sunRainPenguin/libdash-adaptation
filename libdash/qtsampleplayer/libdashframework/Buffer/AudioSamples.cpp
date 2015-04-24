#include "AudioSamples.h"
using namespace libdash::framework::buffer;

AudioSamples :: AudioSamples(AudioChunk  *audioSamples,  uint32_t segmentIndex, bool isLastFrame):
					samples (audioSamples),
					segmentIndex (segmentIndex),
					isLastFrame (isLastFrame)
{
}

AudioSamples::~AudioSamples()
{
	delete this->samples;
}

AudioChunk*  AudioSamples::Samples()
{
	return this->samples;
}
uint32_t  AudioSamples::SegmentIndex	()
{
	return this->segmentIndex;
}

bool	 AudioSamples::IsLastFrame		()
{
	return  this->isLastFrame;
}