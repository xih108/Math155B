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

#ifndef RAY_TRACE_KD
#define RAY_TRACE_KD 

class VectorR3;
class Light;
class VisiblePoint;
class KdTree;
class SceneDescription;
class PixelArray;

// Call this to build a KdTree.
KdTree& myBuildKdTree(const SceneDescription& theKdTreeScene);

// Main ray tracing routine
void RayTraceView(const SceneDescription& theRayTraceScene, KdTree& theRayTraceKdTree, PixelArray& theRayTracePixels);

// Internal routines for ray tracing
long SeekIntersectionKd(const VectorR3& startPos, const VectorR3& direction,
    double *hitDist, VisiblePoint& returnedPoint,
    long avoidK = -1);
void RayTrace(int TraceDepth, const VectorR3& pos, const VectorR3 dir,
    VectorR3& returnedColor, long avoidK = -1);
bool ShadowFeelerKd(const VectorR3& pos, const Light& light, VectorR3 displacement, long intersectNum = -1);
void CalcAllDirectIllum(const VectorR3& viewPos, const VisiblePoint& visPoint,
    VectorR3& returnedColor, long avoidK = -1);

#endif // RAY_TRACE_KD