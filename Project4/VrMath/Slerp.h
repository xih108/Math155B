/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *
 * Mathematics Subpackage
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

//
// Spherical Operations Classes
//
// A. SphericalInterpolator
//
//
//
// Functions for spherical operations
// A. Many routines for rotation and averaging on a sphere
//

#ifndef SLERP_H
#define SLERP_H

#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "../VrMath/MathMisc.h"
#include "../VrMath/Quaternion.h"


Quaternion Slerp( const Quaternion& q1, const Quaternion& q2, double alpha);

// *****************************************************
// SphericalInterpolator class                         *
//    - Does linear interpolation (slerp-ing)		   *
// * * * * * * * * * * * * * * * * * * * * * * * * * * *


class SphericalInterpolator {

private:

public:

};

#endif // SLERP_H
