/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *
 * Mathematics Subpackage (Splines)
 *
 * Author: Samuel R. Buss
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


#include "Slerp.h"


Quaternion Slerp( const Quaternion& q1, const Quaternion& q2, double alpha)
{
	double c = q1^q2;
	Quaternion v(q2);
	v -= c*q1;
	double s = v.Norm();
	double phi = atan2(s,c);

	if ( s >= 1.0e-9 ) {
		v = (sin((1-alpha)*phi)/s)*q1 + (sin(alpha*phi)/s)*q2;
	}
	else {
		v = (1-alpha)*q1 + alpha*q2;
	}
	return v;
}
