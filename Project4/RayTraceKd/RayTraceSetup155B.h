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

#ifndef RAY_TRACE_SETUP_155B
#define RAY_TRACE_SETUP_155B

class SceneDescription;

// Here are the data structure that holds information about the scene

extern SceneDescription TheScene155B;

// Routines that load the data into the scene description:
void SetupScene155B();         // Use this one to setup the whole scene

// Helper routines called by SetUpScene2()
void SetUpMaterials155B(SceneDescription& scene);
void SetUpLights155B(SceneDescription& scene);
void SetUpViewableObjects155B(SceneDescription& scene);

#endif // RAY_TRACE_SETUP_155B