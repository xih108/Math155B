
// Stack.h.   release 3.2.  May 3, 2007.
//
//   A general purpose dynamically resizable stack.
//   Implemented with templates.
//	Items are stored contiguously, for quick accessing.
//		However, allocation may require the stack to be copied into
//		new memory locations.
//
// Usage: Stack<T> or Stack<T,IdxVal>  
//
// Stores a denamically resizable stack of objects of type T.
// The second, optional, class parameter long is the data type used
//   to index into the array.  This defaults to "IdxVal", but "int" or "int32_t",
//   etc., may be used instead.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.




#ifndef STACK_H
#define STACK_H

#include <assert.h>

#include "../VrMath/MathMisc.h"

template <class T, class IdxVal = long> class Stack {

public:
	Stack();						// Constructor
	Stack(IdxVal initialSize);		// Constructor
	~Stack();					// Destructor

	void Reset();

	void Resize( IdxVal newMaxSize );	// Increases allocated size (will not decrease size)

	T& Top() const { assert( !IsEmpty() ); return *TopElement; };
	T* TopPtr() const { return IsEmpty() ? 0 : TopElement; }
	T& Pop();

	T* Push();					// New top element is arbitrary
	T* Push( const T& newElt );			// Push newElt onto stack.

	bool IsEmpty() const { return (SizeUsed==0); }

	IdxVal Size() const { return SizeUsed; }
	IdxVal SizeAllocated() const { return Allocated; }

private:

	IdxVal SizeUsed;				// Number of elements in the stack
	T* TopElement;				// Pointer to the top element of the stack
	IdxVal Allocated;				// Number of entries allocated
	T* TheStack;				// Pointer to the array of entries
};

template<class T, class IdxVal> inline Stack<T,IdxVal>::Stack()
{ 
	SizeUsed = 0;
	TheStack = 0;
	Allocated = 0;
	Resize( 10 );
}

template<class T, class IdxVal> inline Stack<T,IdxVal>::Stack(IdxVal initialSize)
{
	SizeUsed = 0;
	TheStack = 0;
	Allocated = 0;
	Resize( initialSize );
}


template<class T, class IdxVal> inline Stack<T,IdxVal>::~Stack()
{
	delete[] TheStack;
}

template<class T, class IdxVal> inline void Stack<T,IdxVal>::Reset()
{
	SizeUsed = 0;
	TopElement = TheStack-1;
}

template<class T, class IdxVal> inline void Stack<T,IdxVal>::Resize( IdxVal newMaxSize )
{
	if ( newMaxSize <= Allocated ) {
		return;
	}
	IdxVal newSize = Max(2*Allocated+1,newMaxSize);
	T* newArray = new T[newSize];
	T* toPtr = newArray;
	T* fromPtr = TheStack;
	IdxVal i;
	for ( i=0; i<SizeUsed; i++ ) {
		*(toPtr++) = *(fromPtr++);
	}
	delete[] TheStack;
	TheStack = newArray;
	Allocated = newSize;
	TopElement = TheStack+(SizeUsed-1);
}

template<class T, class IdxVal> inline T& Stack<T,IdxVal>::Pop()
{
	T* ret = TopElement;
	assert( SizeUsed>0 );		// Should be non-empty
	SizeUsed--;
	TopElement--;
	return *ret;
}

// Enlarge the stack but do not update the top element.
//    Returns a pointer to the top element (which is unchanged/uninitialized)
template<class T, class IdxVal> inline T* Stack<T,IdxVal>::Push( )
{
	if ( SizeUsed >= Allocated ) {
		Resize(SizeUsed+1);
	}
	SizeUsed++;
	TopElement++;
	return TopElement;
}

template<class T, class IdxVal> inline T* Stack<T,IdxVal>::Push( const T& newElt )
{
	Push();
	*TopElement = newElt;
	return TopElement;
}


#endif	// STACK_H