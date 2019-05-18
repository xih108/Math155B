//
// TimeStamp.cpp
//

#include "TimeStamp.h"

void TimeStamp::Reset()
{
	TimeStampValue* tsPtr = TheTimeArray.GetFirstEntryPtr();
	for ( long i = TheTimeArray.SizeUsed(); i>0; i--, tsPtr++ ) {
		tsPtr->TheTime = -2;
	}
	TheCurrentTime = 0;
}

void TimeStamp::RecalibrateTime( long timeStep )
{
	assert( TheCurrentTime > ( LONG_MAX>>1)+timeStep );
	long timeShift = (TheCurrentTime - (LONG_MAX>>1)) + timeStep;
	TimeStampValue* tsPtr = TheTimeArray.GetFirstEntryPtr();
	for ( long i = TheTimeArray.SizeUsed(); i>0; i--, tsPtr++ ) {
		if ( tsPtr->TheTime >= 0 ) {
			long newTime = tsPtr->TheTime - timeShift;
			tsPtr->TheTime = newTime<0 ? -1 : newTime;
		}
	}
	TheCurrentTime -= timeShift;
	assert( TheCurrentTime>0 && TheCurrentTime<(LONG_MAX>>1) );
}
