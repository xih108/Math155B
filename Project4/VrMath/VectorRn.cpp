/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *
 * Mathematics Subpackage (VrMath)
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
// VectorRn:  Vector over Rn  (Variable length vector)
//

#include "VectorRn.h"

// A small number of work vectors (to avoid constructors!)
VectorRn VectorRn::WorkVector;
VectorRn VectorRn::WorkVector2;
VectorRn VectorRn::WorkVector3;

// Load src into initial part of "this"
void VectorRn::LoadAsSubVector( const VectorRn& src )		
{
	assert ( src.Length <= Length );
	const double *fromPtr = src.x;
	double* toPtr = x;
	for ( long i = src.Length; i>0; i-- ) {
		*(toPtr++) = *(fromPtr++);
	}
}

// Load src into "this" starting as position i
void VectorRn::LoadAsSubVector( int iStart, const VectorRn& src )
{
	assert ( src.Length+ iStart <= Length );
	const double *fromPtr = src.x;
	double* toPtr = x + iStart;
	for ( long i = src.Length; i>0; i-- ) {
		*(toPtr++) = *(fromPtr++);
	}
}


double VectorRn::MaxAbs () const
{
	double result = 0.0;
	double* t = x;
	for ( long i = Length; i>0; i-- ) {
		if ( (*t) > result ) {
			result = *t;
		}
		else if ( -(*t) > result ) {
			result = -(*t);
		}
		t++;
	}
	return result;
}


// Returns true if entries are strictly increasing
bool VectorRn::IsStrictlyIncreasing() const
{
	const double* xPtr = x;
	for ( long i=Length-1; i>0; i-- ) {
		if ( !( (*xPtr) < (*(xPtr+1) ) ) ) {
			return false;
		}
		xPtr++;
	}
	return true;
}

// Euclidean distance, squared
double VectorRn::DistanceSq( const VectorRn& other ) const
{
	assert ( Length == other.Length );

	double ret = 0.0;
	const double* thisPtr = x;
	const double* otherPtr = other.x;
	for ( int i = Length; i>0; i-- ) {
		ret += Square( (*(thisPtr++)) - (*(otherPtr++)) );
	}
	return ret;
}
	
