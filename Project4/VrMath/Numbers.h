
// Numbers.h
// Simple number theoretic functions and related items

#ifndef NUMBERS_H
#define NUMBERS_H

// Greatest common divisor of two positive integers
long GreatestCommonDivisor( long m, long n );

// Determine whether two positive integers are relatively prime
bool RelativelyPrime( long m, long n );

// Find closest integer to n/(golden ratio) relatively prime to n
// Useful for multiplicative hashing and other applications.
// The integer n must be greater than two!
long GoldenStepSize( long n );




// ************************************
// Inlined functions
// ************************************

inline bool RelativelyPrime( long m, long n )
{
	return ( 1==GreatestCommonDivisor(m, n) );
}




#endif // NUMBERS_H