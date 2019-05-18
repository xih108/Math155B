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

// Array.h
//
//   A general purpose dynamically resizable array.
//   Implemented with templates.
//	Items are stored contiguously, for quick indexing.
//		However, allocation may require the array to be copied into
//		new memory locations.
//	Because of the dynamic resizing, you should be careful to understand
//		how the array code works before using it.  Care should be taken 
//		if the array might be resized during an expression evaluation.
//
// Usage: Array<T> or Array<T,IdxVal>  
//
//  The class T is the type of objects stored in the array
//  The second, optional, class parameter IdxVal is the data type used
//     to index into the array.  This defaults to "long", but "int" or "int32_t",
//     etc., may be used instead.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.


#ifndef ARRAY_H
#define ARRAY_H

#include <assert.h>
#include <stdlib.h>

#include "../VrMath/MathMisc.h"

template <class T, class IdxVal = long> class Array {

public:
	Array();						// Constructor
	Array(IdxVal initialSize);		// Constructor
	Array( const Array<T,IdxVal>& );		// Constructor (presently disabled)
	~Array();						// Destructor

	bool InitialAllocationSucceeded() const { return (TheEntries!=0); }

	void Reset() { ReducedSizeUsedNoCheck(0); }
	void ResetAndDelete();

	// Next two routines: Main purpose to control allocation.
	//   Note: Allocation can occur without warning if you do not take care.
	//	 When allocation occurs, pointer and references become bad.
	// Use this judiciously to control when the array may be resized
	bool PreallocateMore( IdxVal numAdditional ) { return Resize( SizeUsed()+numAdditional ); }
	bool Resize( IdxVal newMaxSize );	// Increases allocated size (will not decrease allocated size)

	// ExplicitAllocate allows for taking control of the array allocation
	void ExplicitAllocate( T* newArrayLocation, IdxVal newAllocSize );
	void ExplicitAllocateNoCopy( T* newArrayLocation, IdxVal newAllocSize );
	void ExplicitAllocateNoCopy( T* newArrayLocation, IdxVal newAllocSize, IdxVal newSizeUsed );
	void ExplicitAllocateDelete( T* newArrayLocation, IdxVal newAllocSize );
	void ExplicitReduceSize( IdxVal reducedAllocatedSize );

	// Next routines are used to update the "SizeUsed" of the stack.
	void Touch( IdxVal i );			// Makes entry i active.  Increases SizeUsed (MaxEntryPlus)
	void ReducedSizeUsed( IdxVal i ); // "i" is the number of used entries now.
	void ReducedSizeUsedNoCheck( IdxVal i ); // "i" is the number of used entries now.
	void ChangeSizeUsed( IdxVal i );			// "i" is the number of used entries now.
	void ChangeSizeUsedNoCheck( IdxVal i ); // "i" is the number of used entries now.
	void ChangeSizeUsedNoCheckNoAssert( IdxVal i ); // "i" is the number of used entries now.
	void MakeEmpty() {MaxEntryPlus=0;};	// Makes the length "SizeUsed" equal zero.
	// Next nine functions give the functionality of a stack.
	T& Top() { assert(MaxEntryPlus>0); return *(TheEntries+(MaxEntryPlus-1)); }
	const T& Top() const { assert(MaxEntryPlus>0); return *(TheEntries+(MaxEntryPlus-1)); }
	T& Pop();
	T* Push();					// Push with no argument returns pointer to new top element
	T* Push( const T& newElt );
	T* PushUnsafe();			// "Unsafe" versions do not check for exceeding size of array!
	T* PushUnsafe( const T& newElt );
	bool IsEmpty() const { return (MaxEntryPlus==0); }
	T& NextTop() { return *(TheEntries+MaxEntryPlus); }
	const T& NextTop() const { assert(MaxEntryPlus>0); return *(TheEntries+MaxEntryPlus); }

	void DisallowDynamicResizing() { DynamicResizingOK = false; }
	void AllowDynamicResizing() { DynamicResizingOK = true; }

	// Access function - if i is out of range, it resizes
	T& operator[]( IdxVal i );
	const T& operator[]( IdxVal i ) const;

	// Access functions - Does not check whether i is out of range!!
	//		No resizing will occur!  Use only to get existing entries.
	T& GetEntry( IdxVal i );
	const T& GetEntry( IdxVal i ) const;
	T& GetFirstEntry();
	const T& GetFirstEntry() const;
	T& GetLastEntry();
	const T& GetLastEntry() const;
	T* GetFirstEntryPtr();
	const T* GetFirstEntryPtr() const;
	T* GetEntryPtr( IdxVal i );
	const T* GetEntryPtr( IdxVal i ) const;
	T* GetTheEntries();
	const T* GetTheEntries() const;

	IdxVal SizeUsed() const;			// Number of elements used (= 1+(max index));

	IdxVal AllocSize() const;			// Size allocated for the array
	bool IsFull() const { return MaxEntryPlus==Allocated; }
	// IdxVal SizeAvailable() const { return Allocated-MaxEntryPlus; }

	Array<T,IdxVal>& operator=( const Array<T,IdxVal>& other );

	void SwapContents( Array<T,IdxVal>& other);

	// Higher-level functions
	//   HasMember was formerly named "IsMember"
	bool HasMember( const T& queryElt ) const;	// Check if present in array
	void Append( const Array<T,IdxVal>& fromArray );	// Append entire array contents

private:

	IdxVal MaxEntryPlus;				// Maximum entry used, plus one (Usually same as size)
	IdxVal Allocated;					// Number of entries allocated
	T* TheEntries;					// Pointer to the array of entries

	bool DynamicResizingOK;			// If "true", array can be dynamically resized.
	bool TheEntriesExternallyHandled;	// True if "Explicit" allocation methods used.

};

template<class T, class IdxVal> inline Array<T,IdxVal>::Array()
{ 
	MaxEntryPlus = 0;
	TheEntries = 0;
	Allocated = 0;
	DynamicResizingOK = true;
	TheEntriesExternallyHandled = false;
	Resize( 10 );
}

template<class T, class IdxVal> inline Array<T,IdxVal>::Array(IdxVal initialSize)
{
	MaxEntryPlus = 0;
	TheEntries = 0;
	Allocated = 0;
	DynamicResizingOK = true;
	TheEntriesExternallyHandled = false;
	Resize( initialSize );
	assert( initialSize==0 || TheEntries!=0 );
}


template<class T, class IdxVal> inline Array<T,IdxVal>::~Array()
{
	if ( !TheEntriesExternallyHandled ) {
		delete[] TheEntries;
	}
}

template<class T, class IdxVal> inline bool Array<T,IdxVal>::Resize( IdxVal newMaxSize )
{
	assert( newMaxSize>=0 );
	if ( newMaxSize <= Allocated ) {
		return false;				// No actual reallocation is needed.
	}
	if ( !DynamicResizingOK ) {
		assert(false);
		exit(0);
	}
	Allocated = Max(2*Allocated+1,newMaxSize);
	T* newArray = new T[Allocated];
	if ( newArray==0 ) {
		return false;			// Return false to indicate that the reallocation failed.
	}
	T* toPtr = newArray;
	T* fromPtr = TheEntries;
	for ( IdxVal i=MaxEntryPlus; i>0; i-- ) {
		*(toPtr++) = *(fromPtr++);
	}
	assert( !TheEntriesExternallyHandled );
	delete[] TheEntries;
	TheEntries = newArray;	
	return true;				// Return true to indicate reallocation occurred.
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ResetAndDelete()
{
	MaxEntryPlus = 0;
	Allocated = 0;
	delete TheEntries;
	TheEntries = 0;
}

// ExplicitAllocate allows for taking control of the array allocation
template<class T, class IdxVal> inline void Array<T,IdxVal>::ExplicitAllocate( T* newArrayLocation, IdxVal newAllocSize )
{
	assert( newAllocSize>=MaxEntryPlus );
	T* toPtr = newArrayLocation;
	T* fromPtr = TheEntries;
	for ( IdxVal i=MaxEntryPlus; i>0; i-- ) {
		*(toPtr++) = *(fromPtr++);
	}
	Allocated = newAllocSize;
	TheEntries = newArrayLocation;
	DynamicResizingOK = false;
	TheEntriesExternallyHandled = true;
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ExplicitAllocateNoCopy( T* newArrayLocation, IdxVal newAllocSize )
{
	assert( newAllocSize>=MaxEntryPlus );
	Allocated = newAllocSize;
	TheEntries = newArrayLocation;
	DynamicResizingOK = false;
	TheEntriesExternallyHandled = true;
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ExplicitAllocateNoCopy( T* newArrayLocation, IdxVal newAllocSize, IdxVal newSizeUsed )
{
	assert( newAllocSize>=newSizeUsed );
	Allocated = newAllocSize;
	TheEntries = newArrayLocation;
	MaxEntryPlus = newSizeUsed;
	DynamicResizingOK = false;
	TheEntriesExternallyHandled = true;
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ExplicitAllocateDelete( T* newArrayLocation, IdxVal newAllocSize ) {
	T* oldEntries = TheEntries;
	ExplicitAllocate( newArrayLocation, newAllocSize );
	delete oldEntries;
	TheEntriesExternallyHandled = true;
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ExplicitReduceSize( IdxVal reducedAllocatedSize ) {
	assert ( reducedAllocatedSize>=MaxEntryPlus );
	Allocated = reducedAllocatedSize;
	assert( TheEntriesExternallyHandled == true );
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::Touch( IdxVal i )
{
	assert( i >= 0 );
	if ( i>=Allocated ) {
		Resize( i+1 );
	}
	if ( i >= MaxEntryPlus ) {
		MaxEntryPlus = i+1;
	}
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ReducedSizeUsed( IdxVal i )
{
	// "i" is the number of used entries now.
	assert( i >= 0 );
	if ( i<MaxEntryPlus ) {
		MaxEntryPlus = i;
	}
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ReducedSizeUsedNoCheck( IdxVal i )
{
	// "i" is the number of used entries now.
	assert( i >= 0 && i <= MaxEntryPlus );
	MaxEntryPlus = i;
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ChangeSizeUsed( IdxVal i )
{
	// "i" is the number of used entries now.
	Resize(i);
	MaxEntryPlus = i;
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ChangeSizeUsedNoCheck( IdxVal i )
{
	// "i" is the number of used entries now.
	assert( i >= 0 && i <= Allocated );
	MaxEntryPlus = i;
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::ChangeSizeUsedNoCheckNoAssert( IdxVal i )
{
	// "i" is the number of used entries now.
	MaxEntryPlus = i;
}

template<class T, class IdxVal> inline T& Array<T,IdxVal>::Pop()
{
	assert( MaxEntryPlus>0 );
	MaxEntryPlus-- ;
	return *(TheEntries+MaxEntryPlus);
}

template<class T, class IdxVal> inline T* Array<T,IdxVal>::Push()
{
	if ( MaxEntryPlus>=Allocated ) {
		Resize( MaxEntryPlus+1 );
	}
	T* ret = TheEntries+MaxEntryPlus;
	MaxEntryPlus++;
	return ret;
}

template<class T, class IdxVal> inline T* Array<T,IdxVal>::Push( const T& newElt )
{
	T* top = Push();
	*top = newElt;
	return top;
}

template<class T, class IdxVal> inline T* Array<T,IdxVal>::PushUnsafe()
{
	assert ( MaxEntryPlus<Allocated );
	T* ret = TheEntries+MaxEntryPlus;
	MaxEntryPlus++;
	return ret;
}

template<class T, class IdxVal> inline T* Array<T,IdxVal>::PushUnsafe( const T& newElt )
{
	T* top = PushUnsafe();
	*top = newElt;
	return top;
}

template<class T, class IdxVal> inline T& Array<T,IdxVal>::operator[]( IdxVal i )
{
	assert( i >= 0 );
	if ( i>=Allocated ) {
		Resize( i+1 );
	}
	if ( i >= MaxEntryPlus ) {
		MaxEntryPlus = i+1;
	}
	return TheEntries[i];
}

template<class T, class IdxVal> inline const T& Array<T,IdxVal>::operator[]( IdxVal i ) const
{
	assert( i >= 0 );
	if ( i>=Allocated ) {
		const_cast<Array<T,IdxVal>*>(this)->Resize( i+1 );
	}
	if ( i >= MaxEntryPlus ) {
		const_cast<Array<T,IdxVal>*>(this)->MaxEntryPlus = i+1;
	}
	return TheEntries[i];
}

template<class T, class IdxVal> inline T& Array<T,IdxVal>::GetEntry( IdxVal i )
{
	assert( i >= 0 );
	assert ( i < MaxEntryPlus );
	return TheEntries[i];
}

template<class T, class IdxVal> inline const T& Array<T,IdxVal>::GetEntry( IdxVal i ) const
{
	assert( i >= 0 );
	assert ( i < MaxEntryPlus );
	return TheEntries[i];
}

template<class T, class IdxVal> inline T& Array<T,IdxVal>::GetFirstEntry()
{
	assert(Allocated>0);
	return *TheEntries;
}

template<class T, class IdxVal> inline const T& Array<T,IdxVal>::GetFirstEntry() const
{
	assert(Allocated>0);
	return *TheEntries;
}

template<class T, class IdxVal> inline T* Array<T,IdxVal>::GetTheEntries()
{
	return TheEntries;
}

template<class T, class IdxVal> inline const T* Array<T,IdxVal>::GetTheEntries() const
{
	return TheEntries;
}

template<class T, class IdxVal> inline T* Array<T,IdxVal>::GetFirstEntryPtr()
{
	assert(Allocated>0);
	return TheEntries;
}

template<class T, class IdxVal> inline const T* Array<T,IdxVal>::GetFirstEntryPtr() const
{
	assert(Allocated>0);
	return TheEntries;
}

template<class T, class IdxVal> inline T* Array<T,IdxVal>::GetEntryPtr( IdxVal i )
{
	assert( i >= 0 );
	assert( i < MaxEntryPlus );
	return TheEntries+i;
}

template<class T, class IdxVal> inline const T* Array<T,IdxVal>::GetEntryPtr( IdxVal i ) const
{
	assert( i >= 0 );
	assert( i < MaxEntryPlus );
	return TheEntries+i;
}

template<class T, class IdxVal> inline T& Array<T,IdxVal>::GetLastEntry()
{
	assert(MaxEntryPlus>0);
	return *(TheEntries+(MaxEntryPlus-1));
}

template<class T, class IdxVal> inline const T& Array<T,IdxVal>::GetLastEntry() const
{ 
	assert(MaxEntryPlus>0);
	return *(TheEntries+(MaxEntryPlus-1));
}

template<class T, class IdxVal> inline IdxVal Array<T,IdxVal>::SizeUsed () const
{
	return MaxEntryPlus;
}

template<class T, class IdxVal> inline IdxVal Array<T,IdxVal>::AllocSize () const
{
	return Allocated;
}

// Check if queryElt present in array
template<class T, class IdxVal> inline bool Array<T,IdxVal>::HasMember( const T& queryElt ) const
{
	IdxVal i;
	T* tPtr = TheEntries;
	for ( i=MaxEntryPlus; i>0; i--, tPtr++) {
		if ( (*tPtr) == queryElt ) {
			return true;
		}
	}
	return false;
}

// Append entire Array contents from fromArray to "this" Array
template<class T, class IdxVal> inline void Array<T,IdxVal>::Append( const Array<T,IdxVal>& fromArray )
{
	IdxVal newSizeUsed = MaxEntryPlus+fromArray.MaxEntryPlus;
	Resize( newSizeUsed );
	IdxVal n = fromArray.MaxEntryPlus;
	T* toPtr = TheEntries+MaxEntryPlus;
	MaxEntryPlus = newSizeUsed;
	const T* fromPtr = fromArray.TheEntries;
	while ( (n--)>0 ) {
		(*(toPtr++)) = *(fromPtr++);
	}
}


// My thanks to Hans Dietrich for reporting a bug in an earlier untested version,
//    and assisting with the fix.
template<class T, class IdxVal> inline Array<T,IdxVal>& Array<T,IdxVal>::operator=( const Array<T,IdxVal>& other )
{
	if ( TheEntries != other.TheEntries ) {
		Resize( other.MaxEntryPlus );
		MaxEntryPlus = other.MaxEntryPlus;
		IdxVal i;
		T* toPtr = TheEntries;
		const T* fromPtr = other.TheEntries;
		for ( i=MaxEntryPlus; i>0; i--, toPtr++, fromPtr++ ) {
			*toPtr = *fromPtr;
		}
	}
	return *this;
}

// copy ctor - Supplied by Hans Dietrich (Thanks!)
//  However in the present version below, I added an "assert(false)"
//  because it is easy to inadvertantly use a copy constructor 
//  and this is often a mistake for performance reasons.
// If you wish to use this, comment out the assert.
template<class T, class IdxVal> inline Array<T,IdxVal>::Array(const Array<T,IdxVal>& other)
{
	assert ( false );
    TheEntries = 0;
    Allocated = 0;
    DynamicResizingOK = other.DynamicResizingOK;
    Resize(other.MaxEntryPlus);
    MaxEntryPlus = other.MaxEntryPlus;

    T* toPtr = TheEntries;
    const T* fromPtr = other.TheEntries;

    for (int i = MaxEntryPlus; i > 0; i--, toPtr++, fromPtr++) 
    {
        *toPtr = *fromPtr;
    }
}

template<class T, class IdxVal> inline void Array<T,IdxVal>::SwapContents( Array<T,IdxVal>& other )
{
	IdxVal temp_IdxVal;
	temp_IdxVal = MaxEntryPlus;
	MaxEntryPlus = other.MaxEntryPlus;
	other.MaxEntryPlus = temp_IdxVal;
	temp_IdxVal = Allocated;
	Allocated = other.Allocated;
	other.Allocated = temp_IdxVal;

	bool temp_bool;
	temp_bool = DynamicResizingOK;
	DynamicResizingOK = other.DynamicResizingOK;
	other.DynamicResizingOK = temp_bool;
	temp_bool = TheEntriesExternallyHandled;
	TheEntriesExternallyHandled = other.TheEntriesExternallyHandled;
	other.TheEntriesExternallyHandled = temp_bool;

	T* temp_ptr = TheEntries;
	TheEntries = other.TheEntries;
	other.TheEntries = temp_ptr;
}


#endif // ARRAY_H