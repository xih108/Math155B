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

#include <assert.h>

#include "stdlib.h"
#include "MathMisc.h"
#include "Statistics.h"

UniformGenerator UniformGeneratorDefault(0.0,1.0);

// Integer valued factorial functions

long Factorial( int i )
{
	assert ( i>=0 );

	long ret = 1;
	for ( int j=1; j<=i; j++ ) {
		assert( (ret*j)/j == ret );	 // Watch out for integer overflow
		ret *= j;
	}
	return ret;
}

long long Factorial_int64( int i )
{
	assert ( i>=0 );

	long long ret = 1;
	for ( int j=1; j<=i; j++ ) {
		assert( (ret*j)/j == ret );	 // Watch out for integer overflow
		ret *= j;
	}
	return ret;
}

double CumulativeNormalValuesTable[67] = {
	7.9197255526572135e-017,
	6.2209604980732577e-016,
	4.594627428158745e-015,
	3.1908916707957432e-014,
	2.0838581585958971e-013,
	1.279812543878215e-012,
	7.3922577780101928e-012,
	4.0160005838906431e-011,
	2.0522634252188401e-010,
	9.8658764503768796e-010,
	4.4621724539016001e-009,
	1.8989562465883565e-008,
	7.6049605164887185e-008,
	2.8665157187919381e-007,
	1.0170832425687021e-006,
	3.3976731247301339e-006,
	1.0688525774934475e-005,
	3.1671241833119979e-005,
	8.8417285200803923e-005,
	0.00023262907903552534,
	0.00057702504239076733,
	0.0013498980316300948,
	0.0029797632350545577,
	0.0062096653257759718,
	0.012224472655044536,
	0.022750131948179042,
	0.040059156863816926,
	0.066807201268853422,
	0.10564977366685062,
	0.15865525393145241,
	0.22662735237686354,
	0.30853753872598688,
	0.40129367431707624,
	0.5,
	0.5987063256829237,
	0.69146246127401767,
	0.77337264762313651,
	0.84134474606854759,
	0.89435022633314942,
	0.93319279873114203,
	0.959940843136183,
	0.9772498680518209,
	0.98777552734495544,
	0.9937903346742295,
	0.99702023676495111,
	0.99865010196837556,
	0.9994229749576149,
	0.99976737092096313,
	0.99991158271479785,
	0.99996832875816555,
	0.99998931147422376,
	0.99999660232687493,
	0.99999898291675704,
	0.99999971334842774,
	0.9999999239503945,
	0.99999998101043797,
	0.99999999553782792,
	0.99999999901341274,
	0.99999999979477405,
	0.99999999995983524,
	0.99999999999260303,
	0.99999999999871547,
	0.99999999999978695,
	0.99999999999996825,
	0.99999999999999556,
	0.99999999999999956,
	1.0
};

double CumulativeNormal( double x )
{
	if ( x<-8.4 ) {
		return 0.0;
	}
	if ( x>8.2 ) {
		return 1.0;
	}
		
	// Use a Taylor series exansion around nearest quarter integer point
	int startIndex = (int)(4.0*x + 33.5);		// ((x-(-8.25))*4.0 + 0.5)
	double z0 = 0.25*(double)startIndex - 8.25;		// Center of Taylor expansion
	double z = (x-z0);
	double zTimesz0 = z*z0;
	double zSq = Square(z);
	double olderTaylorTerm = z*exp(-0.5*Square(z0));
	double oldTaylorTerm = -zTimesz0*olderTaylorTerm;
	double ret = olderTaylorTerm;
	ret += 0.5*oldTaylorTerm;
	double deg = 2.0;
	for ( int i=0; i<10; i++ ) {
		double thisTaylorTerm = -(zSq*olderTaylorTerm + zTimesz0*oldTaylorTerm)/deg;
		deg += 1.0;
		ret += thisTaylorTerm/deg;
		olderTaylorTerm = oldTaylorTerm;
		oldTaylorTerm = thisTaylorTerm;
	}
	ret *= TwoPiSqrtInv;
	ret += CumulativeNormalValuesTable[startIndex];
	return ClampRange( ret, 0.0, 1.0 );
}


const double SpougeC[12] = {		// Holds values SpougeC[i] = c_{i+1}, a= 12.5
	133550.50294247747,
	-492930.93529936048,
	741287.47369761183,
	-585097.37760399678,
	260425.27033038536,
	-65413.353396114217,
	8801.4596350842148,
	-564.80502412898045,
	13.803798339181425,
	-0.080781761698950807,
	3.4797414457424617e-005,
	-5.6892712275042444e-012   };

double HalfLogTwoPi = 0.5*log(PI2);

// Use the Spouge algorithm to compute the log-gamma function
double LogGamma( double x ) 
{
	double sum = 1.0;
	const double* cPtr = SpougeC;
	int i;
	for ( i=0; i<12; i++ ) {
		sum += (*(cPtr++))/(x+(double)i);
	}

	return ( (x-0.5)*log(x+11.5) - (x+11.5) + HalfLogTwoPi + log(sum) );
}

double Gamma( double x )
{
	return exp(LogGamma(x));
}

// **********************************************************
// MeansAndCovarianceComputer
// **********************************************************

long MeansAndCovarianceComputer::AddValues( double valueX, double valueY )
{
	IsFinalized = false;
	TheSumOfX += valueX;
	TheSumOfY += valueY;
	double deltaX = valueX-VarianceCenterX;
	SumSquaresCenteredX += Square(deltaX);
	double deltaY = valueY-VarianceCenterY;
	SumSquaresCenteredY += Square(deltaY);
	SumProductsXY += deltaX*deltaY;

	if ( TheNumberOfValues == ResetVarianceCenterCount ) {
		if ( TheNumberOfValues==0 ) {
			VarianceCenterX = valueX;
			SumSquaresCenteredX = 0.0;
			VarianceCenterY = valueY;
			SumSquaresCenteredY = 0.0;
			SumProductsXY = 0.0;
			ResetVarianceCenterCount = 1;
		}
		else {
			double newNumValues = (double)(TheNumberOfValues+1);
			double newCenterX = TheSumOfX/newNumValues;
			double newCenterY = TheSumOfY/newNumValues;
			double deltaCenterX = VarianceCenterX-newCenterX;
			double deltaCenterY = VarianceCenterY-newCenterY;
			SumSquaresCenteredX -= newNumValues*Square(deltaCenterX);
			SumSquaresCenteredY -= newNumValues*Square(deltaCenterY);
			SumProductsXY -= newNumValues*deltaCenterX*deltaCenterY;
			VarianceCenterX = newCenterX;
			VarianceCenterY = newCenterY;
			ResetVarianceCenterCount *= 2;
		}
	}

	TheNumberOfValues++;
	return TheNumberOfValues;
}

// Call this before extracting statistics
long MeansAndCovarianceComputer::Finalize()
{
	double numValues = (double)TheNumberOfValues;
	double numValuesInv = 1.0/numValues;

	TheMeanX = TheSumOfX*numValuesInv;
	TheMeanY = TheSumOfY*numValuesInv;

	double deltaCenterX = VarianceCenterX-TheMeanX;
	double deltaCenterY = VarianceCenterY-TheMeanY;
	SumSquaresCenteredX -= numValues*Square(deltaCenterX);
	SumSquaresCenteredY -= numValues*Square(deltaCenterY);
	SumProductsXY -= numValues*deltaCenterX*deltaCenterY;
	VarianceCenterX = TheMeanX;
	VarianceCenterY = TheMeanY;
	ResetVarianceCenterCount = 2*TheNumberOfValues;

	TheVarianceX = Max(0.0,numValuesInv*SumSquaresCenteredX);		// Avoids roundoff yielding negative values
	TheVarianceY = Max(0.0,numValuesInv*SumSquaresCenteredY);
	TheCovariance = numValuesInv*SumProductsXY;
	
	IsFinalized = true;
	return TheNumberOfValues;
}

// The next three routines return information about the best linear approximation
//   to Y, based on X.
//   The first two return the constants a, b so that a*X + b best approximates Y.
// The optimal value for a.
double MeansAndCovarianceComputer::OptimalFactorToApproxY() const
{
	assert( IsFinalized );
	if ( TheVarianceX<=0.0 ) {
		return 0;
	}
	return TheCovariance/TheVarianceX;
}
// The optimal value for b.
double MeansAndCovarianceComputer::OptimalConstantToApproxY() const
{
	assert( IsFinalized );
	return TheMeanY - OptimalFactorToApproxY()*TheMeanX;
}
// The variance in the resulting approximation.
double MeansAndCovarianceComputer::OptimalApproxYVariance() const
{
	assert( IsFinalized );
	if ( TheVarianceX<=0.0 ) {
		return TheVarianceY;
	}
	double ret = TheVarianceY - Square(TheCovariance)/TheVarianceX;
	ret = Max(0.0, ret);				// Avoid roundoff errors giving negative value
	return ret;
}


// **********************************************************
// GausssianGenerator
// **********************************************************
double GaussianGenerator::ComputeNextTwoValues()
{
	double x, y;
	double R;		// Radius squared
	do {
		x = (2.0/(double)RAND_MAX)*((double)rand()) - 1.0;  // Crappy random number generator, but OK for now
		y = (2.0/(double)RAND_MAX)*((double)rand()) - 1.0;
		R = x*x+y*y;
	} while ( R>1.0 || R==0.0 );

	double s = sqrt( (-2.0*log(R))/R ) * StdDevValue;

	NextValue = x*s + MeanValue;
    return ( y*s + MeanValue );
}

// **********************************************************
// HomogenousR2UniformGenerator
// **********************************************************
void HomogeneousR2UniformGenerator::Rand( double &alpha, double &beta, double &gamma)
{
	alpha = UniformGeneratorDefault.Rand();
	beta = UniformGeneratorDefault.Rand();
	gamma = 1.0-(alpha+beta);
	if ( gamma<0.0 ) {
		alpha = 1.0-alpha;
		beta = 1.0-beta;
		gamma = 1.0-(alpha+beta);
	}
}


