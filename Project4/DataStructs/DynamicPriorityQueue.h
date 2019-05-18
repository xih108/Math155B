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
// DynamicPriorityQueue.h
//

// Implements a dynamic priority queue and combined priority queue and array
// Items have (a) an index (that serves as a key, or unique identifier) and 
//            (b) a priority.
// The priority is accessed via the "operator<" method.  Only an ordering is needed,
//      no explicit priority values are required.
// Priority of items can be changed dynamically, but only one item's priority
//      should be changed at a time.  (Do this via the SetEntryMethod.)
// The objects stored in the priority queue have type T.
//    The class T must implement the < operator.
//	  The class T also must implement the = operator (assignment), and the < operator,
//    and have a default constructor.
// The second, optional, class parameter IdxVal is the data type used
//    to index into the array underlying the priority queue.  This 
//    defaults to "long", but "int" or "int32_t", etc., may be used instead.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.

#ifndef DYNAMIC_PRIORITY_QUEUE_H
#define DYNAMIC_PRIORITY_QUEUE_H

#include "PriorityQueue.h"
#include <assert.h>

template <class T, class IdxVal = long> class DynamicPriorityQueue
{
	friend class PriorityQueue<T,IdxVal>;
	class TwithIndex {
	public:
		T TheEntry;
		IdxVal TheIndex;
		bool operator<( const TwithIndex& other ) const { return (*this).TheEntry < other.TheEntry; }
	};

public:
	DynamicPriorityQueue();
	DynamicPriorityQueue( IdxVal initialSize );

	// Empty the entire queue. (Does not change the allocation size)
	void Reset();

	// Resize the priority queue and/or the array.
	void ResizeQueue( IdxVal newAllocSize );
	void ResizeIndexArray( IdxVal newAllocSize );
	void Resize( IdxVal newAllocSize ) { ResizeQueue( newAllocSize); ResizeIndexArray( newAllocSize ); }
	
	// Access or update items by their index:
	bool EntryPresent( IdxVal index ) const;
	const T* GetEntry( IdxVal index ) const;
	void SetEntry( IdxVal index, const T& newValue );	  // Use this call if the entry's priority changes.
	void ClearEntry( IdxVal index );
	void ClearTopEntry();
	void ClearTopEntry( IdxVal topIndex );	// Slightly faster (for internal use, really)

	// Use the next method *only* if the priority value is unchanged.
	void ChangeEntryValueUnsafe( IdxVal index, const T& newValue );	// Changes the entry with no checking

	// Priority Queue access methods
	
	// Check whether priority queue is empty.
	bool IsEmpty() const { return ThePQ.IsEmpty(); }
	// "GetTop" methods do not change the contents of the priority queue.
	const T* GetTop() const;
	IdxVal GetTopIndex() const;
	void GetTop( T* retT, IdxVal* retIndex ) const;
	// "PopTop" methods also remove the top element of the priority queue.
	//		They are equivalent to "GetTop..." follows by "ClearTopEntry"
	void PopTop( T* retT );
	IdxVal PopTopIndex();
	void PopTop( T* retT, IdxVal* retIndex );

	IdxVal QueueSize() const { return ThePQ.Size(); }
	void GetQueueEntry( IdxVal queueIndex, T* retT, IdxVal* retIndex ) const;

private:
	PriorityQueue<TwithIndex> ThePQ;
	Array<IdxVal> PtrsToPQ;

	void UpdatePQindices( IdxVal bubbleLow, IdxVal bubbleHigh );

	TwithIndex tempTwithIndex;	// Work variable

public:
	bool DEBUG_CheckWellFormed() const;
};

template <class T, class IdxVal>
inline DynamicPriorityQueue<T,IdxVal>::DynamicPriorityQueue()
{}

template <class T, class IdxVal>
inline DynamicPriorityQueue<T,IdxVal>::DynamicPriorityQueue( IdxVal initialSize )
: ThePQ(initialSize), PtrsToPQ(initialSize)
{}

template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::Reset()
{
	ThePQ.Reset();					// Empty out the priority queue

	IdxVal oldSize = PtrsToPQ.SizeUsed();
	PtrsToPQ.Reset();				// Empty out the array
	if ( oldSize>0 ) {
		ResizeIndexArray( oldSize );	//  and then fill it with -1's
	}
}

template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::ResizeQueue( IdxVal newAllocSize )
{
	ThePQ.Resize( newAllocSize );
}

template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::ResizeIndexArray( IdxVal newAllocSize )
{
	PtrsToPQ.Resize( newAllocSize );

	// Load the new entries with -1's to newAllocSize many entries.
	IdxVal i = PtrsToPQ.SizeUsed();
	PtrsToPQ.Touch( newAllocSize-1 );
	IdxVal* pointerPtr = PtrsToPQ.GetFirstEntryPtr()+i;
	for ( ; i<newAllocSize; i++, pointerPtr++ ) {
		*pointerPtr = -1;
	}
}

// Get the least (highest priority) element of the priority queue.
// Returns Null if the priority queue is empty.
template <class T, class IdxVal>
inline const T* DynamicPriorityQueue<T,IdxVal>::GetTop() const
{
	if ( ThePQ.Size() == 0 ) {
		return 0;
	}
	else {
		return &(ThePQ.GetArray().GetFirstEntry().TheEntry);
	}
}

template <class T, class IdxVal>
inline IdxVal DynamicPriorityQueue<T,IdxVal>::GetTopIndex() const
{
	if ( ThePQ.Size() == 0 ) {
		return -1;
	}
	else {
		return ThePQ.GetArray().GetFirstEntry().TheIndex;
	}
}

template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::GetTop( T* retT, IdxVal* retIndex ) const
{
	const TwithIndex* topEntry = ThePQ.Peek();
	if ( topEntry==0 ) {
		*retIndex = -1;
	}
	else {
		*retT = topEntry->TheEntry;
		*retIndex = topEntry->TheIndex;
	}
}

template <class T, class IdxVal>
inline bool DynamicPriorityQueue<T,IdxVal>::EntryPresent( IdxVal index ) const
{
	return (PtrsToPQ[index] != -1);
}

template <class T, class IdxVal>
inline const T* DynamicPriorityQueue<T,IdxVal>::GetEntry( IdxVal index ) const
{
	IdxVal indexPQ = PtrsToPQ[index];
	if ( indexPQ == -1 ) {
		return 0;
	}
	else {
		return &(ThePQ.GetArray().GetEntry(indexPQ).TheEntry);
	}
}

template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::SetEntry( IdxVal index, const T& newValue )
{
	// Load more uninitialized entries as needed
	if ( index >= PtrsToPQ.SizeUsed() ) {
		ResizeIndexArray( index+1 );
	}

	IdxVal indexPQ = PtrsToPQ[index];
	tempTwithIndex.TheIndex = index;
	tempTwithIndex.TheEntry = newValue;
	if ( indexPQ == -1 ) {
		// New value.
		IdxVal bubbleStart = ThePQ.Size();
		IdxVal bubbleEnd = ThePQ.Add( tempTwithIndex );
		UpdatePQindices( bubbleEnd, bubbleStart );
	}
	else {
		// Old value, is changing
		IdxVal bubbleEnd = ThePQ.BubbleIn( indexPQ, tempTwithIndex );
		if ( bubbleEnd<=indexPQ ) {
			UpdatePQindices( bubbleEnd, indexPQ );
		}
		else {
			UpdatePQindices( indexPQ, bubbleEnd );
		}
	}
}

template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::ChangeEntryValueUnsafe( IdxVal index, const T& newValue )
{
	// Changes the entry with no checking
	assert( index>=0 && index<PtrsToPQ.SizeUsed() );
	IdxVal indexPQ = PtrsToPQ.GetEntry( index );
	assert ( indexPQ != -1 );
	TwithIndex& theTwI = ThePQ.GetArray()[indexPQ];
	theTwI.TheEntry = newValue;
	return;
}

// Remove an entry from the Priority Queue.  
// If the index does not correspond to an entry, nothing is done.
template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::ClearEntry( IdxVal index )
{
	IdxVal indexPQ = PtrsToPQ[index];
	if ( indexPQ != -1 ) {
		// Value exists, and will be removed
		PtrsToPQ[index] = -1;
		if ( indexPQ+1 == ThePQ.Size() ) {
			// If removing the final element, just remove it and be done.
			ThePQ.GetArray().Pop();
		}
		else {
			// If not removing the final element
			const TwithIndex& theLastEntry = *(ThePQ.GetEntriesPtr()+ThePQ.Size()-1);
			ThePQ.GetArray().Pop();
			IdxVal bubbleEnd = ThePQ.BubbleIn( indexPQ, theLastEntry );
			if ( bubbleEnd<=indexPQ ) {
				UpdatePQindices( bubbleEnd, indexPQ );
			}
			else {
				UpdatePQindices( indexPQ, bubbleEnd );
			}
		}
	}
}

// Remove the first (highest priority) entry from the priority queue.
template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::ClearTopEntry()
{
	assert( ThePQ.Size()>0 );	// Priority Queue should not be empty.

	PtrsToPQ[ThePQ.GetArray().GetFirstEntry().TheIndex] = -1;
	if ( ThePQ.Size()==1 ) {
		ThePQ.GetArray().Pop();
	}
	else {
		const TwithIndex& theLastEntry = ThePQ.GetArray().Top();
		IdxVal destIndex = ThePQ.Pop();
		UpdatePQindices( 0, destIndex );
	}
}

// Remove the first (highest priority) entry from the priority queue.
template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::ClearTopEntry( IdxVal topIndex )
{
	assert( ThePQ.Size()>0 );	// Priority Queue should not be empty.

	PtrsToPQ[topIndex] = -1;
	const TwithIndex& theLastEntry = ThePQ.GetArray().Top();
	IdxVal destIndex = ThePQ.Pop();
	UpdatePQindices( 0, destIndex );	
}

// Elements in the priority queue have been bubbled between bubbleLow and bubbleHigh
template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::UpdatePQindices( IdxVal bubbleLow, IdxVal bubbleHigh )
{
	assert( bubbleLow<=bubbleHigh );
	assert( bubbleHigh<ThePQ.Size() );
	const TwithIndex* TheEntriesPQ = ThePQ.GetEntriesPtr();	
	while ( bubbleHigh>=bubbleLow ) {
		const TwithIndex thisEntry = *(TheEntriesPQ+bubbleHigh);
		PtrsToPQ[thisEntry.TheIndex] = bubbleHigh;			
		bubbleHigh = ((bubbleHigh-1)>>1);
	}
}

// "PopTop" methods also remove the top element of the priority queue.
template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::PopTop( T* retT )
{
	IdxVal retIndex;
	GetTop( retT, &retIndex );
	ClearTopEntry( retIndex );
}

template <class T, class IdxVal>
inline IdxVal DynamicPriorityQueue<T,IdxVal>::PopTopIndex()
{
	IdxVal retValue = GetTopIndex();
	ClearTopEntry( retValue );
}


template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::PopTop( T* retT, IdxVal* retIndex )
{
	GetTop( retT, retIndex );
	ClearTopEntry( *retIndex );
}

template <class T, class IdxVal>
inline bool DynamicPriorityQueue<T,IdxVal>::DEBUG_CheckWellFormed() const
{
	if ( ! ThePQ.DEBUG_CheckWellFormed() ) {
		return false;
	} 
	for ( IdxVal i=0; i<PtrsToPQ.SizeUsed(); i++ ) {
		IdxVal pointer = PtrsToPQ[i];
		if ( pointer != -1 ) {
			if ( pointer>=ThePQ.Size() ) {
				assert( false );
				return false;
			}
			const TwithIndex& thePQentry = ThePQ.GetArray()[pointer];
			if ( thePQentry.TheIndex != i ) {
				assert( false );
				return false;
			}
		}
	}
	for ( IdxVal i=0; i<ThePQ.Size(); i++ ) {
		const TwithIndex& thePQentry = ThePQ.GetArray()[i];
		IdxVal index = thePQentry.TheIndex;
		if ( PtrsToPQ[index] != i ) {
			assert ( false );
			return false;
		}
	}
	return true;
}

template <class T, class IdxVal>
inline void DynamicPriorityQueue<T,IdxVal>::GetQueueEntry( IdxVal queueIndex, T* retT, IdxVal* retIndex ) const
{
	const TwithIndex& theItem = ThePQ.GetArray()[queueIndex];
	*retT = theItem.TheEntry;
	*retIndex = theItem.TheIndex;
}


#endif // DYNAMIC_PRIORITY_QUEUE_H