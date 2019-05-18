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

#include "PolygonIntersect.h"
#include "Numbers.h"

// *************************************************************
// The next two methods use a linear time algorithm, and is
//    preferred for medium size polygons (size approximately 6 or higher).
// *************************************************************

// Arguments: Two planar convex polygons
// Returns true if they intersent.  False otherwise.
bool ConvexPolygonIntersectTest( const VectorR2Array& polygon1, const VectorR2Array& polygon2 )
{
	const VectorR2* curVert1 = &(polygon1.GetFirstEntry());	// Current vertex from polygon 1
	const VectorR2* curVert2 = &(polygon2.GetFirstEntry());	// Current vertex from polygon 2
	VectorR2 nextEdge1 = *(curVert1+1);
	nextEdge1 -= *curVert1;							// Next edge from polygon 1
	VectorR2 prevEdge1 = *curVert1;
	prevEdge1 -= polygon1.GetLastEntry();			// Previous edge from polygon 1
	VectorR2 nextEdge2 = *(curVert2+1);
	nextEdge2 -= *curVert2;							// Next edge from polygon 2
	VectorR2 prevEdge2 = *curVert2;
	prevEdge2 -= polygon2.GetLastEntry();			// Previous edge from polygon 2
	
	long vertCount1 = 1;				// Number of verts examined from polygon 1
	long vertCount2 = 1;				// Numver of verts examined from polygon 2
	
	VectorR2 relVert2;					// curVertex 2 relative to curVertex 1

	while ( true ) {
		relVert2 = *curVert2;
		relVert2 -= *curVert1;			// curVert2 - curVert1
		bool vert2InsidePrev1 = ( CrossR2(prevEdge1,relVert2) >= 0 );	// Is vert2 inside prev edge 1? 
		bool vert2InsideNext1 = ( CrossR2(nextEdge1,relVert2) >= 0 );	// Is vert2 inside next edge 1? 
		if ( vert2InsidePrev1 && vert2InsideNext1 ) {
			// Advance polygon 1  
			if ( CPIT_AdvanceVert( polygon1, vertCount1, curVert1, prevEdge1, nextEdge1 ) ) {
				return true;		// They intersect
			}
			continue;
		}
		bool vert1InsidePrev2 = ( CrossR2(prevEdge2,relVert2) <= 0 );	// Is vert1 inside prev edge 2? 
		bool vert1InsideNext2 = ( CrossR2(nextEdge2,relVert2) <= 0 );	// Is vert1 inside next edge 2? 
		if ( vert1InsidePrev2 && vert1InsideNext2 ) {
			// Advance polygon 2
			if ( CPIT_AdvanceVert( polygon2, vertCount2, curVert2, prevEdge2, nextEdge2 ) ) {
				return true;		// They intersect
			}
		}
		else if ( (!vert1InsidePrev2 && !vert2InsideNext1)
			&& (CrossR2(nextEdge1,prevEdge2)<=0) )
		{
			// Advance polygon 2
			if ( CPIT_AdvanceVert( polygon2, vertCount2, curVert2, prevEdge2, nextEdge2 ) ) {
				return true;		// They intersect
			}
		}
		else if ( (!vert2InsidePrev1 && !vert1InsideNext2)
			&& (CrossR2(nextEdge2,prevEdge1)<=0) )
		{
			// Advance polygon 1
			if ( CPIT_AdvanceVert( polygon1, vertCount1, curVert1, prevEdge1, nextEdge1 ) ) {
				return true;		// They intersect
			}
		}
		else {
			return false;
		}
	}
	// Will never reach this point.
}

// Similar but returns true if their *interiors* intersect
bool ConvexPolygonInteriorIntersectTest( const VectorR2Array& /*polygon1*/, const VectorR2Array& /*polygon2*/ )
{
	// TO BE WRITTEN - COPY FROM ABOVE
	assert(0);
	return true;
}

// Helper routine (internal use only)
inline bool CPIT_AdvanceVert( const VectorR2Array& polygon, long& vertCount, const VectorR2* &curVert, VectorR2& prevEdge, VectorR2& nextEdge )
{
		prevEdge = nextEdge;
		if ( (++vertCount)>=polygon.SizeUsed() ) {
			if ( vertCount>polygon.SizeUsed() ) {
				return true;			// The two polygons intersect
			}
			nextEdge = polygon.GetFirstEntry();
		}
		else {
			nextEdge = *(curVert+2);
		}
		nextEdge -= *(++curVert);
		return false;				// Need to continue processing
}


// **************************************************************
// The next two methods use a quadratic method.
// This is preferred for small polygons, say with less than 20 edges each.
// **************************************************************

// Arguments: Two planar convex polygons
// Returns true if they intersent.  False otherwise.
bool ConvexPolygonIntersectTestQuad( const VectorR2Array& polygon1, const VectorR2Array& polygon2 )
{
	return ( CPITQ_TestVertsAgainstEdges( polygon1, polygon2 )
			 && CPITQ_TestVertsAgainstEdges( polygon2, polygon1 ) );
}

// Similar but returns true if their *interiors* intersect
bool ConvexPolygonInteriorIntersectTestQuad( const VectorR2Array& polygon1, const VectorR2Array& polygon2 )
{
	return ( CPITQ_TestVertsAgainstEdgesNonstrict( polygon1, polygon2 )
			 && CPITQ_TestVertsAgainstEdgesNonstrict( polygon2, polygon1 ) );
}

// Returns false if there is an edge E of polygon2 such that all
//  verts of polygon1 are strictly "above" the line containing E.
// Otherwise returns true.
bool CPITQ_TestVertsAgainstEdges( const VectorR2Array& polygon1, const VectorR2Array& polygon2 )
{
	const VectorR2* prev = &(polygon2.GetLastEntry());
	const VectorR2* curr = &(polygon2.GetFirstEntry());
	long numEdges = polygon2.SizeUsed();
	long numVerts = polygon1.SizeUsed();
	VectorR2 edgeVector;
	for ( long i=0; i<numEdges; i++ ) {
		edgeVector = *curr;
		edgeVector -= *prev;
		double edgeConst = CrossR2(*curr,edgeVector);
		const VectorR2* vert = &(polygon1.GetFirstEntry());
		for ( long j=0; ; j++ ) {
			if ( j==numVerts ) {
				return false;				// All verts were above this edge!  We're done.
			}
			if ( CrossR2(*vert,edgeVector)<=edgeConst ) {
				break;
			}
			vert++;
		}
		prev = curr;
		curr++;
	}
	return true;
}

// Returns false if there is an edge E of polygon2 such that all
//  verts of polygon1 are "on or above"  the line containing E.
// Otherwise returns true.
bool CPITQ_TestVertsAgainstEdgesNonstrict( const VectorR2Array& /*polygon1*/, const VectorR2Array& /*polygon2*/ )
{
	// TO BE WRITTEN (COPY FROM ABOVE, CHANGE <= to <.
	assert(0);
	return true;
}

// **************************************************************
// The next two methods use a quadratic method with golden ratio jumping
// This is experimental.
// **************************************************************

// Arguments: Two planar convex polygons
// Returns true if they intersent.  False otherwise.
bool ConvexPolygonIntersectTestGolden( const VectorR2Array& polygon1, const VectorR2Array& polygon2 )
{
	long goldenStep1 = GoldenStepSize( polygon1.SizeUsed() );
	long goldenStep2 = GoldenStepSize( polygon2.SizeUsed() );
	return ( CPITG_TestVertsAgainstEdges( polygon1, polygon2, goldenStep1, goldenStep2 )
			 && CPITG_TestVertsAgainstEdges( polygon2, polygon1, goldenStep2, goldenStep1 ) );
}

// Similar but returns true if their *interiors* intersect
bool ConvexPolygonInteriorIntersectTestGolden( const VectorR2Array& polygon1, const VectorR2Array& polygon2 )
{
	long goldenStep1 = GoldenStepSize( polygon1.SizeUsed() );
	long goldenStep2 = GoldenStepSize( polygon2.SizeUsed() );
	return ( CPITG_TestVertsAgainstEdgesNonstrict( polygon1, polygon2, goldenStep1, goldenStep2 )
			 && CPITG_TestVertsAgainstEdgesNonstrict( polygon2, polygon1, goldenStep2, goldenStep1 ) );
}

// Returns false if there is an edge E of polygon2 such that all
//  verts of polygon1 are strictly "above" the line containing E.
// Otherwise returns true.
bool CPITG_TestVertsAgainstEdges( const VectorR2Array& polygon1, const VectorR2Array& polygon2, long goldenStep1, long goldenStep2 )
{
	long numEdges = polygon2.SizeUsed();
	long numVerts = polygon1.SizeUsed();
	long edgeNum = 0;
	VectorR2 curEdge;
	for ( long i=0; i<numEdges; i++ ) {
		const VectorR2 *curr = &(polygon2.GetEntry(edgeNum));
		curEdge = *curr;
		curEdge -= polygon2.GetEntry((edgeNum+(numEdges-1))%numEdges);
		double edgeConst = CrossR2(*curr,curEdge);
		long vertNum=0;
		for ( long j=0; ; j++ ) {
			if ( j==numVerts ) {
				return false;				// All verts were above this edge!  We're done.
			}
			if ( CrossR2(polygon1.GetEntry(vertNum),curEdge)<=edgeConst ) {
				break;
			}
			vertNum = (vertNum+goldenStep1)%numVerts;
		}
		edgeNum = (edgeNum+goldenStep2)%numEdges;
	}
	return true;
}

// Returns false if there is an edge E of polygon2 such that all
//  verts of polygon1 are "on or above"  the line containing E.
// Otherwise returns true.
bool CPITG_TestVertsAgainstEdgesNonstrict( const VectorR2Array& /*polygon1*/, const VectorR2Array& /*polygon2*/, long /*goldenStep1*/, long /*goldenStep2*/ )
{
	// TO BE WRITTEN (COPY FROM ABOVE, CHANGE <= to <.
	assert(0);
	return true;
}



