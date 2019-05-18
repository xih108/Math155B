//
// TimeStamp.h
//

// A time stamp routine that automatically rolls over 
//	when run past limits.

#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include "../DataStructs/Array.h"

class TimeStampValue {
	friend class TimeStamp;
	public:
		TimeStampValue() { TheTime = -2; }
	private:
		long TheTime;	// -2 means never stamped, -1 means stamped only a long time ago
};

class TimeStamp
{
public:
	TimeStamp() { TheCurrentTime = 0; }
	TimeStamp( long initialSize ) : TheTimeArray(initialSize) { TheCurrentTime=0; };

	void SetSize( long newSize );

	void Reset();

	void IncrementTime();
	void IncrementTime( long timeStep );

	bool IsCurrent( long index ) const;

	// Time elapsed since "touch", or LONG_MAX-1 or LONG_MAX for long ago or never.	
	long TimeSinceTouch( long index ) const;
	long TimeSinceTouch( const TimeStampValue& tsv ) const;

	// Time elapsed since "touch", or -1 or -2 for long ago or never.	
	// Deprecated in favor of TimeSinceTouch(index).
	long TimeElapsed_Deprecated( long index ) const;

	// Mark with current time
	void Touch( long index );				// Touch with current time
	void Touch( TimeStampValue& tsv );		// Touch with current time
	bool TouchNewlyCurrent( long index );	// Touch with current time and check if newly current

	// Touch the item at "pastTimeDelta" steps in the past.
	//		pastTimeDelta should be non-negative
	void TouchBack( long index, long pastTimeDelta );
	void TouchBack( TimeStampValue& tsv , long pastTimeDelta );

	// Report the old time elapsed, and set to the current time.
	// Returns LONG_MAX-1 or LONG_MAX for "long ago" or "never"
	long TestAndTouch( long index );

	// Obtain a pointer to the time stamp value.
	// There is no checking of array size limits.
	TimeStampValue& GetTimeStamp( long index );
	const TimeStampValue& GetTimeStamp( long index ) const;

private:
	long TheCurrentTime;
	Array<TimeStampValue> TheTimeArray;

private:
	void RecalibrateTime( long timeStep );
	long TimeSinceTouchCalc( long time ) const;

private:
	static const long MaxTime = LONG_MAX-2;

};

inline void TimeStamp::IncrementTime()
{
	IncrementTime( 1 );
}

inline void TimeStamp::IncrementTime( long timeStep )
{
	assert( timeStep>0 && timeStep<(MaxTime>>2) );
	if ( TheCurrentTime>=MaxTime-timeStep ) {
		RecalibrateTime( timeStep );
	}
	assert( TheCurrentTime+timeStep>TheCurrentTime );
	assert( TheCurrentTime+timeStep<=MaxTime );
	TheCurrentTime += timeStep;
}

inline void TimeStamp::Touch( long index )
{
	TheTimeArray[index].TheTime = TheCurrentTime;
}

inline void TimeStamp::Touch( TimeStampValue& tsv )
{
	assert ( &tsv>=TheTimeArray.GetFirstEntryPtr() && &tsv<=TheTimeArray.GetEntryPtr(TheTimeArray.SizeUsed()-1) );
	tsv.TheTime = TheCurrentTime;
}

inline void TimeStamp::TouchBack( long index, long pastTimeDelta )
{
	assert( pastTimeDelta>=0 );
	assert( pastTimeDelta<=TheCurrentTime );
	TheTimeArray[index].TheTime = TheCurrentTime-pastTimeDelta;
}

inline void TimeStamp::TouchBack( TimeStampValue& tsv , long pastTimeDelta ) 
{
	assert ( &tsv>=TheTimeArray.GetFirstEntryPtr() && &tsv<=TheTimeArray.GetEntryPtr(TheTimeArray.SizeUsed()-1) );
	assert( pastTimeDelta>=0 );
	assert( pastTimeDelta<=TheCurrentTime );
	tsv.TheTime = TheCurrentTime-pastTimeDelta;
}


// Return false if already was current, true otherwise.  
// Make current in any event.
inline bool TimeStamp::TouchNewlyCurrent( long index )
{
	TimeStampValue& theEntry = TheTimeArray[index];
	if ( theEntry.TheTime==TheCurrentTime ) {
		return false;
	}
	else {
		theEntry.TheTime = TheCurrentTime;
		return true;
	}
}

inline long TimeStamp::TestAndTouch( long index )
{
	TimeStampValue& theEntry = TheTimeArray[index];
	long ret;
	long time = TheTimeArray.GetEntry(index).TheTime;
	if ( time<0 ) {
		assert( time==-1 || time==-2 );
		ret = (LONG_MAX-2)-time;		// Value will be LONG_MAX-1 or LONG_MAX for "long ago" or "never"
	}
	else {
		assert( TheCurrentTime>=time );
		ret = TheCurrentTime-time;
	}
	theEntry.TheTime = TheCurrentTime;
	return ret;
}

inline bool TimeStamp::IsCurrent( long index ) const
{
	assert( index>=0 );
	if ( index >= TheTimeArray.SizeUsed() ) {
		return false;
	}
	return TheTimeArray.GetEntry(index).TheTime==TheCurrentTime;
}

// Return value can be LONG_MAX-1 or LONG_MAX for "long ago" or "never"
inline long TimeStamp::TimeSinceTouch( const TimeStampValue& tsv ) const
{
	assert ( &tsv>=TheTimeArray.GetFirstEntryPtr() && &tsv<=TheTimeArray.GetEntryPtr(TheTimeArray.SizeUsed()-1) );
	return TimeSinceTouchCalc( tsv.TheTime );
}

// Return value can be LONG_MAX-1 or LONG_MAX for "long ago" or "never"
inline long TimeStamp::TimeSinceTouch( long index ) const
{
	assert( index>=0 );
	if ( index>=TheTimeArray.SizeUsed() ) {
		return LONG_MAX;
	}
	return TimeSinceTouchCalc( TheTimeArray.GetEntry(index).TheTime );
}

inline long TimeStamp::TimeSinceTouchCalc( long time ) const
{
	if ( time<0 ) {
		assert( time==-1 || time==-2 );
		return (LONG_MAX-2)-time;		// Value will be LONG_MAX-1 or LONG_MAX for "long ago" or "never"
	}
	else {
		assert( TheCurrentTime>=time );
		return TheCurrentTime-time;
	}
}

// Doesn't accomplish much if newSize is less than allocated size
inline void TimeStamp::SetSize( long newSize )
{
	TheTimeArray.ChangeSizeUsed( newSize );
}

inline TimeStampValue& TimeStamp::GetTimeStamp( long index )
{
	assert( index>=0 && index<TheTimeArray.SizeUsed() );
	return TheTimeArray.GetEntry( index );
}

inline const TimeStampValue& TimeStamp::GetTimeStamp( long index ) const
{
	assert( index>=0 && index<TheTimeArray.SizeUsed() );
	return TheTimeArray.GetEntry( index );
}


inline long TimeStamp::TimeElapsed_Deprecated( long index ) const
{
	assert( index>=0 );
	if ( index>=TheTimeArray.SizeUsed() ) {
		return -2;
	}
	long time = TheTimeArray.GetEntry(index).TheTime;
	if ( time<0 ) {
		return time;		// Value will be -1 or -2 for "long ago" or "never"
	}
	else {
		assert( TheCurrentTime>=time );
		return TheCurrentTime-time;
	}
}

#endif // TIME_STAMP_H