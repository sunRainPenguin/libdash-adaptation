#include "TimeBak.h"

using namespace libdash::framework::helpers;

uint32_t    Time::GetCurrentUTCTimeInSec   ()
{
	return mktime(Time::GetCurrentUTCTime());
}

uint64_t	Time::GetCurrentUTCTimeInMilliSec(){
	struct timeval curtime;
	gettimeofday(&curtime, 0);
	return (curtime.tv_sec)*1000 + curtime.tv_usec/1000;
}
int Time::gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year     = wtm.wYear - 1900;
	tm.tm_mon     = wtm.wMonth - 1;
	tm.tm_mday     = wtm.wDay;
	tm.tm_hour     = wtm.wHour;
	tm.tm_min     = wtm.wMinute;
	tm.tm_sec     = wtm.wSecond;
	tm. tm_isdst    = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}

std::string Time::GetCurrentUTCTimeStr   ()
{
	char timeString[30];
	strftime(timeString, 30, "%Y-%m-%dT%H:%M:%SZ", Time::GetCurrentUTCTime());

	return std::string(timeString);
}
struct tm*  Time::GetCurrentUTCTime     ()
{
	time_t      rawTime;

	time(&rawTime);
	return gmtime(&rawTime);
}
