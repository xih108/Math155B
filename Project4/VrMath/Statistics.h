
// Statistical functions and related items

#ifndef STATISTICS_H
#define STATISTICS_H

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "MathMisc.h"

// **************************************
// Classes defined in this header file
// **************************************
class MeanAndVarianceComputer;
class WeightedMeanAndVarianceComputer;
// Random number generators
class UniformGenerator;
class GaussianGenerator;
class HomogenousR2UniformGenerator;


// Factorial (long integer valued)
long Factorial( int i );	// Not inlined!
long long Factorial_int64( int i );	// Not inlined!

// The standard normal distribution (or Gaussian).
inline double NormalDistribution( double x );
inline double NormalDistribution( double x, double mean, double stddev );
// Cumulative normal distribution function.  (\int_{-\infty}^x).
double CumulativeNormal( double x );
inline double CumulativeNormal( double x, double mean, double stddev );

// The gamma and log gamma functions

double Gamma( double x );
double LogGamma( double x );

// ***************************************************************
// A handy little class for computing means and variances.
//		Resets the "center" (approximate mean) for the variance
//		calculation on an occasional basis.
// ***************************************************************
class MeanAndVarianceComputer
{
public:
	MeanAndVarianceComputer();

	// Usage: Repeatedly add values.  When done, call Finalize();
	//    It is OK to add more values after a call to Finalize();
	long AddValue( double value );
	long Finalize();

	// After calling Finalize(), can get statistics.
	long NumberValues() const { return TheNumberOfValues; }
	double Mean() const;
	double Variance() const;
	double StandardDeviation() const { return sqrt( Variance() ); }


private:
	long TheNumberOfValues;
	double TheMean;
	double TheVariance;

	double TheSum;						// Sum of the numbers so far

	double VarianceCenter;
	double SumSquaresCentered;		// Sum of differences from the "VarianceCenter" value.

	long ResetVarianceCenterCount;		// When number of values reaches this, reset the variance's center
	bool IsFinalized;
};

// ***************************************************************
// A handy little class for computing means, variances and covariances
//		of two random variables.
//		Resets the "center" (approximate mean) for the variance
//		calculation on an occasional basis.
// ***************************************************************
class MeansAndCovarianceComputer
{
public:
	MeansAndCovarianceComputer();

	// Usage: Repeatedly add values.  When done, call Finalize();
	//    It is OK to add more values after a call to Finalize();
	long AddValues( double valueX, double valueY );
	long Finalize();

	// After calling Finalize(), can get statistics.
	long NumberValues() const { return TheNumberOfValues; }
	double MeanX() const;
	double MeanY() const;
	double VarianceX() const;
	double VarianceY() const;
	double Covariance() const;
	double StandardDeviationX() const { return sqrt( VarianceX() ); }
	double StandardDeviationY() const { return sqrt( VarianceY() ); }

	double MeanOfDifference() const { return TheMeanX + TheMeanY; }
	double VarianceOfDifference() const { return TheVarianceX + TheVarianceY - 2.0*TheCovariance; }

	// Return the constants a, b so that a*X + b best approximates Y.
	double OptimalFactorToApproxY() const;		// The optimal value for a.
	double OptimalConstantToApproxY() const;	// The optimal value for b.
	double OptimalApproxYVariance() const;		// The variance in the resulting approximation.

private:
	long TheNumberOfValues;
	double TheMeanX;
	double TheMeanY;
	double TheVarianceX;
	double TheVarianceY;
	double TheCovariance;

	double TheSumOfX;						// Sum of the X numbers so far
	double TheSumOfY;						// Sum of the Y numbers so far

	double VarianceCenterX;
	double VarianceCenterY;
	double SumSquaresCenteredX;		// Sum of differences from the "VarianceCenterX" value.
	double SumSquaresCenteredY;		// Sum of differences from the "VarianceCenterY" value.
	double SumProductsXY;

	long ResetVarianceCenterCount;		// When number of values reaches this, reset the variance's center
	bool IsFinalized;
};

// ***************************************************************
// A handy little class for computing weighted means and variances.
//		Resets the "center" (approximate mean) for the variance
//		calculation on an occasional basis.
// ***************************************************************
class WeightedMeanAndVarianceComputer
{
public:
	WeightedMeanAndVarianceComputer();

	// Usage: Repeatedly add values.  When done, call Finalize();
	//    It is OK to add more values after a call to Finalize();
	long AddValue( double value, double weight = 1.0 );
	long Finalize();

	// After calling Finalize(), can get statistics.
	long NumberValues() const { return TheNumberOfValues; }
	double TotalWeight() const { return TheTotalWeight; }
	double Mean() const;
	double Variance() const;
	double StandardDeviation() const { return sqrt( Variance() ); }


private:
	double TheMean;
	double TheVariance;

	double TheSum;						// Weighted sum of the numbers so far

	double VarianceCenter;
	double SumSquaresCentered;		// Weighted sum of differences from the "VarianceCenter" value.

	double TheTotalWeight;
	long TheNumberOfValues;

	long ResetVarianceCenterCount;		// When number of values reaches this, reset the variance's center
	bool IsFinalized;
};




// ***************************************************************
// Generate uniformly distributed values
//	in the specified range.
// ***************************************************************

class UniformGenerator
{
public:

	UniformGenerator();
	UniformGenerator( double min, double max );

	void SetMinMax( double min, double max );

	double Rand();

private:
	double MinValue;
	double DeltaValue;		// equals MaxValue-MinValue

};

// ***************************************************************
// Generate normally distributed (Gaussian) values 
//  with specified mean and	variance
// ***************************************************************

class GaussianGenerator
{
	// Defaults to mean zero and variance one
	GaussianGenerator( double mean = 0.0, double stddev = 1.0 );

	void SetMeanAndVariance( double mean, double stddev );

	double Mean() const {return MeanValue;}
	double StdDev() const {return sqrt(StdDevValue);}
	double Variance() const { return Square(StdDevValue); }

	inline double Rand();

private:
	double MeanValue;
	double StdDevValue;

	bool NextComputed;
	double NextValue;

	double ComputeNextTwoValues();
};

// Generate random homogenous coordinates for triangles.
// Returns alpha, beta and gamma values in [0,1] that sum to 1.
class HomogeneousR2UniformGenerator
{
public:
	HomogeneousR2UniformGenerator() {}

	void Rand( double &alpha, double &beta, double &gamma );
};

// ************************************************
// Inlined functions
// ************************************************

inline double NormalDistribution( double x )
{
	return exp( -0.5*x*x ) * TwoPiSqrtInv;
}

inline double NormalDistribution( double x, double mean, double stddev )
{
	assert( stddev>0.0 );
	double stddevInv = 1.0/stddev;
	return( stddevInv * NormalDistribution((x-mean)*stddevInv) );
}

inline double CumulativeNormal( double x, double mean, double stddev )
{
	assert ( stddev>0.0 );
	return CumulativeNormal( (x-mean)/stddev );
}


// ********************************************************
// Inlined functions for MeanAndVarianceComputer
// ********************************************************

inline MeanAndVarianceComputer::MeanAndVarianceComputer()
{
	IsFinalized = false;
	TheNumberOfValues = 0;
	TheSum = 0.0;
	SumSquaresCentered = 0.0;
	ResetVarianceCenterCount = 0;
}

inline double MeanAndVarianceComputer::Mean() const
{
	assert(IsFinalized); 
	return TheMean; 
}

inline double MeanAndVarianceComputer::Variance() const
{
	assert(IsFinalized); 
	return TheVariance; 
}

inline long MeanAndVarianceComputer::AddValue( double value )
{
	IsFinalized = false;
	if ( TheNumberOfValues == ResetVarianceCenterCount ) {
		if ( TheNumberOfValues==0 ) {
			TheSum = value;
			VarianceCenter = value;
			SumSquaresCentered = 0.0;
			ResetVarianceCenterCount = 1;
		}
		else {
			TheSum += value;
			SumSquaresCentered += Square(value-VarianceCenter);
			double newNumValues = (double)(TheNumberOfValues+1);
			double newCenter = TheSum/newNumValues;
			SumSquaresCentered -= newNumValues*Square(VarianceCenter-newCenter);
			VarianceCenter = newCenter;
			ResetVarianceCenterCount *= 2;
		}
	}
	else {
		TheSum += value;
		SumSquaresCentered += Square(value-VarianceCenter);
	}
	TheNumberOfValues++;
	return TheNumberOfValues;
}

// Call this before extracting statistics
inline long MeanAndVarianceComputer::Finalize()
{
	assert( TheNumberOfValues>0 );
	double numValues = (double)TheNumberOfValues;
	double numValuesInv = 1.0/numValues;

	TheMean = TheSum*numValuesInv;

	SumSquaresCentered -= numValues*Square(VarianceCenter-TheMean);
	VarianceCenter = TheMean;
	ResetVarianceCenterCount = 2*TheNumberOfValues;

	TheVariance = Max(0.0,numValuesInv*SumSquaresCentered);		// Avoids roundoff yielding negative values
	
	IsFinalized = true;
	return TheNumberOfValues;
}

// ********************************************************
// Inline functions for MeansAndCovarianceComputer
// ********************************************************

inline MeansAndCovarianceComputer::MeansAndCovarianceComputer()
{
	IsFinalized = false;
	TheNumberOfValues = 0;
	TheSumOfX = 0.0;
	TheSumOfY = 0.0;
	SumSquaresCenteredX = 0.0;
	SumSquaresCenteredY = 0.0;
	VarianceCenterX = 0.0;
	VarianceCenterY = 0.0;
	SumProductsXY = 0.0;
	ResetVarianceCenterCount = 0;

}

inline double MeansAndCovarianceComputer::MeanX() const
{
	assert(IsFinalized); 
	return TheMeanX; 
}

inline double MeansAndCovarianceComputer::MeanY() const
{
	assert(IsFinalized); 
	return TheMeanY; 
}

inline double MeansAndCovarianceComputer::VarianceX() const
{
	assert(IsFinalized); 
	return TheVarianceX; 
}

inline double MeansAndCovarianceComputer::VarianceY() const
{
	assert(IsFinalized); 
	return TheVarianceY; 
}

inline double MeansAndCovarianceComputer::Covariance() const
{
	assert(IsFinalized); 
	return TheCovariance; 
}

// ********************************************************
// Inlined functions for WeightedMeanAndVarianceComputer
// ********************************************************

inline WeightedMeanAndVarianceComputer::WeightedMeanAndVarianceComputer()
{
	IsFinalized = false;
	TheNumberOfValues = 0;
	TheTotalWeight = 0.0;
	TheSum = 0.0;
	SumSquaresCentered = 0.0;
	ResetVarianceCenterCount = 0;
}

inline double WeightedMeanAndVarianceComputer::Mean() const
{
	assert(IsFinalized); 
	return TheMean; 
}

inline double WeightedMeanAndVarianceComputer::Variance() const
{
	assert(IsFinalized); 
	return TheVariance; 
}

inline long WeightedMeanAndVarianceComputer::AddValue( double value, double weight )
{
	assert( weight>0.0 );
	IsFinalized = false;
	if ( TheNumberOfValues == ResetVarianceCenterCount ) {
		if ( TheNumberOfValues==0 ) {
			TheTotalWeight = weight;
			TheSum = value*weight;
			VarianceCenter = value;
			SumSquaresCentered = 0.0;
			ResetVarianceCenterCount = 1;
		}
		else {
			TheTotalWeight += weight;
			TheSum += value*weight;
			SumSquaresCentered += weight*Square(value-VarianceCenter);
			double newCenter = TheSum/TheTotalWeight;
			SumSquaresCentered -= TheTotalWeight*Square(VarianceCenter-newCenter);
			VarianceCenter = newCenter;
			ResetVarianceCenterCount *= 2;
		}
	}
	else {
		TheTotalWeight += weight;
		TheSum += value*weight;
		SumSquaresCentered += weight*Square(value-VarianceCenter);
	}
	TheNumberOfValues++;
	return TheNumberOfValues;
}

// Call this before extracting statistics
inline long WeightedMeanAndVarianceComputer::Finalize()
{
	assert( TheTotalWeight>0.0 );
	TheMean = TheSum/TheTotalWeight;

	SumSquaresCentered -= TheTotalWeight*Square(VarianceCenter-TheMean);
	VarianceCenter = TheMean;
	ResetVarianceCenterCount = 2*TheNumberOfValues;

	TheVariance = Max(0.0,SumSquaresCentered/TheTotalWeight);		// Avoids roundoff yielding negative values
	
	IsFinalized = true;
	return TheNumberOfValues;
}


// ************************************************
// Inlined members for UniformGenerator
// ************************************************

inline UniformGenerator::UniformGenerator()
{
	SetMinMax(0.0, 1.0);
}

inline UniformGenerator::UniformGenerator( double min, double max)
{
	SetMinMax(min, max);
}

inline void UniformGenerator::SetMinMax( double min, double max)
{
	MinValue = min;
	DeltaValue = max-min;
	assert( DeltaValue>0.0);
}

inline double UniformGenerator::Rand()
{
	double r = ((double)rand())/((double)RAND_MAX);		// Crappy random number generator but OK for now.
	return DeltaValue*r + MinValue;
}

// ************************************************
// Inlined members for GaussianGenerator
// ************************************************

inline GaussianGenerator::GaussianGenerator( double mean, double stddev )
{
	SetMeanAndVariance ( mean, stddev );
}

inline void GaussianGenerator::SetMeanAndVariance( double mean, double stddev )
{
	assert( StdDevValue>0.0 );

	MeanValue = mean;
	StdDevValue = stddev;
	NextComputed = false;
}

inline double GaussianGenerator::Rand()
{ 
	if ( NextComputed ) {
		NextComputed = false;
		return NextValue;
	}
	else {
		return ComputeNextTwoValues();
	}
}

#endif // STATISTICS_H

