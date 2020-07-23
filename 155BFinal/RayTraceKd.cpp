/*
 *
 * RayTrace Software Package, release 4.beta, May 2018.
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


#include <windows.h>
// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <vector>

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <algorithm>

#include "RayTraceStats.h"

#include "../Graphics/PixelArray.h"
#include "../Graphics/ViewableBase.h"
#include "../Graphics/DirectLight.h"
#include "../Graphics/CameraView.h"
#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "../VrMath/MathMisc.h"
#include "../VrMath/Aabb.h"
#include "../DataStructs/KdTree.h"

#include "../RaytraceMgr/LoadNffFile.h"
#include "../RaytraceMgr/LoadObjFile.h"
#include "../RaytraceMgr/SceneDescription.h"
#include "RayTraceSetup155B.h"
#include "RayTraceSetup2.h"
#include "RayTraceKd.h"

// ***********************Statistics************
RayTraceStats MyStats;
// **********************************************


// ******************************************************
//   KdTree definitions and routines for creating the KdTree
// ******************************************************
KdTree ObjectKdTree;
const SceneDescription* kdTreeScene;

void myExtentsFunc( long objNum, AABB& retBox )
{
	return kdTreeScene->GetViewable(objNum).CalcAABB( retBox );
}
bool myExtentsInBox( long objNum, const AABB& aabb, AABB& retBox)
{
	return kdTreeScene->GetViewable(objNum).CalcExtentsInBox( aabb, retBox );
}

KdTree& myBuildKdTree(const SceneDescription& theKdTreeScene)
{
	ObjectKdTree.SetDoubleRecurseSplitting( true );
	ObjectKdTree.SetObjectCost(8.0);
    kdTreeScene = &theKdTreeScene;
	ObjectKdTree.BuildTree(kdTreeScene->NumViewables(), myExtentsFunc, myExtentsInBox  );
	RayTraceStats::PrintKdStats( ObjectKdTree );
    return ObjectKdTree;
}

vector<int> RandomPermutate(int N) {
	vector<int> p(N);
	for (int i = 0; i < N; i++) {
		p[i] = i;
	}

	for (int i = N - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		if (j < i){
			int temp = p[j];
			p[j] = p[i];
			p[i] = temp;
		}
	}
	return p;
}
// *****************************************************************
// RayTraceView() is the top level routine that starts the ray tracing.
//	Current implementation: casts a ray to the center of each pixel.
//	Calls RayTrace() for each one.
// *****************************************************************
extern const SceneDescription* theScene;
extern KdTree* theKdTree;
void RayTraceView(const SceneDescription& theRayTraceScene, KdTree& theRayTraceKdTree, PixelArray& theRayTracePixels)
{
    
	VectorR3 PixelDir;
	VectorR3 curPixelColor;		// Accumulator for Pixel Color

    theScene = &theRayTraceScene;
    theKdTree = &theRayTraceKdTree;
	const CameraView& MainView = theRayTraceScene.GetCameraView();
    int windowWidth = MainView.GetWidthPixels();
    int windowHeight = MainView.GetHeightPixels();

    // Initialize for statistics on ray tracing and kd-tree performance
	MyStats.Init();
	ObjectKdTree.ResetStats();
	// Do the rendering here
	int TraceDepth = 5;
	const int subpixels = 2;
	for ( int i=0; i<windowWidth; i++) {
		for ( int j=0; j<windowHeight; j++ ) {
			curPixelColor.SetZero();
			vector<VectorR2> sub(subpixels*subpixels);
			vector<VectorR2> eyes(subpixels*subpixels);
			//vector<VectorR3> pixLoc(subpixels*subpixels);
			for (int k = 0; k < subpixels*subpixels; k++) {
				//VectorR3 accumColor = curPixelColor;
				//float rangeX = i + (k % subpixels)*1.0 / subpixels;
				float rangeX = (k % subpixels)*1.0 / subpixels;
			
				//float rangeY = j + (k / subpixels)*1.0 / subpixels;
				float rangeY = (k / subpixels)*1.0 / subpixels;

				float newPixelX = i - 1.0/2 + rangeX + (float(rand()) / float(RAND_MAX) ) / subpixels;
				float newPixelY = j - 1.0/2 + rangeY + (float(rand()) / float(RAND_MAX) ) / subpixels;
				
				sub[k] = VectorR2(newPixelX, newPixelY);
				float rangeU = (k % subpixels)*5.0/subpixels;
				float rangeV = (k / subpixels)*5.0/subpixels;

				float newU = - 2.5 / 2 + rangeU + (float(rand()) / float(RAND_MAX)) *5.0/ subpixels;
				float newV = - 2.5 / 2 + rangeV + (float(rand()) / float(RAND_MAX)) *5.0/ subpixels;

				eyes[k] = VectorR2(rangeU,rangeV);
				//printf("%f %f", eyes[k].x,eyes[k].y);
				//MainView.CalcPixelDirection(newPixelX, newPixelY,&PixelDir);
				//RayTrace(TraceDepth, MainView.GetPosition(), PixelDir.Normalize(), accumColor);

				//curPixelColor += accumColor;
			}

			vector<int> p = RandomPermutate(subpixels*subpixels);
			for (int k = 0; k < subpixels*subpixels; k++) {
				VectorR3 accumColor = curPixelColor;
				VectorR3 eyepos = MainView.GetPosition() + eyes[k].x*MainView.GetPixeldU()+eyes[k].y*MainView.GetPixeldV();
				MainView.CalcPixelPosition(sub[p[k]].x, sub[p[k]].y,&PixelDir);
				PixelDir -= eyepos;
				RayTrace(TraceDepth, eyepos, PixelDir.Normalize(), accumColor);

				curPixelColor += accumColor;

			}
            theRayTracePixels.SetPixel(i,j,curPixelColor/(subpixels*subpixels));
		}
	}
	
   /*
	
	for ( float i=0; i<windowWidth; i += 1.0) {
		for ( float j=0; j<windowHeight; j += 1.0) {
	
			MainView.CalcPixelDirection(i, j, &PixelDir);
			RayTrace(TraceDepth, MainView.GetPosition(), PixelDir, curPixelColor);

		theRayTracePixels.SetPixel(i, j, curPixelColor);
		}
	}*/
	MyStats.GetKdRunData( ObjectKdTree );
	MyStats.PrintStats();
    theRayTracePixels.ClampAllValues();      // Clamp values to range [0,1]
}


// *********************************************************
// Data that supports the callback operation 
//		of the SeekIntersectionKd kd-Tree Traversal
//      and the ShadowFeelerKd kd-Tree Traversal
// *********************************************************
const SceneDescription* theScene;
KdTree* theKdTree;
bool kdTraverseFeeler;          // Set true if shadow feeler intersections an object.
double isectEpsilon = 1.0e-6;
long bestObject;                // Index of the object at the closest intersection so far.
long kdTraverseAvoid;           // Object from which the ray is cast (to help avoid self-intersections)
double bestHitDistance;         // Distance to the closest intersection found so far.
double kdShadowDist;
VisiblePoint tempPoint;
VisiblePoint* bestHitPoint;     // Information the closest intersection found so far.
VectorR3 kdStartPos;            // Starting position of the current ray into kdTree
VectorR3 kdStartPosAvoid;       // Starting position displaced forward slightly (to avoid self interesections)
VectorR3 kdTraverseDir;         // Direction of the ray.

// Callback function for KdTraversal of view ray or reflection ray
// It is of type PotentialObjectCallback.
// This is called by the KdTraversal function theKdTree->Traverse()
//    whenever a viewable object needs to be tested for an intersection.
// Inputs:  objectNum - the index of the viewable object.
// Returns: true if an intersection happens.
//          retStopDistance - returned as the distance to the intersection
bool potHitSeekIntersection( long objectNum, double* retStopDistance ) 
{
	double thisHitDistance;
	bool hitFlag;
	if ( objectNum == kdTraverseAvoid ) {
		hitFlag = theScene->GetViewable(objectNum).FindIntersection(kdStartPosAvoid, kdTraverseDir,
											bestHitDistance, &thisHitDistance, tempPoint);
		if ( !hitFlag ) {
			return false;
		}
		thisHitDistance += isectEpsilon;		// Adjust back to real hit distance
	}
	else {
		hitFlag = theScene->GetViewable(objectNum).FindIntersection(kdStartPos, kdTraverseDir,
											bestHitDistance, &thisHitDistance, tempPoint);
		if ( !hitFlag ) {
			return false;
		}
	}

	*bestHitPoint = tempPoint;		    // The visible point that was hit
	bestObject = objectNum;				// The object that was hit
	bestHitDistance = thisHitDistance;
	*retStopDistance = bestHitDistance;	// No need to traverse search further than this distance
	return true;
}

// Callback function for KdTraversal of shadow feeler
// It is of type PotentialObjectCallback.
// Works similarly to potHitSeekIntersection, but is much simpler.
bool potHitShadowFeeler( long objectNum, double* retStopDistance ) 
{
	double thisHitDistance;
	bool hitFlag = theScene->GetViewable(objectNum).FindIntersection(kdStartPos, kdTraverseDir,
											kdShadowDist, &thisHitDistance, tempPoint);
	if  ( hitFlag && !(/*objectNum==kdTraverseAvoid &&*/ thisHitDistance+isectEpsilon>=kdShadowDist) )
	{
		kdTraverseFeeler = false;   // Shadow feeler intersected some object.
		*retStopDistance = -1.0;	// Negative value should abort process quickly
		return true;
	}
	else { 
		return false;
	}
}

// SeekIntersectionKd seeks for an intersection with all viewable objects
// If it finds one, it returns the index of the viewable object,
//   and sets the value of hitDist and fills in the returnedPoint values.
// This "Kd" version uses the Kd-Tree
// Inputs: pos and direction - starting point and direction of the ray to trace
//         avoidK - index of object where ray starts, to avoid self-intersections.
// Outputs: *hitDist - distance of object hit, if any
//          returnedPoint - Information about the surface hit.
// Returns: Index of object hit, if any.  Or -1 if no object hit.
long SeekIntersectionKd(const VectorR3& pos, const VectorR3& direction,
										double *hitDist, VisiblePoint& returnedPoint,
										long avoidK)
{
	MyStats.AddRayTraced();

	bestObject = -1;
	bestHitDistance = DBL_MAX;
	kdTraverseAvoid = avoidK;
	kdStartPos = pos;
	kdTraverseDir = direction;
	kdStartPosAvoid = pos;
	kdStartPosAvoid.AddScaled( direction, isectEpsilon );
	bestHitPoint = &returnedPoint;
	
    theKdTree->Traverse( pos, direction, *potHitSeekIntersection );

	if ( bestObject>=0 ) {
		*hitDist = bestHitDistance;
	}
	return bestObject;
}	

// ShadowFeelerKd - returns whether the light is visible from the position pos.
//		Return value is "true" if no shadowing object found.
//		intersectNum is the index of the visible object being (possibly)
//		     illuminated at pos, or equals -1 if not applicable.

bool ShadowFeelerKd(const VectorR3& pos, const Light& light, VectorR3 displacement,long intersectNum ) {
	MyStats.AddRayTraced();
	MyStats.AddShadowFeeler();

	kdStartPos = light.GetPosition() + displacement;
	kdTraverseDir = pos;
	kdTraverseDir -= kdStartPos;
	double dist = kdTraverseDir.Norm();
	if ( dist<1.0e-7 ) {
		return true;		// Extremely close to the light!
	}
	kdTraverseDir /= dist;			// Direction from light position towards pos
	kdTraverseFeeler = true;		// True indicates no shadowing objects
	kdTraverseAvoid = intersectNum;
	kdShadowDist = dist;
    // The ray is traced from the light source towards the illuminated point.
    theKdTree->Traverse(kdStartPos, kdTraverseDir, potHitShadowFeeler, dist, true );

	return kdTraverseFeeler;	// Return whether ray is free of shadowing objects
}

// The main recursive ray tracing routine.
// Inputs: TraceDepth - depth of recursive calls to trace
//          pos, dir - starting position and direction of the ray
//          avoidK - the object the ray starts at (to avoid self-intersections).
// Outputs: returnedColor - net color from the ray tracing.
void RayTrace( int TraceDepth, const VectorR3& pos, const VectorR3 dir, 
			  VectorR3& returnedColor, long avoidK ) 
{
	double hitDist;
	VisiblePoint visPoint;

	int intersectNum = SeekIntersectionKd(pos, dir,
								&hitDist, visPoint, avoidK );
	if ( intersectNum<0 ) {
        // If no object intersected, return the background color
		returnedColor = theScene->BackgroundColor();
	}
	else {
        // Calculate local lighting (Phong lighting, or Cook-Torrance)
		CalcAllDirectIllum( pos, visPoint, returnedColor, intersectNum );
		if ( TraceDepth > 1 ) {
            // Make recursive call(s) to RayTrace
			VectorR3 nextDir;
			VectorR3 moreColor;
			const MaterialBase* thisMat = &(visPoint.GetMaterial());

			// Ray trace reflection
			if ( thisMat->IsReflective() ) {
				nextDir = visPoint.GetNormal();
				nextDir *= -2.0*(dir^visPoint.GetNormal());
				nextDir += dir;
				nextDir.ReNormalize();	// Just in case...
				VectorR3 c = thisMat->GetReflectionColor(visPoint, -dir, nextDir);
				RayTrace( TraceDepth-1, visPoint.GetPosition(), nextDir, moreColor, intersectNum);
				moreColor.x *= c.x;
				moreColor.y *= c.y;
				moreColor.z *= c.z;
				returnedColor += moreColor;
			}

			// Ray Trace Transmission
			if ( thisMat->IsTransmissive() ) {
				if ( thisMat->CalcRefractDir(visPoint.GetNormal(), dir, nextDir) ) {
					VectorR3 c = thisMat->GetTransmissionColor(visPoint, -dir, nextDir);
					RayTrace( TraceDepth-1, visPoint.GetPosition(), nextDir, moreColor, intersectNum);
					moreColor.x *= c.x;
					moreColor.y *= c.y;
					moreColor.z *= c.z;
					returnedColor += moreColor;
				}
			}
		}
	}
}

// Calculate local lighting from all light sources
// Cast shadow feelers, calculate local lighting (e.g., Phong lighting)
void CalcAllDirectIllum( const VectorR3& viewPos,
						 const VisiblePoint& visPoint, 
						 VectorR3& returnedColor, long avoidK )
{
	const MaterialBase* thisMat = &(visPoint.GetMaterial());
	const VectorR3& ambientcolor = thisMat->GetColorAmbient();
	const VectorR3& ambientlight = theScene->GlobalAmbientLight();
	const VectorR3& emitted = thisMat->GetColorEmissive();
	returnedColor.x = ambientcolor.x*ambientlight.x + emitted.x;
	returnedColor.y = ambientcolor.y*ambientlight.y + emitted.y;
	returnedColor.z = ambientcolor.z*ambientlight.z + emitted.z;

	bool transmissive = visPoint.GetMaterial().IsTransmissive();
	double viewDot;
	if ( !transmissive) {						// If not transmissive
		VectorR3 toViewPos = viewPos;
        toViewPos -= visPoint.GetPosition();		// Direction to *viewer*
		viewDot = toViewPos ^visPoint.GetNormal();
	}

	VectorR3 thisColor;
	VectorR3 percentLit;
	int numLights = theScene->NumLights();
	for ( int k=0; k<numLights; k++ ) {
		const Light& thisLight = theScene->GetLight(k);
		bool clearpath = true;
		// Cast a shadow feeler if (a) transmissive or (b) light and view on the same side
		if ( !transmissive) {
			VectorR3 toLight = thisLight.GetPosition();
			toLight -= visPoint.GetPosition();		// Direction to light
			if ( !SameSignNonzero( viewDot, (toLight^visPoint.GetNormal()) ) ) {
				clearpath = false;
			}
		}
		if (clearpath) {
			int num = 2;
			int sum = 0;
			int count = 0;
			for (int i = -num; i < num; i++) {
				for (int j = -num; j < num; j++) {
					//for (int k = 0; k < num*num; k++) {
						//float lightX = i +  % num) * 1.0 / num;
						//float lightZ = j + (k / num) * 1.0 / num;
						float lightX = i;
						float lightZ = j;
						clearpath = ShadowFeelerKd(visPoint.GetPosition(), thisLight, VectorR3(lightX, 0,lightZ), avoidK);
						if (clearpath) {
							sum += 1;
						}
						count += 1;
					//}
				}
			}
			
			percentLit.Set(float(sum)/float(count), float(sum) / float(count), float(sum) / float(count));
		
			//clearpath = ShadowFeelerKd(visPoint.GetPosition(), thisLight, VectorR3(1.0,1.0,1.0),avoidK );
		}
		/*
		if ( clearpath ) {
			percentLit.Set(1.0,1.0,1.0);	// Directly lit, with no shadowing
		}
		else {
			percentLit.SetZero();	// Blocked by shadows (still do ambient lighting)
		}
        // */
		DirectIlluminateViewPos (visPoint, viewPos, thisLight, thisColor, percentLit); 
		returnedColor += thisColor;
	}
}

