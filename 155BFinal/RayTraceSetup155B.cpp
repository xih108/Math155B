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

#include "RayTraceSetup155B.h"

#include "../Graphics/Material.h"
#include "../Graphics/ViewableSphere.h"
#include "../Graphics/ViewableEllipsoid.h"
#include "../Graphics/ViewableCone.h"
#include "../Graphics/ViewableTorus.h"
#include "../Graphics/ViewableTriangle.h"
#include "../Graphics/ViewableParallelogram.h"
#include "../Graphics/ViewableCylinder.h"
#include "../Graphics/ViewableParallelepiped.h"
#include "../Graphics/ViewableBezierSet.h"
#include "../Graphics/TextureCheckered.h"
#include "../Graphics/TextureBilinearXform.h"
#include "../Graphics/TextureSequence.h"
#include "../Graphics/Light.h"
#include "../Graphics/CameraView.h"
#include "../RaytraceMgr/SceneDescription.h"

// ******************************************
SceneDescription TheScene155B;			// The scene as created by the routines below
// ******************************************

void SetupScene155B()
{
    SetUpMaterials155B(TheScene155B);
    SetUpLights155B(TheScene155B);
    SetUpViewableObjects155B(TheScene155B);
}

void SetUpMaterials155B(SceneDescription& scene) {
    // Initialize Array of Materials

    // First material for the floor (yellow)
    Material* mat0 = new Material;
    scene.AddMaterial(mat0);
    mat0->SetColorAmbientDiffuse(0.5, 0.5, 0.0);
    mat0->SetColorSpecular(0.8, 0.8, 0.8);
    mat0->SetColorReflective(0.6, 0.6, 0.6);
    mat0->SetShininess(512);                        // Specular exponent

    // Second material for the floor (bright blue)
    Material* mat1 = new Material;
    scene.AddMaterial(mat1);
    mat1->SetColorAmbientDiffuse(0.0, 0.2, 0.8);
    mat1->SetColorSpecular(0.8, 0.8, 0.8);
    mat1->SetColorReflective(0.6, 0.6, 0.6);
    mat1->SetShininess(512);

    // Material for side walls, dark yellow
    Material* mat2 = new Material;
    scene.AddMaterial(mat2);
    mat2->SetColorAmbientDiffuse(0.3, 0.3, 0.0);
    mat2->SetColorSpecular(0.8, 0.8, 0.8);
    mat2->SetColorReflective(0.8, 0.8, 0.4);
    mat2->SetShininess(160);

    // White. Goes on back wall and a sphere under a texture map
    Material* mat3 = new Material;
    scene.AddMaterial(mat3);
    mat3->SetColorAmbientDiffuse(0.6, 0.6, 0.6);
    mat3->SetColorSpecular(0.4, 0.4, 0.4);
    mat3->SetColorReflective(0.1, 0.1, 0.1);
    mat3->SetShininess(80);
    //mat3->SetUseFresnel(true);        // Default is to not use Fresnel approximation 
    //mat3->SetUseFresnel(true);        // Default is to not use Fresnel approximation 

    // Black, with a high gloss
    Material* mat4 = new Material;
    scene.AddMaterial(mat4); 
    mat4->SetColorAmbientDiffuse(0.0, 0.0, 0.0);
    mat4->SetColorSpecular(0.95, 0.95, 0.95);
    mat4->SetColorReflective(0.0, 0.0, 0.0);
    mat4->SetShininess(160);

    // Glass with a specular highlight
    Material* mat5 = new Material;
    scene.AddMaterial(mat5);
    mat5->SetColorAmbientDiffuse(0.0, 0.0, 0.0);
    mat5->SetColorSpecular(0.3, 0.3, 0.3);
    mat5->SetColorReflective(0.3, 0.3, 0.3);
    mat5->SetColorTransmissive(1.0, 1.0, 1.0);
    mat5->SetShininess(512);
    mat5->SetIndexOfRefraction(1.5);	// Glass!

    // A near perfect mirror
    Material* mat6 = new Material;
    scene.AddMaterial(mat6);
    mat6->SetColorAmbientDiffuse(0.05, 0.05, 0.05);
    mat6->SetColorSpecular(0.6, 0.6, 0.6);
    mat6->SetColorReflective(0.95, 0.95, 0.95);
    mat6->SetShininess(160);

	Material* mat7 = new Material;
	scene.AddMaterial(mat7);
	mat7->SetColorAmbientDiffuse(0.4, 0.4, 0.4);
	mat7->SetColorSpecular(0.3, 0.3, 0.3);
	mat7->SetColorReflective(0.05, 0.05, 0.05);
	mat7->SetShininess(160);
}


void SetUpLights155B(SceneDescription& scene) {

    // Global ambient light and the background color are set here.
    scene.SetGlobalAmbientLight(0.2, 0.2, 0.2);
    scene.SetBackGroundColor(0.0, 0.0, 0.0);

    // Initialize Array of Lights
    // Both lights are white, but with no ambient component

    Light* myLight0 = new Light();
    scene.AddLight(myLight0);
    myLight0->SetColorAmbient(0.0, 0.0, 0.0);
    myLight0->SetColorDiffuse(1.0, 1.0, 1.0);
    myLight0->SetColorSpecular(1.0, 1.0, 1.0);
    myLight0->SetPosition(15.0, 30.0, 25.0);

    Light* myLight1 = new Light();
    scene.AddLight(myLight1);
    myLight1->SetColorAmbient(0.0, 0.0, 0.0);
    myLight1->SetColorDiffuse(1.0, 1.0, 1.0);
    myLight1->SetColorSpecular(1.0, 1.0, 1.0);
    myLight1->SetPosition(-15.0, 30.0, 25.0);

	Light* lamp = new Light();
	scene.AddLight(lamp);
	lamp->SetColorAmbient(0.0, 0.0, 0.0);
	lamp->SetColorDiffuse(1.0, 1.0, 1.0);
	lamp->SetColorSpecular(1.0, 1.0, 1.0);
	lamp->SetPosition(-2.3, 7.1, -3.0);
	lamp->SetDirectional(1.0,-1.0,0.0);

}

void SetUpViewableObjects155B(SceneDescription& scene) {

    // Fill the scene with viewable objects

    // Vertices of two rectangles
    //   Consists of three verts in CCW order (4th vertex is generated automatically to make a rectangle
    //float FloorVerts[3][3] = { { -8.0f,0.0f,6.0f },{ 8.0f,0.0f,6.0f },{ 8.0f,0.0f, -6.0f } };
    float BackWallVerts[3][3] = { { -8.0f,0.0f,-6.0f },{ 8.0f,0.0f,-6.0f },{ 8.0f,10.0f,-6.0f } };

    // Vertices for the two side triangles - in CCW order
    float LeftWall[3][3] = { { -8.0f,0.0f,6.0f },{ -8.0f,0.0f,-6.0f },{ -8.0f,10.0f,-6.0f } };
    float RightWall[3][3] = { { 8.0f,0.0f,6.0f },{ 8.0f,0.0f,-6.0f },{ 8.0f,10.0f,-6.0f } };

    // Flat plane (the floor)
    /*ViewableParallelogram* vp;
    vp = new ViewableParallelogram();
    vp->Init(&FloorVerts[0][0]);
    vp->SetMaterial(&scene.GetMaterial(0));
    scene.AddViewable(vp);*/
	

    // Base material on floor is material 0, as set by SetMaterial
    //   Overlay a second material to make a checkerboard texture
    /*TextureCheckered* txchecked = scene.NewTextureCheckered();
    txchecked->SetMaterial1(&scene.GetMaterial(1));
    txchecked->SetWidths(1.0/15.0, 1.0/9.0);
    vp->TextureMap(txchecked);                      // This line adds the texture map "texchecked" to the floor ("vp")
	*/


    // Back wall

    ViewableParallelogram *vw = new ViewableParallelogram();
    vw->Init(&BackWallVerts[0][0]);
    vw->SetMaterial(&scene.GetMaterial(3));     // Underlying material is white
    scene.AddViewable(vw);
	
	TextureCheckered* txchecked = scene.NewTextureCheckered();
	txchecked->SetMaterial1(&scene.GetMaterial(7));
	txchecked->SetWidths(1.0 / 15.0, 1.0 / 9.0);
	vw->TextureMap(txchecked);

    // Left wall (triangular)
	ViewableParallelogram *vt = new ViewableParallelogram();
    vt = new ViewableParallelogram();
    vt->Init(&LeftWall[0][0]);
    vt->SetMaterial(&scene.GetMaterial(3));
    scene.AddViewable(vt);
	vt->TextureMap(txchecked);

    // Right wall (triangular)
	ViewableParallelogram *vt1 = new ViewableParallelogram();
    vt1 = new ViewableParallelogram();
    vt1->Init(&RightWall[0][0]);
    vt1->SetMaterial(&scene.GetMaterial(3));
    scene.AddViewable(vt1);
	vt1-> TextureMap(txchecked);
	
	
    // Left glass sphere
    
    // Right texture mapped sphere
   /* vs = new ViewableSphere();
    vs->SetCenter(5.5, 2.0, 3.0);
    vs->SetRadius(2.0);
    vs->SetMaterial(&scene.GetMaterial(3));
    scene.AddViewable(vs);
    txRgb = scene.NewTextureRgbImage("earth_clouds.bmp");
    vs->TextureMap(txRgb);                      // Apply the RGB Image texture to the sphere.
    vs->SetuvCylindrical();		              // SetuvSpherical is the default

    // Back mirrored cylinder
    ViewableCylinder* vCyl = new ViewableCylinder();
    vCyl->SetCenter(0.0, 2.4, -4.0);
    vCyl->SetHeight(4.0);
    vCyl->SetRadius(1.5);
    vCyl->SetMaterial(&scene.GetMaterial(6));
    scene.AddViewable(vCyl);*/

    // Back flat black cylinder
   /* vCyl = new ViewableCylinder();
    vCyl->SetCenter(0.0, 0.2, -4.0);
    vCyl->SetHeight(0.4);
    vCyl->SetRadius(2.0);
    vCyl->SetMaterial(&scene.GetMaterial(4));
    scene.AddViewable(vCyl);*/

    // Torus
    /*ViewableTorus* vTorus = new ViewableTorus();
    vTorus->SetCenter(0.0, 1.0, 3.0);
    vTorus->SetRadii(2.0, 0.75);
    vTorus->SetAxis(VectorR3(0.0, 1.0, 0.0));
    vTorus->SetMaterial(&scene.GetMaterial(5));     
    scene.AddViewable(vTorus);*/

    // Ellipsoid
   /* ViewableEllipsoid* vEllip = new ViewableEllipsoid();
    vEllip->SetCenter(-6.0, 2.0, -8.0);
    vEllip->SetRadii(1.5, 0.7, 0.3);
    vEllip->SetAxes(VectorR3(-1.0, 0.0, 1.0), VectorR3(0.0,1.0,0.0));
    vEllip->SetMaterial(&scene.GetMaterial(0));
    scene.AddViewable(vEllip);*/

	
	/*ViewableBezierSet* vBe = new ViewableBezierSet();
	VectorR4 controlPoints[] = {
		VectorR4(-1,0,2,1),VectorR4(0,0,1,0),VectorR4(1,0,2,1),
		VectorR4(-2,1,2,1),VectorR4(0,0,2,0),VectorR4(2,1,2,1),
		VectorR4(-1 / 2,3 / 2,2,1),VectorR4(0,0,1 / 2,0),VectorR4(1 / 2,3 / 2,2,1),
		VectorR4(-1,2,2,1),VectorR4(0,0,1,0),VectorR4(1,2,2,1) , };
	VectorR4 controlPoints1[] = {
		VectorR4(-1,0,2,1),VectorR4(0,0,-1,0),VectorR4(1,0,2,1),
		VectorR4(-2,1,2,1),VectorR4(0,0,-2,0),VectorR4(2,1,2,1),
		VectorR4(-1 / 2,3 / 2,2,1),VectorR4(0,0,-1 / 2,0),VectorR4(1 / 2,3 / 2,2,1),
		VectorR4(-1,2,2,1),VectorR4(0,0,-1,0),VectorR4(1,2,2,1) , };
	vBe->AddPatch(3, 4, controlPoints);
	//vBe->AddPatch(3, 4, controlPoints1);
	vBe->SetMaterial(&scene.GetMaterial(3));
	TextureRgbImage* txRgb = scene.NewTextureRgbImage("mirror.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vBe->TextureMap(txRgb);
	scene.AddViewable(vBe);*/


	ViewableBezierSet* vBe = new ViewableBezierSet();
	
	VectorR4 controlPoints[] = {
		VectorR4(-6,0,2,1),VectorR4(0,0,1,0),VectorR4(-4,0,2,1),
		VectorR4(-7,1,2,1),VectorR4(0,0,2,0),VectorR4(-3,1,2,1),
		VectorR4(-5.5,1.5,2,1),VectorR4(0,0,0.5,0),VectorR4(-4.5,1.5,2,1),
		VectorR4(-6,2,2,1),VectorR4(0,0,1,0),VectorR4(-4,2,2,1) , };
	VectorR4 controlPoints1[] = {
		VectorR4(-6,0,2,1),VectorR4(0,0,-1,0),VectorR4(-4,0,2,1),
		VectorR4(-7,1,2,1),VectorR4(0,0,-2,0),VectorR4(-3,1,2,1),
		VectorR4(-5.5,1.5,2,1),VectorR4(0,0, -0.5,0),VectorR4(-4.5,1.5,2,1),
		VectorR4(-6,2,2,1),VectorR4(0,0,-1,0),VectorR4(-4,2,2,1) , };
	vBe->AddPatch(3, 4, controlPoints);
	vBe->AddPatch(3, 4, controlPoints1);
	vBe->SetMaterial(&scene.GetMaterial(3));
	TextureRgbImage* txRgb = scene.NewTextureRgbImage("vase.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vBe->TextureMap(txRgb);
	scene.AddViewable(vBe);

	ViewableCylinder* vc = new ViewableCylinder();
	vc->SetCenter(-5,2,2);
	vc->SetRadius(1);
	vc->SetHeight(0.1);
	vc->SetMaterial(&scene.GetMaterial(3));
	txRgb = scene.NewTextureRgbImage("mirror.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vBe->TextureMap(txRgb);
	scene.AddViewable(vBe);
	
	//vBe->SetBoundingSphereCenter(-6.0, 2.0, -8.0);
	
    // Use a command like the following to get a handle to an object

	ViewableParallelepiped *vtable = new ViewableParallelepiped();
	vtable->SetVertices(VectorR3(-8.0f, -1.0f, 6.0f), VectorR3(8.0f, -1.0f, 6.0f), VectorR3(-8.0f, 0.0f, 6.0f), VectorR3(-8.0f, -1.0f, -6.0f));
	vtable->SetMaterial(&scene.GetMaterial(3));
	scene.AddViewable(vtable);

	txRgb = scene.NewTextureRgbImage("table.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vtable->TextureMap(txRgb);

	ViewableParallelepiped *vleg = new ViewableParallelepiped();
	vleg->SetVertices(VectorR3(-1.5f, -5.0f, 1.5f), VectorR3(1.5f, -5.0f, 1.5f), VectorR3(-1.5f, -1.0f, 1.5f), VectorR3(-1.5f, -5.0f, -1.5f));
	vleg->SetMaterial(&scene.GetMaterial(3));
	scene.AddViewable(vleg);
	vleg->TextureMap(txRgb);

	/*ViewableSphere* vs;
	vs = new ViewableSphere();
	vs->SetCenter(-5.5, 2.0, 3.0);
	vs->SetRadius(1.0);
	vs->SetMaterial(&scene.GetMaterial(5));
	int leftSphereIdx = scene.AddViewable(vs);*/

	float mirror[3][3] = { { 5.0f,0.0f,-3.0f },{ 5.0+sqrt(3.0f),0.0f,-2.0f },{ 7, 3*sqrt(3.0f)/2, -2.0f } };
	ViewableParallelogram* vmirror;
	vmirror = new ViewableParallelogram();
	vmirror->Init(& mirror[0][0]);
	vmirror->SetMaterialFront(&scene.GetMaterial(6));

	vmirror->SetMaterialBack(&scene.GetMaterial(3));
	scene.NewTextureRgbImage("mirror.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vmirror->TextureMap(txRgb);
	scene.AddViewable(vmirror);

	float mirrorf[3][3] = { { 3.0f,0.1f,-5.0f },{ 5.0f,0.1f,-3.0f },{ 6.0f,4.0f, -4.0f } };
	/*ViewableParallelepiped* vmirrorf = new ViewableParallelepiped();
	vmirrorf->SetVertices(VectorR3(5.1f,0.0f,-3.0f), VectorR3( 5.1f,0.0f, -3.0f ), VectorR3(6.1f, 4.1f, -4.0f), VectorR3(3.0f, 0.0f, -5.1f));
	vmirrorf->SetMaterial(&scene.GetMaterial(0));
	scene.AddViewable(vmirrorf);
	*/
	// Back mirrored cylinder
	ViewableCylinder* vmirrorf1 = new ViewableCylinder();
	vmirrorf1->SetCenter(6.2, 0.5, -2.6);
	vmirrorf1->SetHeight(2.0);
	vmirrorf1->SetRadius(0.1);
	vmirrorf1->SetMaterial(&scene.GetMaterial(3));
	scene.NewTextureRgbImage("mirror.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vmirrorf1->TextureMap(txRgb);
	scene.AddViewable(vmirrorf1); 

	ViewableParallelepiped *vcmp = new ViewableParallelepiped();
	vcmp->SetVertices(VectorR3(-3.0f, 1.5f, 0.0f), VectorR3(3.0f, 1.5f, 0.0f), VectorR3(-3.0f, 5.7f, 0.0f), VectorR3(-3.0f, 1.5f, -0.5f));
	vcmp->SetMaterial(&scene.GetMaterial(3));
	scene.AddViewable(vcmp);
    txRgb = scene.NewTextureRgbImage("monitor.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vcmp->TextureMap(txRgb);                      // Apply the RGB Image texture to the back wall.
	

	ViewableParallelepiped *vbase = new ViewableParallelepiped();
	vbase->SetVertices(VectorR3(-1.5f, 0.0f, 0.2f), VectorR3(1.5f, 0.0f, 0.2f), VectorR3(-1.5f, 0.2f, 0.2f), VectorR3(-1.5f, 0.0f, -1.8f));
	vbase->SetMaterialOuter(&scene.GetMaterial(3));
	scene.AddViewable(vbase);
	txRgb = scene.NewTextureRgbImage("monitor.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vbase->TextureMap(txRgb);

	ViewableParallelepiped *vbase1 = new ViewableParallelepiped();
	vbase1->SetVertices(VectorR3(-0.7f, 0.2f, -0.5f), VectorR3(0.7f, 0.2f, -0.5f), VectorR3(-0.7f, 3.0f, -0.5f), VectorR3(-0.7f, 0.2f, -0.7f));
	vbase1->SetMaterialOuter(&scene.GetMaterial(3));
	scene.AddViewable(vbase1);
	txRgb = scene.NewTextureRgbImage("mirror.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vbase1->TextureMap(txRgb);

	float Screen[3][3] = { { -2.8f,1.7f,0.01f },{ 2.8f,1.7f,0.01f },{ 2.8f, 5.5f,0.01f } };
	ViewableParallelogram *vscreen = new ViewableParallelogram();
	vscreen -> Init(&Screen[0][0]);
	vscreen->SetMaterial(&scene.GetMaterial(4));
	scene.AddViewable(vscreen);

	ViewableParallelepiped *vkey = new ViewableParallelepiped();
	vkey->SetVertices(VectorR3(-2.0f, 0.0f, 3.0f), VectorR3(2.0f, 0.0f, 3.0f), VectorR3(-2.0f, 0.2f, 3.0f), VectorR3(-2.0f, 0.1f, 1.5f));
	vkey->SetMaterialOuter(&scene.GetMaterial(3));
	scene.AddViewable(vkey);
	
	txRgb = scene.NewTextureRgbImage("keyboard.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vkey->TextureMap(txRgb);

	ViewableParallelepiped *vs1 = new ViewableParallelepiped();
	vs1->SetVertices(VectorR3(-1.8f, 0.0f, 1.7f), VectorR3(-1.7f, 0.0f, 1.7f), VectorR3(-1.8f, 0.09f, 1.7f), VectorR3(-1.8f, 0.0f, 1.6f));
	vs1->SetMaterialOuter(&scene.GetMaterial(3));
	scene.AddViewable(vs1);
	txRgb = scene.NewTextureRgbImage("monitor.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vs1->TextureMap(txRgb);


	ViewableParallelepiped *vs2 = new ViewableParallelepiped();
	vs2->SetVertices(VectorR3(1.8f, 0.0f, 1.7f), VectorR3(1.7f, 0.0f, 1.7f), VectorR3(1.8f, 0.09f, 1.7f), VectorR3(1.8f, 0.0f, 1.6f));
	vs2->SetMaterialOuter(&scene.GetMaterial(3));
	scene.AddViewable(vs2);
	txRgb = scene.NewTextureRgbImage("monitor.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vs2->TextureMap(txRgb);

	ViewableCylinder* vlbase = new ViewableCylinder();
	vlbase->SetCenter(-5.5, 0.1, -3.0);
	vlbase->SetHeight(0.2);
	vlbase->SetRadius(1.52);
	vlbase->SetMaterial(&scene.GetMaterial(3));
	txRgb = scene.NewTextureRgbImage("lamp.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vlbase->TextureMap(txRgb);
	scene.AddViewable(vlbase);

	ViewableCylinder* vls1 = new ViewableCylinder();
	vls1->SetCenter(-5.5, 2.75, -3.0);
	vls1->SetHeight(5);
	vls1->SetRadius(0.1);
	vls1->SetMaterial(&scene.GetMaterial(3));
	scene.AddViewable(vls1);
	vls1->TextureMap(txRgb);

	ViewableCylinder* vls2 = new ViewableCylinder();
	vls2->SetCenterAxis(1.0,1.0,0.0);
	vls2->SetCenter(-4.5, 6.0, -3.0);
	vls2->SetHeight(5);
	vls2->SetRadius(0.1);
	vls2->SetMaterial(&scene.GetMaterial(3));
	scene.AddViewable(vls2);
	vls2->TextureMap(txRgb);

	ViewableCylinder* vls3 = new ViewableCylinder();
	vls3->SetCenterAxis(1.0, -1.0, 0.0);
	vls3->SetCenter(-2.8, 7.6, -3.0);
	vls3->SetHeight(0.5);
	vls3->SetRadius(0.3);
	vls3->SetMaterial(&scene.GetMaterial(3));
	scene.AddViewable(vls3);
	vls3->TextureMap(txRgb);


	ViewableCone *vlamp = new ViewableCone();
	vlamp->SetCenterAxis(-1.0, 1.0, 0.0);
	vlamp->SetApex(-2.8, 7.6, -3.0);
	vlamp->SetHeight(1.5);
	vlamp->SetMaterialOuter(&scene.GetMaterial(3));
	vlamp->SetMaterialInner(&scene.GetMaterial(2));
	scene.AddViewable(vlamp);
	vlamp->TextureMap(txRgb);

	ViewableSphere *vlight = new ViewableSphere();
	vlight->SetCenter(-2.3, 7.1, -3.0);
	vlight->SetRadius(0.5);
	vlight->SetMaterial(&scene.GetMaterial(5));
	scene.AddViewable(vlamp);

	ViewableCylinder *vtb = new ViewableCylinder();
	vtb->SetCenterAxis(1.0,0.0,0.0);
	vtb->SetCenter(4.5,0.2,1.5);
	vtb->SetRadius(0.2);
	vtb->SetHeight(2);
	vtb->SetMaterial(&scene.GetMaterial(3));
	txRgb = scene.NewTextureRgbImage("mirror.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vtb->TextureMap(txRgb);
	scene.AddViewable(vtb);

	float Touch[3][3] = { { 3.5f, 0.0f, 3.0f },{ 5.5f, 0.0f, 3.0f },{ 5.5f, 0.4f, 1.5f } };
	ViewableParallelogram *vtp= new ViewableParallelogram();
	vtp->Init(&Touch[0][0]);
	vtp->SetMaterial(&scene.GetMaterial(3));
	txRgb = scene.NewTextureRgbImage("monitor.bmp");
	txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
	vtp->TextureMap(txRgb);
	scene.AddViewable(vtp);

}

