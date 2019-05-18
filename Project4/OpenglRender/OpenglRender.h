/*
 *
 * RayTrace Software Package, release 4.gamma.  May 8, 2019.
 *
 * Author: Sam Buss
 *
 * Software accompanying a possible second edition to the book
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

#ifndef OPENGL_RENDER_H
#define OPENGL_RENDER_H

#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "EduPhong.h"
#include "../Graphics/Light.h"
#include "../Graphics/MaterialBase.h"
#include "GlGeomSphere.h"
#include "GlGeomCylinder.h"
#include "GlGeomTorus.h"
#include "GlShaderMgr.h"

class CameraView;
class Light;
class ViewableBase;
class ViewableBezierSet;
class ViewableCone;
class ViewableCylinder;
class ViewableEllipsoid;
class ViewableParallelepiped;
class ViewableParallelogram;
class ViewableSphere;
class ViewableTorus;
class ViewableTriangle;
class MaterialBase;
class SceneDescription;

#include <vector>

// The OpenglRenderer class controls rendering Graphics items from the RayTrace
//	software package.  Real time modification of the
//  scene is currently not well supported. Texture mapping is not supported at all!
//
// It is used as follows:
//		0. Set up the window, initialize OpenGL (say, with GLFW/GLEW).  
//         Do this yourself.
//		1. Call InitScene() with the Scene Description.  This can be split
//            into separate calls to 
//          a. InitView or SetCameraView
//          b. InitLights
//          c. InitMaterials
//          d. InitViewables
//      2. Each time you wish to render, call
//          a. Call SetCameraView() or InitView() only if the view has changed.
//          b. RenderScene() every time you want to re-render,
class OpenglRenderer {

public:
    OpenglRenderer() { SetMeshCounts(8); }

    void InitScene(const SceneDescription& theScene);
    void RenderScene();

	// Set mesh resolutions 
    // Optionally set whether to mesh flat faces (triangles, parallelograms, etc.), default is no.    
    // Defaults to meshRes=8, as set by the constructor.
    // To change, call SetMeshCounts BEFORE calling InitScene.
    void SetMeshCounts(int meshRes, bool meshFlats = false);

	// Multiplies all material coefficients by this factor. Default = 1.
	// Compensates for lack of bounces adding more brightness.
    // Can be called after the InitScene() or InitMaterials() routines.
    void SetBrightnessFactor(double factor) { BrightnessMultiplier = factor; }

    void SetupOpenGL();                         // Set OpenGL options needed for rendering (optional to call)
    void SetCameraView( const CameraView& theCameraView );
    void InitView(const SceneDescription& theScene);
    void InitLights(const SceneDescription& theScene);
    void InitMaterials(const SceneDescription& theScene);
    void InitViewables(const SceneDescription& theScene);
	
private:
    int MeshRes;
	bool MeshFlats;         // Control whether triangles should be further meshed.

    GlGeomSphere BaseSphere;
    int BaseSphereNumElts;
    int BaseSphereNumVerts;
    float* BaseSphereVerts = 0;
    unsigned int* BaseSphereElts = 0;

    GlGeomCylinder BaseCylinder;
    int BaseCylinderNumElts;
    int BaseCylinderNumVerts;
    float* BaseCylinderVerts = 0;
    unsigned int* BaseCylinderElts = 0;
    int BaseCylinderDiskNumVerts;

    GlGeomTorus BaseTorus;
    int BaseTorusNumElts;
    int BaseTorusNumVerts;
    float* BaseTorusVerts = 0;
    unsigned int* BaseTorusElts = 0;

	double BrightnessMultiplier;
	VectorR3 GlobalAmbientLight;
	VectorR3 BackgroundColor;
    Light SceneLights[phMaxNumLights];  // Copied from theScene

    phGlobal GlobalLightInfo;
    phLight LightInfo[phMaxNumLights];

    LinearMapR4 ModelViewMatrix;
    LinearMapR4 ProjectionMatrix;

    unsigned int theVAO = 0;
    unsigned int theVBO = 0;
    unsigned int theEBO = 0;

    std::vector<float> VBOdata;
    std::vector<unsigned int> EBOdata;
    class RenderCommand {
    public:
        const MaterialBase* materialFront;
        const MaterialBase* materialBack;      // Currently unused
        int numTriangles;                   // Set to zero for the final entry
        int startElement;                   // Size of array for the final entry
    };
    std::vector<RenderCommand> RenderCmds;
    void AddVertPosNormal(const VectorR3& pos, const VectorR3& normal);
    void AddRenderCommand(int numTriangles, const MaterialBase* matFront, const MaterialBase* matBack);
    void AddRectangle(const VectorR3& vertA, const VectorR3& vertB, const VectorR3& vertC, 
        const VectorR3& vertD, const VectorR3& normal);

private:
    static bool OpenGLisSetup;
    static unsigned int ShaderProgramRT;       // Shader program handle
    static unsigned int VertPos_LocRT;          // Location of vertex position in shader program
    static unsigned int VertNormal_LocRT;       // Location of vertex normal in shader program.
    static int ProjMat_LocRT;                   // Location of uniform projection matrix in shader program
    static int ModelViewMat_LocRT;              // Location of uniform model view matrix in shader program

    static void SetupShaderProgram();
    void SetupOpenGlBuffers();

    virtual void Load_Vbo_Ebo_Mats(const ViewableBase& bezierSet);
    void Load_Vbo_Ebo_Mats(const ViewableBezierSet& bezierSet);
    void Load_Vbo_Ebo_Mats(const ViewableCone& object);
    void Load_Vbo_Ebo_Mats(const ViewableCylinder& object);
    void Load_Vbo_Ebo_Mats(const ViewableEllipsoid& object);
    void Load_Vbo_Ebo_Mats(const ViewableParallelepiped& object);
    void Load_Vbo_Ebo_Mats(const ViewableParallelogram& object);
    void Load_Vbo_Ebo_Mats(const ViewableSphere& object);
    void Load_Vbo_Ebo_Mats(const ViewableTorus& object);
    void Load_Vbo_Ebo_Mats(const ViewableTriangle& object);

    int MeshResFlats() const { return MeshFlats ? MeshFlats : 1; }

	void CalcConeBasePt( double theta, const VectorR3& baseN, double baseD, 
						 const VectorR3& apex, const VectorR3& axisC, 
						 const VectorR3& axisA, const VectorR3& axisB, 
						 VectorR3* basePt1, VectorR3* normal1 );
	void CalcCylBasePt( double theta, const VectorR3& baseN, double baseD, 
						const VectorR3& center, const VectorR3& axisC, 
						const VectorR3& axisA, const VectorR3& axisB, 
						const VectorR3& topN, double topD, 
						VectorR3* basePt1, VectorR3* normal1 );
	void CalcCylTopPt ( double theta, const VectorR3& baseN, double baseD, 
						const VectorR3& center, const VectorR3& axisC, 
						const VectorR3& axisA, const VectorR3& axisB, 
						const VectorR3& topN, double topD, 
						VectorR3* basePt1 );

	void DrawLadder( const VectorR3& basePt1, const VectorR3& basePt2, 
					 const VectorR3& topPt1, const VectorR3& topPt2, 
					 const VectorR3& normal1, const VectorR3& normal2 );
};

#endif	// GLFW_RENDER_H