
#ifndef QTSAMPLEPLAYER_BUFFER_VIDEOFRAME_H_
#define QTSAMPLEPLAYER_BUFFER_VIDEOFRAME_H_

#include "config.h"
#include <QtGui/qimage.h>

namespace libdash
{
	namespace framework
	{
		namespace buffer
		{
			//2015.4.8 - php
			class VideoFrame
			{
			public:
				VideoFrame		(QImage *frameImage,  uint32_t segmentIndex, bool isLastFrame);
				~VideoFrame();

				QImage*			FrameImage	();
				uint32_t			SegmentIndex	();
				bool					IsLastFrame();

			private:
				QImage		*frameImage;
				uint32_t		 segmentIndex;
				bool				isLastFrame;				//2015.4.15 - php
			};


		}
	}
}

#endif /* QTSAMPLEPLAYER_BUFFER_VIDEOFRAME_H_ */
