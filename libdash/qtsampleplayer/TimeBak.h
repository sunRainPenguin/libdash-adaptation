#ifndef DASH_HELPERS_TIMEBAK_H_
#define DASH_HELPERS_TIMEBAK_H_


#include <string>
#include "libdash.h"
#include <sys/timeb.h>
#include <sys/utime.h>
#include <WinSock.h>
#include <time.h>
#include "config.h"


namespace libdash
{
	namespace framework{
		namespace helpers
		{
			class Time
			{
			public:
				static uint32_t     GetCurrentUTCTimeInSec  ();
				static uint64_t	    GetCurrentUTCTimeInMilliSec();
				static std::string  GetCurrentUTCTimeStr    ();
				static int gettimeofday(struct timeval *tp, void *tzp);

			private:
				static struct tm*   GetCurrentUTCTime       ();

			};
		}
	}
}

#endif /* DASH_HELPERS_TIMEBAK_H_ */
