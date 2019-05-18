/*
 *
 * RayTrace Software Package, release 3.3.  August 10, 2010.
 *
 * Data Structures Subpackage (DataStructs)
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

//
// PriorityQueue.h
//

// PriorityQueue<T> or PriorityQueue<T,IdxVal>  
//
// A dynamically resizable priority queue.  Based on the Array<T,IdxVal> class.
//  Allows finding the *least* element in the < ordering quickly.
//  Implemented with a standard binary heap structure.
//  The objects stored in the priority queue have type T.
//    The class T must implement the < operator.
//	  The class T also must implement the = operator (assignment), and the < operator,
//    and have a default constructor.
//  The second, optional, class parameter IdxVal is the data type used
//    to index into the array underlying the priority queue.  This 
//    defaults to "long", but "int" or "int32_t", etc., may be used instead.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.


#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "Array.h"
#include <assert.h>

template <class T, class IdxVal = long> class PriorityQueue 
{
public:
	PriorityQueue();						// Constructor
	PriorityQueue(IdxVal initialSize);		// Constructor

	// Empty the entire queue. (Does not change the allocation size)
	void Reset();
	void MakeEmpty() { Reset(); }

	// Next two routines: Main purpose to control allocation.
	//   Note: Allocation can occur without warning if you do not take care.
	//	 When allocation occurs, pointers and references become bad.
	// Use this judiciously to control when the queue may be resized
	void PreallocateMore( IdxVal numAdditional ) { Resize( Size()+numAdditional ); }
	void Resize( IdxVal newMaxSize );	// Increases allocated size (will not decrease allocated size)
	
	// Central access routines.
	//  Add - returns position where newEntry ends up.
	//  Pop - returns position where the final entry in the heap ends up.
	IdxVal Add( const T& newEntry );	// Adds this entry to the priority queue
	const T* Peek() const;			// Returns a pointer to the least element in < ordering
	IdxVal Pop();						// Removes the least (highest priority) element
	IdxVal Pop( T* retValue );		// Removes and returns the least element
	IdxVal Size() const;				// The number of elements in the priority queue
	bool IsEmpty() const;			// Test if the queue is empty.

	// Allow non-standard access to the queue entries.
	const T* GetEntriesPtr() const;		// Returns a pointer to the array of entries.
	Array<T,IdxVal>& GetArray();				// Returns the underlying array.
	const Array<T,IdxVal>& GetArray() const;	// Returns the underlying array.

	void RemoveLowPriorityItems( bool removalTest( const T& elt ) );

	// Mostly for internal use: 
	//   These do not have full support in the priority queue interface,
	//   for one example of how to use, see DynamicPriorityQueue class
	// These functions all return the position in the queue where the new element ended up.
	IdxVal BubbleInUp( IdxVal position, const T& newValue );
	IdxVal BubbleInDown( IdxVal position, const T& newValue );
	IdxVal BubbleIn( IdxVal position, const T& newValue );

private:
	Array<T,IdxVal> TheQueue;

public:
	bool DEBUG_CheckWellFormed() const;
};

// Default constructor
template <class T, class IdxVal>
inline PriorityQueue<T,IdxVal>::PriorityQueue()
{}

// Constructor with initial size
template <class T, class IdxVal>
inline PriorityQueue<T,IdxVal>::PriorityQueue( IdxVal initialSize )
: TheQueue( initialSize )
{}

// Empty the entire queue. (Does not change the allocation size)
template <class T, class IdxVal>
inline void PriorityQueue<T,IdxVal>::Reset()
{
	TheQueue.MakeEmpty();
}

// Increases allocated size (will not decrease allocated size)
template <class T, class IdxVal>
inline void PriorityQueue<T,IdxVal>::Resize( IdxVal newAllocSize )
{
	TheQueue.Resize(newAllocSize);
}

// Adds new entry to the priority queue
template <class T, class IdxVal>
inline IdxVal PriorityQueue<T,IdxVal>::Add( const T& newEntry )
{
	IdxVal newIndex = TheQueue.SizeUsed();
	TheQueue.Touch(newIndex);
	return BubbleInUp( newIndex, newEntry );
}

// Returns a reference to the least element in < ordering
template <class T, class IdxVal>
inline const T* PriorityQueue<T,IdxVal>::Peek() const
{
	if ( TheQueue.SizeUsed()>0 ) {
		return TheQueue.GetFirstEntryPtr();
	}
	else {
		return 0;
	}
}

// Removes the least (highest priority) element
// Should *not* be called in case TheQueue is empty!
template <class T, class IdxVal>
inline IdxVal PriorityQueue<T,IdxVal>::Pop()
{
	assert( TheQueue.SizeUsed()>0 );
	if ( TheQueue.SizeUsed()>1 ) {
		T& lastEntry = TheQueue.Pop();
		return BubbleInDown( 0, lastEntry );
	}
	else {
		TheQueue.Pop();
		return -1;
	}
}

// Removes and returns the least (highest priority) element
// Should *not* be called in case TheQueue is empty!
template <class T, class IdxVal>
inline IdxVal PriorityQueue<T,IdxVal>::Pop( T* retValue )
{
	assert( TheQueue.SizeUsed()>0 );
	*retValue = TheQueue.GetFirstEntry();
	return Pop();
}

// The number of elements in the priority queue
template <class T, class IdxVal>
inline IdxVal PriorityQueue<T,IdxVal>::Size() const
{
	return TheQueue.SizeUsed();
}

// Test if the queue is empty.
template <class T, class IdxVal>
inline bool PriorityQueue<T,IdxVal>::IsEmpty() const
{
	return TheQueue.IsEmpty();
}


template <class T, class IdxVal>
inline const T* PriorityQueue<T,IdxVal>::GetEntriesPtr() const
{
	return TheQueue.GetFirstEntryPtr();
}

template <class T, class IdxVal>
inline Array<T,IdxVal>& PriorityQueue<T,IdxVal>::GetArray()
{
	return TheQueue;
}

template <class T, class IdxVal>
inline const Array<T,IdxVal>& PriorityQueue<T,IdxVal>::GetArray() const
{
	return TheQueue;
}

// BubbleInUp
// Insert newValue into position.  This replaces the contents at "position".
//   The newValue should be less than or equal to the replaced value (i.e., *not* lower priority).
template <class T, class IdxVal>
inline IdxVal PriorityQueue<T,IdxVal>::BubbleInUp( IdxVal position, const T& newValue )
{
	assert( position<TheQueue.SizeUsed() );
	IdxVal curPos = position;
	T* curElt = TheQueue.GetFirstEntryPtr()+position;
	int i;
	for ( i=0; curPos>0; i++ ) {
		IdxVal nextPos = (curPos-1)>>1;				// nextpos = Parent(curPos)
		T* nextElt = TheQueue.GetFirstEntryPtr()+nextPos;
		if ( !(newValue < *nextElt) ) {
			break;									// Found end of bubble region
		}
		*curElt = *nextElt;							// Shift this item down one
		curPos = nextPos;
		curElt = nextElt;
	}
	*curElt = newValue;
	return curPos;
}

// BubbleInDown
// Insert newValue into position.  This replaces the contents at "position".
//   The newValue should be greater than or equal to the replaced value (i.e., *not* higher priority).
template <class T, class IdxVal>
inline IdxVal PriorityQueue<T,IdxVal>::BubbleInDown( IdxVal position, const T& newValue )
{
	assert( position<TheQueue.SizeUsed() );
	IdxVal size = TheQueue.SizeUsed();
	IdxVal curPos = position;
	T* curElt = TheQueue.GetFirstEntryPtr()+position;
	for(;;) {
		// nextPos and nextElt refer to the left child of the current node
		IdxVal nextPos = (curPos<<1)+1;
		if ( nextPos>=size ) {
			break;
		}
		T* nextElt = TheQueue.GetFirstEntryPtr()+nextPos;
		if ( nextPos<size-1 ) {
			T* nextEltRight = nextElt+1;
			if ( *nextEltRight < *nextElt ) {
				// Go to the right child instead.
				nextElt = nextEltRight;
				nextPos++;
			}
		}
		if ( !(*nextElt < newValue) ) {
			break;
		}
		*curElt = *nextElt;
		curPos = nextPos;
		curElt = nextElt;
	}
	*curElt = newValue;
	return curPos;
}

// Insert newValue into position.   This should be used if it is not known whether
//	the new value is higher or lower priority than the old value.
//  Basically the same as "BubbleInUp", but with a check to see of BubbleInDown should be
//		used instead.
template <class T, class IdxVal>
inline IdxVal PriorityQueue<T,IdxVal>::BubbleIn( IdxVal position, const T& newValue )
{
	assert( position<TheQueue.SizeUsed() );
	if ( position==0 ) {
		return BubbleInDown( position, newValue );
	}
	IdxVal startPos = position;
	T* startElt = TheQueue.GetFirstEntryPtr()+position;
	IdxVal curPos = (startPos-1)>>1;				// curPos = Parent(startPos)
	T* curElt = TheQueue.GetFirstEntryPtr()+curPos;
	if ( !(newValue < *curElt) ) {
		return BubbleInDown( position, newValue );
	}
	*startElt = *curElt;
	int i;
	for ( i=0; curPos>0; i++ ) {
		IdxVal nextPos = (curPos-1)>>1;				// nextpos = Parent(curPos)
		T* nextElt = TheQueue.GetFirstEntryPtr()+nextPos;
		if ( !(newValue < *nextElt) ) {
			break;									// Found end of bubble region
		}
		*curElt = *nextElt;							// Shift this item down one
		curPos = nextPos;
		curElt = nextElt;
	}
	*curElt = newValue;
	return curPos;

}

template <class T, class IdxVal>
inline void PriorityQueue<T,IdxVal>::RemoveLowPriorityItems( bool removalTest( const T& elt ) )
{
	if ( TheQueue.IsEmpty() ) {
		return;
	}
	IdxVal curIndex = TheQueue.SizeUsed()-1;
	IdxVal topIndex = curIndex;
	T* curPtr = TheQueue.GetFirstEntryPtr()+curIndex;
	T* topPtr = curPtr;
	IdxVal topChildIndex = topIndex>0 ? (topIndex-1)>>1 : -1;
	do {
		if ( !removalTest(*curPtr) ) {
			curIndex--;
			curPtr--;
		}
		else {
			TheQueue.ReducedSizeUsed( topIndex );
			if ( curIndex<topIndex ) {
				BubbleInUp( curIndex, *topPtr);
			}
			else {
				curIndex--;
				curPtr--;
			}
			topIndex--;
			topPtr--;
			topChildIndex = topIndex>0 ? (topIndex-1)>>1 : -1;
		}

	} while ( curIndex>topChildIndex );
}

template <class T, class IdxVal>
inline bool PriorityQueue<T,IdxVal>::DEBUG_CheckWellFormed() const
{
	const T* Tptr = TheQueue.GetFirstEntryPtr()+1;
	for ( IdxVal i=1; i<Size(); i++, Tptr++ ) {
		IdxVal parent = (i-1)>>1;
		const T& parentT = TheQueue[parent];
		if ( *Tptr < parentT ) {
			assert( false );
			return false;
		}
	}
	return true;
}


#endif // PRIORITY_QUEUE_H