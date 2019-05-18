
// Numbers.cpp
// Simple number theoretic functions and related items

#include <assert.h>
#include "Numbers.h"
#include "MathMisc.h"

// Greatest common divisor of two positive integers.
// Uses Euclid's algorithm.
long GreatestCommonDivisor( long m, long n )
{
	assert ( m>0 && n>0 );

	// Invariant n >= m (except possibly first iteration) 
	//     and GCD is preserved.
	while ( m!=0 ) {
		long rem = n%m;
		n = m;
		m = rem;
	}

	return n;
}

// Find closest integer to n/(golden ratio) relatively prime to n.
// Useful for multiplicative hashing and other applications.
// The integer n must be greater than two!
long GoldenStepSize( long n )
{
	assert ( n>2 );

	double phiInvTimesN = GoldenRatioInv*n;
	long trialInt = (long)(phiInvTimesN+0.5);	// Round to nearest integer
	long stepDir = (GoldenRatio*(double)trialInt<n) ? 1 : -1;
	long stepLen = 1;
	while ( !RelativelyPrime( trialInt, n ) ) {
		trialInt += stepDir*stepLen;
		stepDir = -stepDir;
		stepLen += 1;
	}
	return trialInt;

}
