/*
 *
 * RayTrace Software Package, release 3.X
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

// PolygonIntersect.h
//  Author: Sam Buss, July 2006.

#ifndef POLYGON_INTERSECT_H
#define POLYGON_INTERSECT_H

#include "LinearR2.h"
#include "../DataStructs/Array.h"

typedef Array<VectorR2> VectorR2Array;


// *************************************************************
// The next two methods use a linear time algorithm, and is
//    preferred for medium size polygons (size approximately 6 or higher).
// *************************************************************

// Arguments: Two convex polygons
// Returns true if they intersent.  False otherwise.
bool ConvexPolygonIntersectTest( const VectorR2Array& polygon1, const VectorR2Array& polygon2 );

// Similar but returns true if their *interiors* intersect
bool ConvexPolygonInteriorIntersectTest( const VectorR2Array& polygon1, const VectorR2Array& polygon2 );

// **************************************************************
// The next two methods use a quadratic method.
// This is preferred for small polygons, say with less than 20 edges each.
// **************************************************************

// Arguments: Two convex polygons
// Returns true if they intersent.  False otherwise.
bool ConvexPolygonIntersectTestQuad( const VectorR2Array& polygon1, const VectorR2Array& polygon2 );

// Similar but returns true if their *interiors* intersect
bool ConvexPolygonInteriorIntersectTestQuad( const VectorR2Array& polygon1, const VectorR2Array& polygon2 );

/// **************************************************************
// The next two methods use a quadratic method with golden ratio jumping
// This is experimental
// **************************************************************

// Arguments: Two convex polygons
// Returns true if they intersent.  False otherwise.
bool ConvexPolygonIntersectTestGolden( const VectorR2Array& polygon1, const VectorR2Array& polygon2 );

// Similar but returns true if their *interiors* intersect
bool ConvexPolygonInteriorIntersectTestGolden( const VectorR2Array& polygon1, const VectorR2Array& polygon2 );

// *****************************************************************
// Helper routines, intended for internal use only.
// *****************************************************************

bool CPIT_AdvanceVert( const VectorR2Array& polygon, long& vertCount, const VectorR2*& curVert, VectorR2& prevEdge, VectorR2& nextEdge );


bool CPITQ_TestVertsAgainstEdges( const VectorR2Array& polygon1, const VectorR2Array& polygon2 );
bool CPITQ_TestVertsAgainstEdgesNonstrict( const VectorR2Array& polygon1, const VectorR2Array& polygon2 );
bool CPITG_TestVertsAgainstEdges( const VectorR2Array& polygon1, const VectorR2Array& polygon2, long goldenStep1, long goldenStep2 );
bool CPITG_TestVertsAgainstEdgesNonstrict( const VectorR2Array& polygon1, const VectorR2Array& polygon2, long goldenStep1, long goldenStep2 );

#endif // POLYGON_INTERSECT_H


