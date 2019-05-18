// Queue.h.   September 2007.
//
//   A general purpose dynamically resizable FIFO queue.
//   Implemented with templates.
//	Items are stored contiguously, for quick accessing.
//		However, allocation may require the stack to be copied into
//		new memory locations.
//
// Usage: Queue<T> or Queue<T,IdxVal>  
//
// Stores a denamically resizable queue of objects of type T.
// The second, optional, class parameter long is the data type used
//   to index into the queue.  This defaults to "IdxVal", but "int" or "int32_t",
//   etc., may be used instead.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.
// Code is "as is" with no warranties of any kind.

#ifndef QUEUE_H
#define QUEUE_H

#include <assert.h>

#include "../VrMath/MathMisc.h"

template <class T, class IdxVal = long> class Queue {

public:
	Queue();						// Constructor
	Queue(IdxVal initialSize);		// Constructor
	~Queue();						// Destructor

	void Reset();
	void MakeEmpty() { Reset(); }

	void Resize( IdxVal newMaxSize );	// Increases allocated size (will not decrease size)
	
	T* Push();						// New top element is arbitrary
	T* Push( const T& newElt );		// Push newElt onto stack.

	T& Pop();					
	T& Head() { assert(SizeUsedCounter>0); return *HeadElement; }
	const T& Head() const { assert(SizeUsedCounter>0); return *HeadElement; }

	bool IsEmpty() const { return (SizeUsedCounter==0); }

	IdxVal Size() const { return SizeUsedCounter; }
	IdxVal SizeUsed() const { return SizeUsedCounter; }
	IdxVal SizeAllocated() const { return Allocated; }

private:

	// The Queue is a circular list, elements are pushed at the left end 
	//				(lowest index end, if the circularity is not in effect)
	//    and removed from the right end (highest index end).
	// NextTailElement means the entry that will be filled by the next Push() operation.
	// HeadElement means the entry that will be returned by the next Pop() operation.

	IdxVal SizeUsedCounter;		// Number of elements in the queue
	IdxVal Allocated;				// Number of entries allocated
	T* TheQueue;				// Pointer to the array of entries
	T* NextTailElement;			// Pointer to the next tail element of the queue
	T* HeadElement;				// Pointer to the head (next to remove) element of the queue
};

template<class T, class IdxVal> inline Queue<T,IdxVal>::Queue()
{
	SizeUsedCounter = 0;
	TheQueue = 0;
	Allocated =0;
	Resize( 10 );
}


template<class T, class IdxVal> inline Queue<T,IdxVal>::Queue(IdxVal initialSize)
{
	SizeUsedCounter = 0;
	TheQueue = 0;
	Allocated =0;
	Resize( initialSize );
}

template<class T, class IdxVal> inline Queue<T,IdxVal>::~Queue()
{
	delete[] TheQueue;
}

template<class T, class IdxVal> inline void Queue<T,IdxVal>::Reset()
{
	SizeUsedCounter = 0;

	// Both pointers point to the last entry (unused).  The queue will march towards the beginning.
	T* theQueueEnd = TheQueue+(Allocated-1);
	NextTailElement = theQueueEnd;
	HeadElement = theQueueEnd;
}

// Increases allocated size (will not decrease size)
template<class T, class IdxVal> inline void Queue<T,IdxVal>::Resize( IdxVal newMaxSize )
{
	if ( newMaxSize <= Allocated ) {
		return;
	}
	IdxVal newSize = Max(2*Allocated+1,newMaxSize);
	T* newArray = new T[newSize];
	T* theQueueEnd = newArray+(newSize-1);

	T* fromPtr = HeadElement+1;
	T* toPtr = theQueueEnd;
	HeadElement = toPtr;					// New head element
	IdxVal i=SizeUsedCounter;
	while ( i>0 && fromPtr!=TheQueue ) { 
		*(toPtr--) = *(--fromPtr);
		i--;
	}
	fromPtr = TheQueue+Allocated;
	while ( i>0 ) {
		*(toPtr--) = *(--fromPtr);
		i--;
	}
	NextTailElement = toPtr;

	delete[] TheQueue;
	TheQueue = newArray;
	Allocated = newSize;
}
	
// Allocate space for a new first entry
template<class T, class IdxVal> inline T* Queue<T,IdxVal>::Push()
{
	if ( SizeUsedCounter==Allocated ) {
		Resize( SizeUsedCounter+1 );
	}
	T* ret = NextTailElement;
	if ( NextTailElement == TheQueue ) {
		NextTailElement += (Allocated-1);
	}
	else {
		NextTailElement--;
	}
	SizeUsedCounter++;
	return ret;
}

// Insert a new first entry
template<class T, class IdxVal> inline T* Queue<T,IdxVal>::Push( const T& newElt )
{
	T* ret = Push();
	*ret = newElt;
	return ret;
}

template<class T, class IdxVal> inline T& Queue<T,IdxVal>::Pop()
{
	assert( SizeUsedCounter>0 );
	T* ret = HeadElement;
	if ( HeadElement==TheQueue ) {
		HeadElement = TheQueue+(Allocated-1);
	}
	else {
		HeadElement--;
	}
	SizeUsedCounter--;
	assert( (SizeUsedCounter==0) == (HeadElement==NextTailElement) );
	return *ret;	
}

#endif // QUEUE_H