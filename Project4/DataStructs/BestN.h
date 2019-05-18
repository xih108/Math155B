//
// BestN.h
//
//  Author: Sam Buss
//  Initial development: October 2006
//  All rights reserved.
// Software is "as-is" and carries no warranty.  It may be used without
//   restriction, but if you modify it, please change the filenames to
//   prevent confusion between different versions.  Please acknowledge
//   all use of the software in any publications or products based on it.
//
// Bug reports: Sam Buss, sbuss@ucsd.edu.
// Web page: http://math.ucsd.edu/~sbuss/MathCG
//
//

#ifndef BEST_N_H
#define BEST_N_H

#include "Array.h"
#include "ShellSort.h"

// This class takes data items one at a time and keeps the best N of them.
template <class T> class BestN;


// ****************************************************************
// BestN class definition
//  Usage: Initialize with the number wanted.
//         Call Reset (optional for first time)
//         Repeatedly call AddItem.
//         Call Finalize().  Returned array has the items.
//  The class T must have T::operator<() implemented.
//  The *best* items are the "least" in the sense of T::operator<().
// ****************************************************************
template <class T> class BestN 
{
public:
	BestN();						// Constructor
	BestN(long numberWanted);	// Constructor

	void Reset();
	void Reset( long numberWanted );
	void AddItem( const T& newItem );
	long Finalize();				// Returns number of items in the array

	long NumberWanted() const { return TheNumberWanted; }

	const Array<T>& GetArray() const { return TheArray; }
	Array<T>& GetArray() { return TheArray; }

private:
	long TheNumberWanted;
	Array<T> TheArray;

	long TheNumberSorted;

	void SetNumberWanted( long numberWanted );  // Use Reset(numberWanted) instead of this.
};

template <class T>
inline BestN<T>::BestN()
{
	SetNumberWanted(1);
	Reset();
}

template <class T>
inline BestN<T>::BestN( long numberWanted )
{
	SetNumberWanted( numberWanted );
	Reset();
}

template <class T>
inline void BestN<T>::SetNumberWanted( long numberWanted )
{
	assert( TheArray.IsEmpty() );
	TheNumberWanted = numberWanted;
	TheArray.Resize(2*numberWanted);
}

template <class T>
inline void BestN<T>::Reset()
{
	TheArray.MakeEmpty();
	TheNumberSorted = 0;
}

template <class T>
inline void BestN<T>::Reset( long numberWanted )
{
	TheArray.MakeEmpty();
	TheNumberSorted = 0;
	SetNumberWanted( numberWanted );
}

template <class T>
inline void BestN<T>::AddItem( const T& newItem )
{
	if ( TheNumberSorted==TheNumberWanted && TheArray[TheNumberSorted-1] < newItem ) {
		// Do not need to add this item --- it is too small.
		return;
	}
	TheArray.Push( newItem );

	if ( TheArray.IsFull() ) {
		Finalize();
	}
}

template <class T>
inline long BestN<T>::Finalize()
{
	long numElts = TheArray.SizeUsed();
	if ( numElts>1 ) {
		ShellSort( &(TheArray[0]), numElts );
	}
	long newSize = Min( TheNumberWanted, numElts );
	TheArray.ReducedSizeUsedNoCheck(newSize);
	TheNumberSorted = newSize;
	return newSize;
}



#endif    // BEST_N_H