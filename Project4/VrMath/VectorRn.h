/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006
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
//    Not very sophisticated yet.  Needs more functionality
//		To do: better handling of resizing.
//

#ifndef VECTOR_RN_H
#define VECTOR_RN_H

#include <math.h>
#include <assert.h>
#include "LinearR3.h"

class VectorRn {
	friend class MatrixRmn;

public:
	VectorRn();					// Null constructor
	VectorRn( long length );	// Constructor with length
	VectorRn( const VectorRn& copy );	// Constructor setting to another
	~VectorRn();				// Destructor

	void SetLength( long newLength );
	long GetLength() const { return Length; } 

	void SetZero(); 
	void Set ( const VectorRn& src );		// Set( src ) assumes this and src are equal in length!!
	void Fill( double d );
	void Load( const double* d );
	void LoadScaled( const double* d, double scaleFactor );
	void Load( long n, const double* d ) { SetLength(n); Load(d); }
	void LoadScaled( long n, const double* d, double scaleFactor )  { SetLength(n); LoadScaled(d, scaleFactor); }
	void SetScaled ( const VectorRn& src, double factor );		// Set( src ) assumes this and src are equal in length!!
	void LoadAsSubVector( const VectorRn& src );				// Load src into initial part of "this"
	void LoadAsSubVector( int iStart, const VectorRn& src );	// Load src into "this" starting as position iStart
	void Negate();

	void Dump( double* d );

	void operator=(const VectorRn& src);	// Works even if different lengths

	// Two access methods identical in functionality
	// Subscripts are ZERO-BASED!!
	const double& operator[]( long i ) const { assert ( 0<=i && i<Length );	return *(x+i); }
	double& operator[]( long i ) { assert ( 0<=i && i<Length );	return *(x+i); }
	double Get( long i ) const { assert ( 0<=i && i<Length );	return *(x+i); }
	double GetLast() const { assert(Length>0); return *(x+(Length-1)); }

	// Use GetPtr to get pointer into the array (efficient)
	// Is friendly in that anyone can change the array contents (be careful!)
	const double* GetPtr( long i ) const { assert(0<=i && i<Length);  return (x+i); }
	double* GetPtr( long i ) { assert(0<=i && i<Length);  return (x+i); }
	const double* GetPtr() const { return x; }
	double* GetPtr() { return x; }
	const double& GetEntry( long i ) const { assert(0<=i && i<Length); return *(x+i); }
	double& GetEntry( long i ) { assert(0<=i && i<Length); return *(x+i); }

	void Set( long i, double val ) { assert(0<=i && i<Length), *(x+i) = val; }
	void SetTriple( long i, const VectorR3& u );

	VectorRn& operator+=( const VectorRn& src );
	VectorRn& operator-=( const VectorRn& src );
	void AddScaled (const VectorRn& src, double scaleFactor );

	VectorRn& operator*=( double f );
	VectorRn& operator/=( double d ) { return ( (*this) *= (1.0/d) ); }
	double NormSq() const;
	double Norm() const { return sqrt(NormSq()); }
	void Normalize() { (*this) /= Norm(); }

	double MaxAbs() const;

	// Specialized routines
	bool IsStrictlyIncreasing() const;      // Returns true if entries are strictly increasing

	double DistanceSq( const VectorRn& other ) const;
	double Distance( const VectorRn& other ) const { return sqrt(DistanceSq(other)); }
    
private:
	long Length;				// Logical or actual length 
	long AllocLength;			// Allocated length
	double *x;					// Array of vector entries


// For experts only - use cautiously:

public:
	static VectorRn WorkVector;								// Serves as a temporary vector
	static VectorRn& GetWorkVector() { return WorkVector; }
	static VectorRn& GetWorkVector( long len ) { WorkVector.SetLength(len); return WorkVector; }
	static VectorRn WorkVector2;								// Serves as a temporary vector
	static VectorRn& GetWorkVector2() { return WorkVector2; }
	static VectorRn& GetWorkVector2( long len ) { WorkVector2.SetLength(len); return WorkVector2; }
	static VectorRn WorkVector3;								// Serves as a temporary vector
	static VectorRn& GetWorkVector3() { return WorkVector3; }
	static VectorRn& GetWorkVector3( long len ) { WorkVector3.SetLength(len); return WorkVector3; }
};

// Associated non-member functions

inline VectorRn operator+( const VectorRn& u, const VectorRn& v );
inline VectorRn operator-( const VectorRn& u, const VectorRn& v );
inline VectorRn operator*( const VectorRn& u, double f );
inline VectorRn operator*( double f, const VectorRn& u );
inline VectorRn operator/( const VectorRn& u, double f );

inline bool operator==(const VectorRn&, const VectorRn&);

// Three equivalent methods for dot products
inline double Dot( const VectorRn& u, const VectorRn& v );
inline double InnerProduct(const VectorRn& u, const VectorRn& v ) { return Dot(u,v); }
inline double operator^(const VectorRn& u, const VectorRn& v) { return Dot(u,v); }

//**********************************************************************************
//  Inlined Member Functions for VectorRn's	                                       *
//**********************************************************************************

inline VectorRn::VectorRn() 
{
	Length = 0;
	AllocLength = 0;
	x = 0;
}

inline VectorRn::VectorRn( long initLength ) 
{
	Length = 0;
	AllocLength = 0;
	x = 0;
	SetLength( initLength );
}

inline VectorRn::VectorRn( const VectorRn& copy )
{
	Length = 0;
	AllocLength = 0;
	x = 0;
	SetLength( copy.Length );
	Set( copy );
}

inline VectorRn::~VectorRn() 
{
	delete x;
}

// Resize.  
inline void VectorRn::SetLength( long newLength )
{
	assert ( newLength>=0 );
	if ( newLength>AllocLength ) {
		delete x;
		AllocLength = Max( newLength, AllocLength<<1 );
		x = new double[AllocLength];
	}
	Length = newLength;
} 

// Zero out the entire vector
inline void VectorRn::SetZero()
{
	double* target = x;
	for ( long i=Length; i>0; i-- ) {
		*(target++) = 0.0;
	}
}

// Set the value of the i-th triple of entries in the vector
inline void VectorRn::SetTriple( long i, const VectorR3& u ) 
{
	long j = 3*i;
	assert ( 0<=j && j+2 < Length );
	u.Dump( x+j );
}

inline void VectorRn::Fill( double d ) 
{
	double *to = x;
	for ( long i=Length; i>0; i-- ) {
		*(to++) = d;
	}
}

inline void VectorRn::Load( const double* d )
{
	double *to = x;
	for ( long i=Length; i>0; i-- ) {
		*(to++) = *(d++);
	}
}

inline void VectorRn::LoadScaled( const double* d, double scaleFactor )
{
	double *to = x;
	for ( long i=Length; i>0; i-- ) {
		*(to++) = (*(d++))*scaleFactor;
	}
}

inline void VectorRn::Set( const VectorRn& src )
{
	assert ( src.Length == this->Length );
	double* to = x;
	double* from = src.x;
	for ( long i=Length; i>0; i-- ) {
		*(to++) = *(from++);
	}
}

inline void VectorRn::SetScaled( const VectorRn& src, double factor )
{
	assert ( src.Length == this->Length );
	double* to = x;
	double* from = src.x;
	for ( long i=Length; i>0; i-- ) {
		*(to++) = (*(from++))*factor;
	}
}

inline void VectorRn::operator=(const VectorRn& src)
{
	SetLength( src.Length );
	Set( src );
}

inline void VectorRn::Negate()
{
	double* to = x;
	for ( long i=Length; i>0; i-- ) {
		*to = -(*to);
		to++;
	}
}

inline void VectorRn::Dump( double* d )
{
	double *from = x;
	for ( long i=Length; i>0; i-- ) {
		*(d++) = *(from++);
	}
}

inline VectorRn operator+( const VectorRn& u, const VectorRn& v )
{
	VectorRn ret(u);
	ret += v;
	return ret;
}

inline VectorRn operator-( const VectorRn& u, const VectorRn& v )
{
	VectorRn ret(u);
	ret -= v;
	return ret;
}

inline VectorRn& VectorRn::operator+=( const VectorRn& src )
{
	assert ( src.Length == this->Length );
	double* to = x;
	double* from = src.x;
	for ( long i=Length; i>0; i-- ) {
		*(to++) += *(from++);
	}
	return *this;
}

inline VectorRn& VectorRn::operator-=( const VectorRn& src )
{
	assert ( src.Length == this->Length );
	double* to = x;
	double* from = src.x;
	for ( long i=Length; i>0; i-- ) {
		*(to++) -= *(from++);
	}
	return *this;
}

inline void VectorRn::AddScaled (const VectorRn& src, double scaleFactor )
{
	assert ( src.Length == this->Length );
	double* to = x;
	double* from = src.x;
	for ( long i=Length; i>0; i-- ) {
		*(to++) += (*(from++))*scaleFactor;
	}
}

inline VectorRn& VectorRn::operator*=( double f ) 
{
	double* target = x;
	for ( long i=Length; i>0; i-- ) {
		*(target++) *= f;
	}
	return *this;
}

inline VectorRn operator*( const VectorRn& u, double f ) 
{
	VectorRn ret(u.GetLength());
	ret = u;
	ret *= f;
	return ret;
}

inline VectorRn operator*( double f, const VectorRn& u )
{
	VectorRn ret(u.GetLength());
	ret = u;
	ret *= f;
	return ret;
}

inline VectorRn operator/( const VectorRn& u, double f )
{
	VectorRn ret(u.GetLength());
	ret = u;
	ret *= 1.0/f;
	return ret;
}



inline double VectorRn::NormSq() const 
{
	double* target = x;
	double res = 0.0;
	for ( long i=Length; i>0; i-- ) {
		res += (*target)*(*target);
		target++;
	}
	return res;
}

inline bool operator==(const VectorRn& u, const VectorRn& v)
{
	long n = u.GetLength();
	if ( n==v.GetLength() ) {
		const double* uPtr = u.GetPtr();
		const double* vPtr = v.GetPtr();
		while ( (n--)>0 ) {
			if ( (*(uPtr++))!=(*(vPtr++)) ) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}


//**********************************************************************************
//  Inlined Functions for VectorRn's	                                       *
//**********************************************************************************

inline double Dot( const VectorRn& u, const VectorRn& v ) 
{
	assert ( u.GetLength() == v.GetLength() );
	double res = 0.0;
	const double* p = u.GetPtr();
	const double* q = v.GetPtr();
	for ( long i = u.GetLength(); i>0; i-- ) {
		res += (*(p++))*(*(q++));
	}
	return res;
}


#endif // VECTOR_RN_H