
#ifndef QTSAMPLEPLAYER_BUFFER_AUDIOSAMPLES_H_
#define QTSAMPLEPLAYER_BUFFER_AUDIOSAMPLES_H_

#include "config.h"
#include "../libdashframework/Buffer/AudioChunk.h"

namespace libdash
{
	namespace framework
	{
		namespace buffer
		{
			//2015.4.18 - php
			class AudioSamples
			{
			public:
				AudioSamples		(AudioChunk  *audioSamples,  uint32_t segmentIndex, bool isLastFrame);
				~AudioSamples();

				AudioChunk    *Samples  ();
				uint32_t			SegmentIndex	();
				bool					IsLastFrame   ();

			private:
				AudioChunk  *samples;
				uint32_t		 segmentIndex;
				bool				isLastFrame;		
			};


		}
	}
}

#endif /* QTSAMPLEPLAYER_BUFFER_AUDIOSAMPLES_H_ */
